/* Copyright (c) 2014, Nordic Semiconductor ASA

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

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
  With this project you have a starting point for adding your own application
  functionality.

  The following instructions describe the steps to be made on the Windows PC:

  -# Install the Master Control Panel on your computer. Connect the Master
  Emulator (nRF2739) and make sure the hardware drivers are installed.

*/

/* first include Arduino.h, the IDE includes it after irmp*.h ... */
#include "Arduino.h"
/* ... and then chokes on uintX_t ... */
#include <avr/power.h>
#include <avr/sleep.h>

// Glue Code to IRSND
#include "irsndadapter.h"

// Glue Code and State Machine to nRF8001
#include "Sensors/BatterySensorStateMachine.h"
#include "Sensors/SensorStateMachine.h"
#include "WatchdogManager.h"
#include "bleadapter.h"
//#include "Sensors/nrf8001TemperatureSensorStateMachine.h"
#include "Sensors/BME280TemperatureSensorStateMachine.h"

// Interrupt PIN used for Wakeup from nrf8001
#define RDYN_INTR_NO 0
volatile unsigned long wakeupCounter = 0;

BatterySensorStateMachine batterySensorSM(PIPE_BATTERY_BATTERY_LEVEL_TX);
// nrf8001TemperatureSensorStateMachine
// nRFTemperatureSensorSM(PIPE_ENVIRONMENTAL_SENSING_TEMPERATURE_MEASUREMENT_TX);
const uint8_t bmePipes[] = {PIPE_ENVIRONMENTAL_SENSING_TEMPERATURE_MEASUREMENT_TX, PIPE_ENVIRONMENTAL_SENSING_HUMIDITY_TX, PIPE_ENVIRONMENTAL_SENSING_PRESSURE_TX};
BME280TemperatureSensorStateMachine bmeTemperatureSensorSM(bmePipes);

/**
 * ISR for RDYN low events
 */
void rdyn_isr() {
  // This is a level interrupt that would fire again while the
  // signal is low. Thus, we need to detach the interrupt.
  detachInterrupt(RDYN_INTR_NO);
  ble_setWorkAvailable();
  ++wakeupCounter;
}

void ble_dataReceived_Cbk(uint8_t pipe, uint8_t* data, uint8_t len) {
  if (PIPE_IRSND_IRMP_PACKET_RX_ACK_AUTO == pipe) {
    receivedIRMPPacket(data);
  }
}

void ble_pipeEvent_Cbk() {
  bool batteryPipeAvailable =
      lib_aci_is_pipe_available(&aci_state, PIPE_BATTERY_BATTERY_LEVEL_TX);
  if (batteryPipeAvailable != batterySensorSM.isEnabled()) {
    Serial.print(F("Battery Service "));
    // Edge in some direction detected
    if (batteryPipeAvailable) {
      batterySensorSM.enable();
      incrementWatchdogEnableCount();
      batterySensorSM.startSampling();
    } else {
      batterySensorSM.disable();
      decrementWatchdogEnableCount();
      Serial.print(F("un"));
    }
    Serial.println(F("subscribed."));
  }

  bool temperaturePipeAvailable = lib_aci_is_pipe_available(
      &aci_state, PIPE_ENVIRONMENTAL_SENSING_TEMPERATURE_MEASUREMENT_TX);
  if (temperaturePipeAvailable != bmeTemperatureSensorSM.isEnabled()) {
    Serial.print(F("Temperature Service "));
    // Edge in some direction detected
    if (temperaturePipeAvailable) {
      bmeTemperatureSensorSM.enable();
      // nRFTemperatureSensorSM.enable();
      incrementWatchdogEnableCount();
      bmeTemperatureSensorSM.startSampling();
    } else {
      bmeTemperatureSensorSM.disable();
      // nRFTemperatureSensorSM.disable();
      decrementWatchdogEnableCount();
      Serial.print(F("un"));
    }
    Serial.println(F("subscribed."));
  }
  Serial.print(F("Temperature Pipe: "));
  Serial.println(temperaturePipeAvailable, DEC);
}

void ble_temperature_Cbk(uint16_t temperature) {
  // nRFTemperatureSensorSM.handleTemperatureEvent(temperature);
}

bool workAvailable() {
  return irsnd_is_busy() || ble_available() || wdg_expired() ||
         bmeTemperatureSensorSM.isDataPending() ||
         batterySensorSM.isDataPending();
}

/**
 * Put device into power-down mode to save as much energy as possible.
 *
 * The device will wake up again by interrupt, either the watch dog timer
 * or RDYN going low (= nRF8001 sends an event to device).
 */
void do_sleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  // Disable interrupts until we sleep to avoid race conditions
  // (interrupt firing before going to sleep would prevent MCU from
  // waking by interrupt).
  cli();
  if (workAvailable()) {
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

void setup(void) {
  Serial.begin(115200);
  // Wait until the serial port is available (useful only for the Leonardo)
  // As the Leonardo board is not reseted every time you open the Serial Monitor
#if defined(__AVR_ATmega32U4__)
  while (!Serial) {
  }
  delay(5000);  // 5 seconds delay for enabling to see the start up comments on
                // the serial board
#elif defined(__PIC32MX__)
  delay(1000);
#endif
  Serial.println(F("Arduino setup"));
  power_adc_disable();

  IRSND_adapter_init();

  ble_setup();

  bmeTemperatureSensorSM.init();

  // Install interrupt for RDYN line of nRF8001 for event handling.
  // We use a level-interrupt that can also fire in sleep mode to
  // wake up the Arduino when an event is received.
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(RDYN_INTR_NO, rdyn_isr, LOW);

  wdg_init();

  batterySensorSM.startSampling();
  batterySensorSM.disable();

  bmeTemperatureSensorSM.startSampling();
  bmeTemperatureSensorSM.disable();
}

void loop() {
  // Loop the BLE state machine. Returns true when there is additional work to
  // be done.
  if (ble_available()) {
    ble_loop();
  }

  // Perform Watchdog Events
  if (wdg_expired()) {
    Serial.println(F("Watchdog expired."));
    if (batterySensorSM.isEnabled()) {
      batterySensorSM.startSampling();
    }
    /*
    if (nRFTemperatureSensorSM.isEnabled()) {
      nRFTemperatureSensorSM.startSampling();
    }
    */
    if (bmeTemperatureSensorSM.isEnabled()) {
      bmeTemperatureSensorSM.startSampling();
    }
    wdg_reset_expiry();
  }

  // Perform possibly pending transmissions.
  batterySensorSM.transmitSample();
  // nRFTemperatureSensorSM.transmitSample();
  bmeTemperatureSensorSM.transmitSample();

  if (!workAvailable()) {
    attachInterrupt(RDYN_INTR_NO, rdyn_isr, LOW);
    // No event in the ACI Event queue
    // Arduino can go to sleep now
    Serial.println(F("Arduino going to sleep."));
    delay(100);  // Allow for completion of serial transmission
    do_sleep();
    Serial.print(F("Arduino woke up. Wakeup-Counter: "));
    Serial.println(wakeupCounter, DEC);
    // Wakeup from sleep from the RDYN line
  }
}
