/**
 * FSAE Library 32bit CAN Header
 *
 * Processor:   PIC32MZ2048EFM100
 * Compiler:    Microchip XC32
 * Author:      Andrew Mass
 * Created:     2015-2016
 */
#ifndef FSAE_CAN_32_H
#define FSAE_CAN_32_H

#include <xc.h>
#include <sys/kmem.h>
#include <sys/types.h>

/**
 * CanRxMessageBuffer struct
 */

// CMSGSID
typedef struct {
  unsigned SID:11;
  unsigned FILHIT:5;
  unsigned CMSGTS:16;
} rxcmsgsid;

// CMSGEID
typedef struct {
  unsigned DLC:4;
  unsigned RB0:1;
  unsigned :3;
  unsigned RB1:1;
  unsigned RTR:1;
  unsigned EID:18;
  unsigned IDE:1;
  unsigned SRR:1;
  unsigned :2;
} rxcmsgeid;

// CMSGDATA0
typedef struct {
  unsigned Byte0:8;
  unsigned Byte1:8;
  unsigned Byte2:8;
  unsigned Byte3:8;
} rxcmsgdata0;

// CMSGDATA1
typedef struct {
  unsigned Byte4:8;
  unsigned Byte5:8;
  unsigned Byte6:8;
  unsigned Byte7:8;
} rxcmsgdata1;

// CanRxMessageBuffer
typedef union uCanRxMessageBuffer {
  struct {
    rxcmsgsid CMSGSID;
    rxcmsgeid CMSGEID;
    rxcmsgdata0 CMSGDATA0;
    rxcmsgdata1 CMSGDATA1;
  };
  int messageWord[4];
} CanRxMessageBuffer;

/**
 * CanTxMessageBuffer struct
 */

// CMSGSID
typedef struct {
  unsigned SID:11;
  unsigned :21;
} txcmsgsid;

// CMSGEID
typedef struct {
  unsigned DLC:4;
  unsigned RB0:1;
  unsigned :3;
  unsigned RB1:1;
  unsigned RTR:1;
  unsigned EID:18;
  unsigned IDE:1;
  unsigned SRR:1;
  unsigned :2;
} txcmsgeid;

// CMSGDATA0
typedef struct {
  unsigned Byte0:8;
  unsigned Byte1:8;
  unsigned Byte2:8;
  unsigned Byte3:8;
} txcmsgdata0;

// CMSGDATA1
typedef struct {
  unsigned Byte4:8;
  unsigned Byte5:8;
  unsigned Byte6:8;
  unsigned Byte7:8;
} txcmsgdata1;

// CanTxMessageBuffer
typedef union uCanTxMessageBuffer {
  struct {
    txcmsgsid CMSGSID;
    txcmsgeid CMSGEID;
    txcmsgdata0 CMSGDATA0;
    txcmsgdata1 CMSGDATA1;
  };
  int messageWord[4];
} CanTxMessageBuffer;

// Function definitions
int32_t CAN_send_message(uint16_t id, uint8_t dlc, uint8_t* data);

#endif /* FSAE_CAN_32_H */