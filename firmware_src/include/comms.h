#ifndef COMMS_H
#define COMMS_H

/** Maximum size of a packet payload */
#define COMMS_BUFFER_MAX 32

/** State of the communication frame */
typedef enum {
  COMMS_STATE_OP,
  COMMS_STATE_PAYLOAD_LENGTH,
  COMMS_STATE_PAYLOAD,
  COMMS_STATE_COMPLETE
} CommsState_t;

/** Commands that can be processed */
typedef enum {
  PACKET_OP_NOOP,
  /** ASCII m */
  PACKET_OP_MOVEMENT = 109,
  /** ASCII p */
  PACKET_OP_PIN = 112,
  /** ASCII d */
  PACKET_OP_DEBUG = 100,
  /** ASCII r */
  PACKET_OP_READY = 114,
  /** ASCII t */
  PACKET_OP_TEST = 116
} CommsPacketOp_t;

/** argv[0] of the PACKET_OP_MOVEMENT command */
typedef enum {
  MOVEMENT_AXIS_X1,
  MOVEMENT_AXIS_X2,
  MOVEMENT_AXIS_Y1,
  MOVEMENT_AXIS_Z1,
#if defined(FARMDUINO_K15)
  MOVEMENT_AXIS_AUX
#endif
} MOVEMENT_AXIS_t;

typedef enum {
  PIN_ARG_LIGHTING,
  PIN_ARG_WATER,
  PIN_ARG_VACUUM,
#if defined(FARMDUINO_K15)
  PIN_ARG_P4,
  PIN_ARG_P5
#endif
} PIN_ARG_t;

/** Packet structure */
typedef struct {
  CommsState_t _state;
  CommsPacketOp_t op;
  uint8_t payloadLength;
  uint8_t _index;
  uint8_t payload[COMMS_BUFFER_MAX];
} CommsPacket_t;

#endif