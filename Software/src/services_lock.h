/**
* This file is autogenerated by nRFgo Studio 1.21.2RC1.4
*/

#ifndef SETUP_MESSAGES_H__
#define SETUP_MESSAGES_H__

#include "hal_platform.h"
#include "aci.h"

// You have now chosen to upload the configuration to OTP on the device.
// This will result in a device that you can not modify afterwards. If this is your intention,
// remove this comment and the #error below
#error Generating configuration for OTP. Please verify usage by removing this error message from include file.

#define SETUP_ID 0
#define SETUP_FORMAT 3 /** nRF8001 D */
#define ACI_DYNAMIC_DATA_SIZE 169

/* Service: Battery - Characteristic: Battery Level - Pipe: TX */
#define PIPE_BATTERY_BATTERY_LEVEL_TX          1
#define PIPE_BATTERY_BATTERY_LEVEL_TX_MAX_SIZE 1

/* Service: Battery - Characteristic: Temperature Measurement - Pipe: TX */
#define PIPE_BATTERY_TEMPERATURE_MEASUREMENT_TX          2
#define PIPE_BATTERY_TEMPERATURE_MEASUREMENT_TX_MAX_SIZE 2

/* Service: IRSND - Characteristic: IRMP_Packet - Pipe: SET */
#define PIPE_IRSND_IRMP_PACKET_SET          3
#define PIPE_IRSND_IRMP_PACKET_SET_MAX_SIZE 7

/* Service: IRSND - Characteristic: IRMP_Packet - Pipe: RX_ACK_AUTO */
#define PIPE_IRSND_IRMP_PACKET_RX_ACK_AUTO          4
#define PIPE_IRSND_IRMP_PACKET_RX_ACK_AUTO_MAX_SIZE 7

/* Service: Environmental Sensing - Characteristic: Temperature Measurement - Pipe: TX */
#define PIPE_ENVIRONMENTAL_SENSING_TEMPERATURE_MEASUREMENT_TX          5
#define PIPE_ENVIRONMENTAL_SENSING_TEMPERATURE_MEASUREMENT_TX_MAX_SIZE 2

/* Service: Environmental Sensing - Characteristic: Humidity - Pipe: TX */
#define PIPE_ENVIRONMENTAL_SENSING_HUMIDITY_TX          6
#define PIPE_ENVIRONMENTAL_SENSING_HUMIDITY_TX_MAX_SIZE 2

/* Service: Environmental Sensing - Characteristic: Pressure - Pipe: TX */
#define PIPE_ENVIRONMENTAL_SENSING_PRESSURE_TX          7
#define PIPE_ENVIRONMENTAL_SENSING_PRESSURE_TX_MAX_SIZE 4


#define NUMBER_OF_PIPES 7

#define SERVICES_PIPE_TYPE_MAPPING_CONTENT {\
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_SET},   \
  {ACI_STORE_LOCAL, ACI_RX_ACK_AUTO},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
  {ACI_STORE_LOCAL, ACI_TX},   \
}

#define GAP_PPCP_MAX_CONN_INT 0xffff /**< Maximum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_MIN_CONN_INT  0xffff /**< Minimum connection interval as a multiple of 1.25 msec , 0xFFFF means no specific value requested */
#define GAP_PPCP_SLAVE_LATENCY 0
#define GAP_PPCP_CONN_TIMEOUT 0xffff /** Connection Supervision timeout multiplier as a multiple of 10msec, 0xFFFF means no specific value requested */

#define NB_SETUP_MESSAGES 29
#define SETUP_MESSAGES_CONTENT {\
    {0x00,\
        {\
            0x07,0x06,0x00,0x00,0x03,0x02,0x42,0x07,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x00,0x07,0x01,0x01,0x00,0x00,0x06,0x00,0x01,\
            0x91,0x0f,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x8f,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x1c,0xa3,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,0x00,0x06,0x00,0x00,0x00,0x06,0x03,0x90,0x01,0xff,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x10,0x38,0xff,0xff,0x02,0x58,0x0a,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x05,0x06,0x10,0x54,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x00,0x04,0x04,0x02,0x02,0x00,0x01,0x28,0x00,0x01,0x00,0x18,0x04,0x04,0x05,0x05,0x00,\
            0x02,0x28,0x03,0x01,0x02,0x03,0x00,0x00,0x2a,0x04,0x04,0x14,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x1c,0x08,0x00,0x03,0x2a,0x00,0x01,0x49,0x72,0x52,0x65,0x6d,0x6f,0x74,0x65,0x6d,0x69,\
            0x2e,0x63,0x6f,0x6d,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x38,0x05,0x05,0x00,0x04,0x28,0x03,0x01,0x02,0x05,0x00,0x01,0x2a,0x06,0x04,0x03,0x02,\
            0x00,0x05,0x2a,0x01,0x01,0x00,0x00,0x04,0x04,0x05,0x05,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x54,0x06,0x28,0x03,0x01,0x02,0x07,0x00,0x04,0x2a,0x06,0x04,0x09,0x08,0x00,0x07,0x2a,\
            0x04,0x01,0xff,0xff,0xff,0xff,0x00,0x00,0xff,0xff,0x04,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x70,0x02,0x02,0x00,0x08,0x28,0x00,0x01,0x01,0x18,0x04,0x04,0x02,0x02,0x00,0x09,0x28,\
            0x00,0x01,0x0f,0x18,0x04,0x04,0x05,0x05,0x00,0x0a,0x28,0x03,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0x8c,0x01,0x12,0x0b,0x00,0x19,0x2a,0x16,0x04,0x02,0x01,0x00,0x0b,0x2a,0x19,0x01,0x10,\
            0x46,0x14,0x03,0x02,0x00,0x0c,0x29,0x02,0x01,0x00,0x00,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xa8,0x04,0x05,0x05,0x00,0x0d,0x28,0x03,0x01,0x10,0x0e,0x00,0x1c,0x2a,0x16,0x00,0x03,\
            0x02,0x00,0x0e,0x2a,0x1c,0x01,0x00,0x00,0x06,0x04,0x08,0x07,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xc4,0x00,0x0f,0x29,0x04,0x01,0x0e,0xfe,0x00,0x00,0x01,0x00,0x00,0x46,0x14,0x03,0x02,\
            0x00,0x10,0x29,0x02,0x01,0x00,0x00,0x04,0x04,0x10,0x10,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xe0,0x11,0x28,0x00,0x01,0x1b,0x8a,0x73,0x10,0x9a,0xd4,0xfc,0xb0,0x57,0x40,0x0c,0x85,\
            0xa3,0x8f,0x84,0x91,0x04,0x04,0x13,0x13,0x00,0x12,0x28,0x03,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x20,0xfc,0x01,0x0a,0x13,0x00,0x1b,0x8a,0x73,0x10,0x9a,0xd4,0xfc,0xb0,0x57,0x40,0x0c,0x85,\
            0xdc,0xad,0x84,0x91,0x46,0x14,0x08,0x07,0x00,0x13,0xad,0xdc,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x18,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x04,0x08,0x07,0x00,0x14,0x29,0x04,\
            0x01,0x1b,0x00,0x00,0x00,0x01,0x00,0x00,0x04,0x04,0x02,0x02,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x34,0x00,0x15,0x28,0x00,0x01,0x1a,0x18,0x04,0x04,0x05,0x05,0x00,0x16,0x28,0x03,0x01,\
            0x12,0x17,0x00,0x1c,0x2a,0x16,0x04,0x03,0x02,0x00,0x17,0x2a,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x50,0x1c,0x01,0x2a,0x00,0x06,0x04,0x08,0x07,0x00,0x18,0x29,0x04,0x01,0x0e,0xfe,0x2f,\
            0x27,0x01,0x00,0x00,0x46,0x14,0x03,0x02,0x00,0x19,0x29,0x02,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x6c,0x01,0x00,0x00,0x04,0x04,0x05,0x05,0x00,0x1a,0x28,0x03,0x01,0x02,0x1b,0x00,0x1d,\
            0x2a,0x06,0x04,0x02,0x01,0x00,0x1b,0x2a,0x1d,0x01,0x0c,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0x88,0x04,0x05,0x05,0x00,0x1c,0x28,0x03,0x01,0x10,0x1d,0x00,0x6f,0x2a,0x16,0x00,0x03,\
            0x02,0x00,0x1d,0x2a,0x6f,0x01,0x00,0x00,0x06,0x04,0x08,0x07,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xa4,0x00,0x1e,0x29,0x04,0x01,0x06,0xfe,0x00,0x00,0x01,0x00,0x00,0x46,0x14,0x03,0x02,\
            0x00,0x1f,0x29,0x02,0x01,0x00,0x00,0x04,0x04,0x05,0x05,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x21,0xc0,0x20,0x28,0x03,0x01,0x12,0x21,0x00,0x6d,0x2a,0x16,0x04,0x05,0x04,0x00,0x21,0x2a,\
            0x6d,0x01,0x00,0x00,0x00,0x00,0x06,0x04,0x08,0x07,0x00,0x22,\
        },\
    },\
    {0x00,\
        {\
            0x19,0x06,0x21,0xdc,0x29,0x04,0x01,0x08,0xff,0x00,0x00,0x01,0x00,0x00,0x46,0x14,0x03,0x02,0x00,0x23,\
            0x29,0x02,0x01,0x00,0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x00,0x2a,0x19,0x01,0x00,0x02,0x04,0x00,0x0b,0x00,0x0c,0x2a,0x1c,0x01,0x00,0x02,0x04,\
            0x00,0x0e,0x00,0x10,0xad,0xdc,0x02,0x04,0x80,0x04,0x00,0x13,\
        },\
    },\
    {0x00,\
        {\
            0x1f,0x06,0x40,0x1c,0x00,0x00,0x2a,0x1c,0x01,0x00,0x02,0x04,0x00,0x17,0x00,0x19,0x2a,0x6f,0x01,0x00,\
            0x02,0x04,0x00,0x1d,0x00,0x1f,0x2a,0x6d,0x01,0x00,0x02,0x04,\
        },\
    },\
    {0x00,\
        {\
            0x07,0x06,0x40,0x38,0x00,0x21,0x00,0x23,\
        },\
    },\
    {0x00,\
        {\
            0x13,0x06,0x50,0x00,0x1b,0x8a,0x73,0x10,0x9a,0xd4,0xfc,0xb0,0x57,0x40,0x0c,0x85,0x00,0x00,0x84,0x91,\
        },\
    },\
    {0x00,\
        {\
            0x15,0x06,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
            0x00,0x00,\
        },\
    },\
    {0x00,\
        {\
            0x06,0x06,0xf0,0x00,0x83,0xf8,0x8e,\
        },\
    },\
}

#endif
