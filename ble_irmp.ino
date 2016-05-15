/* Copyright (c) 2014, Nordic Semiconductor ASA

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/**
   My project template
*/

/** @defgroup my_project my_project
  @{
  @ingroup projects
  @brief Empty project that can be used as a template for new projects.

  @details
  This project is a firmware template for new projects.
  The project will run correctly in its current state, but does nothing.
  With this project you have a starting point for adding your own application functionality.

  The following instructions describe the steps to be made on the Windows PC:

  -# Install the Master Control Panel on your computer. Connect the Master Emulator
    (nRF2739) and make sure the hardware drivers are installed.

*/

/* use TimerOne http://arduino.cc/playground/Code/Timer1 for interrupts */
#include <TimerOne.h>
/* first include Arduino.h, the IDE includes it after irmp*.h ... */
#include "Arduino.h"
/* ... and then chokes on uintX_t ... */
#include <avr/sleep.h>
#include <avr/power.h>

extern "C" {
//#include <irmp.h>
#include <irsnd.h>
}

void printFrame(IRMP_DATA & frame);

/* F_INTERRUPTS is the interrupt frequency defined in irmpconfig.h */
#define US (1000000 / F_INTERRUPTS)

IRMP_DATA irmp_data;


#include <SPI.h>
#include <EEPROM.h>
#include <lib_aci.h>
#include <aci_setup.h>

/**
  Put the nRF8001 setup in the RAM of the nRF8001.
*/
#include "services.h"
/**
  Include the services_lock.h to put the setup in the OTP memory of the nRF8001.
  This would mean that the setup cannot be changed once put in.
  However this removes the need to do the setup of the nRF8001 on every reset.
*/

#define RDYN_INTR_NO 0
volatile bool ble_ready = false;
volatile unsigned long wakeupCounter = 0;


#ifdef SERVICES_PIPE_TYPE_MAPPING_CONTENT
static services_pipe_type_mapping_t
services_pipe_type_mapping[NUMBER_OF_PIPES] = SERVICES_PIPE_TYPE_MAPPING_CONTENT;
#else
#define NUMBER_OF_PIPES 0
static services_pipe_type_mapping_t * services_pipe_type_mapping = NULL;
#endif
static const hal_aci_data_t setup_msgs[NB_SETUP_MESSAGES] PROGMEM = SETUP_MESSAGES_CONTENT;

// aci_struct that will contain
// total initial credits
// current credit
// current state of the aci (setup/standby/active/sleep)
// open remote pipe pending
// close remote pipe pending
// Current pipe available bitmap
// Current pipe closed bitmap
// Current connection interval, slave latency and link supervision timeout
// Current State of the the GATT client (Service Discovery)
// Status of the bond (R) Peer address
static struct aci_state_t aci_state;
static hal_aci_evt_t aci_data;
static hal_aci_data_t aci_cmd;

/* Define how assert should function in the BLE library */
void __ble_assert(const char *file, uint16_t line)
{
  Serial.print("ERROR ");
  Serial.print(file);
  Serial.print(": ");
  Serial.print(line);
  Serial.print("\n");
  while (1);
}

/**
 * ISR for RDYN low events
 */
void rdyn_isr()
{
    // This is a level interrupt that would fire again while the
    // signal is low. Thus, we need to detach the interrupt.
    detachInterrupt(RDYN_INTR_NO);
    ble_ready = true;  
    ++wakeupCounter;
}


/**
 * Put device into power-down mode to save as much energy as possible.
 * 
 * The device will wake up again by interrupt, either the watch dog timer 
 * or RDYN going low (= nRF8001 sends an event to device).
 */
void do_sleep()
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    // Disable interrupts until we sleep to avoid race conditions
    // (interrupt firing before going to sleep would prevent MCU from
    // waking by interrupt).
    cli();
    if (ble_ready) {
        // Last chance to stay awake.
        sei();
    } else {
        sleep_enable();
        #ifdef DISABLE_BOD_WHILE_SLEEPING
        // Disabling brown-out detection while sleeping 
        // Saves about 25 uA.
        // BODS: Brown-out Detection Sleep
        // BODSE: Brown-out Detection Sleep Enable
        // This is a timed sequence:
        // First, BODS and BODSE must me set to one.
        // Then, BODS must be set to one and BODSE to zero
        // within four clock cycles. Then, BODS stays active three
        // clock cycles, so sleep_cpu() must be called within
        // three cycles after setting BODS.
        MCUCR = bit(BODS) | bit(BODSE);
        MCUCR = bit(BODS);
        #endif 
        // Enable interrupts again. It is guranteed that the next
        // command (entering sleep mode) is executed *before* an 
        // interrupt is fired (no race condition). From the data sheet:
        // "When using the SEI instruction to enable interrupts, 
        // the instruction following SEI will be executed
        // before any pending interrupts."
        sei();
        sleep_cpu();
        // Wake again after interrupt.
        sleep_disable();
    }
}
  
void printFrame(IRMP_DATA & frame) {
  Serial.print(F("P:"));
  Serial.print(frame.protocol, HEX);
  Serial.print(F(" A:"));
  Serial.print(frame.address, HEX);
  Serial.print(F(" C:"));
  Serial.print(frame.command, HEX);
  Serial.print(F(" "));
  Serial.println(frame.flags, HEX);
}

/* helper function: attachInterrupt wants void(), but irmp_ISR is uint8_t() */
void timerinterrupt()
{
  irsnd_ISR();          // call irsnd ISR
}

void setup(void)
{
  Serial.begin(115200);
  //Wait until the serial port is available (useful only for the Leonardo)
  //As the Leonardo board is not reseted every time you open the Serial Monitor
#if defined (__AVR_ATmega32U4__)
  while (!Serial)
  {}
  delay(5000);  //5 seconds delay for enabling to see the start up comments on the serial board
#elif defined(__PIC32MX__)
  delay(1000);
#endif
  Serial.println(F("Arduino setup"));
  power_adc_disable();

  /*irmp_init();
    Timer1.initialize(US);
    Timer1.attachInterrupt(timerinterrupt);
    Serial.println(F("IRMP Detector ready.\n\n"));
  */
  irsnd_init();
  Timer1.initialize(US);
  Timer1.attachInterrupt(timerinterrupt);

  if (NULL != services_pipe_type_mapping)
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = &services_pipe_type_mapping[0];
  }
  else
  {
    aci_state.aci_setup_info.services_pipe_type_mapping = NULL;
  }
  aci_state.aci_setup_info.number_of_pipes    = NUMBER_OF_PIPES;
  aci_state.aci_setup_info.setup_msgs         = (hal_aci_data_t*) setup_msgs;
  aci_state.aci_setup_info.num_setup_msgs     = NB_SETUP_MESSAGES;

  /*
    Tell the ACI library, the MCU to nRF8001 pin connections.
    The Active pin is optional and can be marked UNUSED
  */
  aci_state.aci_pins.board_name = BOARD_DEFAULT; //See board.h for details
  aci_state.aci_pins.reqn_pin   = 10;
  aci_state.aci_pins.rdyn_pin   = 2; // Changed to INT0
  aci_state.aci_pins.mosi_pin   = MOSI;
  aci_state.aci_pins.miso_pin   = MISO;
  aci_state.aci_pins.sck_pin    = SCK;

  aci_state.aci_pins.spi_clock_divider      = SPI_CLOCK_DIV8;//SPI_CLOCK_DIV8  = 2MHz SPI speed
  //SPI_CLOCK_DIV16 = 1MHz SPI speed

  aci_state.aci_pins.reset_pin              = 4;
  aci_state.aci_pins.active_pin             = UNUSED;
  aci_state.aci_pins.optional_chip_sel_pin  = UNUSED;

  aci_state.aci_pins.interface_is_interrupt = false;
  aci_state.aci_pins.interrupt_number       = 1;

  //We reset the nRF8001 here by toggling the RESET line connected to the nRF8001
  //and initialize the data structures required to setup the nRF8001
  lib_aci_init(&aci_state, false);

  // Install interrupt for RDYN line of nRF8001 for event handling.
  // We use a level-interrupt that can also fire in sleep mode to
  // wake up the Arduino when an event is received.
  attachInterrupt(RDYN_INTR_NO, rdyn_isr, LOW);
}

void receivedIRMPPacket(aci_evt_t * aci_evt) {
  Serial.print(F("IRMP Packet received\n"));
  memcpy(&irmp_data, &(aci_evt->params.data_received.rx_data.aci_data[0]), sizeof(IRMP_DATA));
  Serial.println(F("Received Frame: "));
  printFrame(irmp_data);
  
  if (!irsnd_is_busy()) {
    Serial.println(F("IRSND idle. Sending IR frame."));
    irsnd_send_data(&irmp_data, TRUE);
  } else {
    Serial.println(F("IRSND busy. Dropping IR frame."));
  }
}

void loop()
{
  static bool setup_required = false;

  // We enter the if statement only when there is a ACI event available to be processed
  if (lib_aci_event_get(&aci_state, &aci_data))
  {
    aci_evt_t * aci_evt;
    aci_evt = &aci_data.evt;
    switch (aci_evt->evt_opcode)
    {
      /**
        As soon as you reset the nRF8001 you will get an ACI Device Started Event
      */
      case ACI_EVT_DEVICE_STARTED:
        {
          aci_state.data_credit_available = aci_evt->params.device_started.credit_available;
          switch (aci_evt->params.device_started.device_mode)
          {
            case ACI_DEVICE_SETUP:
              /**
                When the device is in the setup mode
              */
              Serial.println(F("Evt Device Started: Setup"));
              setup_required = true;
              break;

            case ACI_DEVICE_STANDBY:
              Serial.println(F("Evt Device Started: Standby"));
              //See ACI Broadcast in the data sheet of the nRF8001
              //lib_aci_broadcast(0/* in seconds */, 0x0100 /* advertising interval 100ms */);
              lib_aci_connect(0, 0x0100);
              //While broadcasting (non_connectable) interval of 100ms is the minimum possible
              Serial.println(F("Broadcasting started"));
              //To stop the broadcasting before the timeout use the
              //lib_aci_radio_reset to soft reset the radio
              //See ACI RadioReset in the datasheet of the nRF8001
              break;
          }
        }
        break; //ACI Device Started Event

      case ACI_EVT_CMD_RSP:
        //If an ACI command response event comes with an error -> stop
        if (ACI_STATUS_SUCCESS != aci_evt->params.cmd_rsp.cmd_status)
        {
          //ACI ReadDynamicData and ACI WriteDynamicData will have status codes of
          //TRANSACTION_CONTINUE and TRANSACTION_COMPLETE
          //all other ACI commands will have status code of ACI_STATUS_SCUCCESS for a successful command
          Serial.print(F("ACI Command "));
          Serial.println(aci_evt->params.cmd_rsp.cmd_opcode, HEX);
          Serial.println(F("Evt Cmd respone: Error. Arduino is in an while(1); loop"));
          while (1);
        }
        break;

      case ACI_EVT_CONNECTED:
        Serial.println(F("Evt Connected"));
        break;

      case ACI_EVT_PIPE_STATUS:
        Serial.println(F("Evt Pipe Status"));
        break;

      case ACI_EVT_DISCONNECTED:
        if (ACI_STATUS_ERROR_ADVT_TIMEOUT == aci_evt->params.disconnected.aci_status)
        {
          Serial.println(F("Broadcasting timed out"));
        }
        else
        {
          Serial.println(F("Evt Disconnected. Link Loss"));
        }
        break;

      case ACI_EVT_DATA_RECEIVED:
        Serial.print(F("Data received on Pipe #: 0x"));
        Serial.println(aci_evt->params.data_received.rx_data.pipe_number, HEX);
        Serial.print(F("Length of data received: 0x"));
        Serial.println(aci_evt->len - 2, HEX);

        if (PIPE_IRSND_IRMP_PACKET_RX_ACK_AUTO == aci_evt->params.data_received.rx_data.pipe_number) {
          /* Received an IRMP packet */
          receivedIRMPPacket(aci_evt);
        }

        break;

      case ACI_EVT_HW_ERROR:
        Serial.println(F("HW error: "));
        Serial.println(aci_evt->params.hw_error.line_num, DEC);
        for (uint8_t counter = 0; counter <= (aci_evt->len - 3); counter++)
        {
          Serial.write(aci_evt->params.hw_error.file_name[counter]); //uint8_t file_name[20];
        }
        Serial.println();
        break;
    }
  }
  else
  {
    if (!setup_required) {
    Serial.println(F("No ACI Events available, no setup required."));
    ble_ready = false;
    attachInterrupt(RDYN_INTR_NO, rdyn_isr, LOW);
    // No event in the ACI Event queue
    // Arduino can go to sleep now
    Serial.println(F("Arduino going to sleep."));
    delay(100); // Allow for completion of serial transmission
    do_sleep();
    Serial.print(F("Arduino woke up. Wakeuo-Counter: "));
    Serial.println(wakeupCounter, DEC);
    // Wakeup from sleep from the RDYN line
    }
  }

  /* setup_required is set to true when the device starts up and enters setup mode.
     It indicates that do_aci_setup() should be called. The flag should be cleared if
     do_aci_setup() returns ACI_STATUS_TRANSACTION_COMPLETE.
  */
  if (setup_required)
  {
    if (SETUP_SUCCESS == do_aci_setup(&aci_state))
    {
      setup_required = false;
    }
  }
}

