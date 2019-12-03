/* --COPYRIGHT--,BSD
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef HCI_TL_H
#define HCI_TL_H

/* Standard Includes */
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

/* HCI Buffer size */
#define TX_BUFFER_SIZE  256
#define RX_BUFFER_SIZE  256

#define HCI_FRAME_SIZE          0x04

/* HCI Packet Types */
#define HCI_CMD_PACKET                 0x01
#define HCI_ACL_DATA_PACKET            0x02
#define HCI_SCO_DATA_PACKET            0x03
#define HCI_EVENT_PACKET               0x04

/* HCI Event Codes */
#define HCI_COMMANDCOMPLETEEVENT        0x0E
#define HCI_LE_EXTEVENT                 0xFF

/* HCI Vendor Specific API Opcodes */
#define HCI_EXT_RESETSYSTEMDONE         0x041D
#define HCI_EXT_RESETSYSTEMCMD          0xFC1D
#define HCI_COMMANDSTATUS               0x067F

/* HCI port types */
#define HCI_PORT_REMOTE_UART      0x01
#define HCI_PORT_REMOTE_SPI       0x02

/* Task configuration */
#define HCI_RX_TASK_PRIORITY        4
#define HCI_RX_TASK_STACK_SIZE      2048

/* HCI Status return types  */
typedef enum
{
    SUCCESS                       = 0x00,
    FAILURE                       = 0x01,
    InvalidParameter              = 0x02,
    InvalidTask                   = 0x03,
    MsgBufferNotAvailable         = 0x04,
    InvalidMsgPointer             = 0x05,
    InvalidEventId                = 0x06,
    InvalidInteruptId             = 0x07,
    NoTimerAvail                  = 0x08,
    NVItemUnInit                  = 0x09,
    NVOpFailed                    = 0x0A,
    InvalidMemSize                = 0x0B,
    ErrorCommandDisallowed        = 0x0C,

    bleNotReady                   = 0x10,   // Not ready to perform task
    bleAlreadyInRequestedMode     = 0x11,   // Already performing that task
    bleIncorrectMode              = 0x12,   // Not setup properly to perform that task
    bleMemAllocError              = 0x13,   // Memory allocation error occurred
    bleNotConnected               = 0x14,   // Can't perform function when not in a connection
    bleNoResources                = 0x15,   // There are no resource available
    blePending                    = 0x16,   // Waiting
    bleTimeout                    = 0x17,   // Timed out performing function
    bleInvalidRange               = 0x18,   // A parameter is out of range
    bleLinkEncrypted              = 0x19,   // The link is already encrypted
    bleProcedureComplete          = 0x1A,   // The Procedure is completed

    /* GAP Status Return Values */
    bleGAPUserCanceled            = 0x30,   // The user canceled the task
    bleGAPConnNotAcceptable       = 0x31,   // The connection was not accepted
    bleGAPBondRejected            = 0x32,   // The bound information was rejected.

    /* ATT Status Return Values */
    bleInvalidPDU                 = 0x40,   // The attribute PDU is invalid
    bleInsufficientAuthen         = 0x41,   // The attribute has insufficient authentication
    bleInsufficientEncrypt        = 0x42,   // The attribute has insufficient encryption
    bleInsufficientKeySize        = 0x43,   // The attribute has insufficient encryption key size

    /* L2CAP Status Return Values - returned as bStatus_t */
    INVALID_TASK_ID               = 0xFF    // Task ID isn't setup properly
} HCI_StatusCodes_t;

/* HCI Packet fields */
typedef struct hciPacket_t
{
    uint8_t packetType;
    uint8_t opcodeLO;
    uint8_t opcodeHI;
    uint8_t dataLength;
    uint8_t *pData;
} hciPacket_t;

//! @brief parameter structure for the HCI command. \n
typedef struct
{
  uint16_t  opcode; //!< HCI opcode to execute
                    //!
                    //! The list of available HCI opcode is listed here: @ref SNP_ALLOWED_HCI
  uint8_t   *pData; //!< parameters of the HCI opcode.
                    //!
                    //! Parameter depends of the HCI command being used. Those parameter are absolutely identical to the one define in TI HCI vendor guide.
} hciCmd_t;

/* HCI Event Header fields */
typedef struct
{
  uint8_t  type;
  uint8_t eventCode;
  uint8_t  dataLength;
  uint16_t eventOpCode;
  uint8_t status;
  uint8_t esg;
} eventHeader_t;

/* HCI Port structure fields */
typedef struct
{
  uint8_t              boardID;        //!< Board ID for physical port, i.e. Board_UART0
  uint32_t             bitRate;        //!< Baud/Bit Rate for physical port

} HCI_Port_t;

/* HCI Transport Parameters */
typedef struct
{
    uint8_t        portType;
    HCI_Port_t     remote;
} HCI_Params;

/* HCI RX Buffer */
extern uint8_t HCI_rxBuffer[RX_BUFFER_SIZE];

/* Prototypes for the APIs */
extern HCI_StatusCodes_t HCI_sendHCICommand(uint16_t opcode, uint8_t *pData, uint8_t dataLength);
extern HCI_StatusCodes_t HCI_ResetCmd(uint8_t type);
extern void HCI_open(HCI_Params *hciParams);
extern void HCI_close(void);
extern void HCI_init(HCI_Params *hciParams);
extern int HCI_waitForEvent(void);
extern eventHeader_t HCI_decodeEventHeader(uint8_t *packet);
#endif /* HCI_TL_H */
