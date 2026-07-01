#ifndef BOARD_CAN_TEST_H
#define BOARD_CAN_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_CAN_LOOPBACK_TX_OK       0x0001U
#define BOARD_CAN_LOOPBACK_RX_OK       0x0002U
#define BOARD_CAN_LOOPBACK_NEW_DATA    0x0004U
#define BOARD_CAN_LOOPBACK_DATA_MATCH  0x0008U
#define BOARD_CAN_LOOPBACK_NO_ERROR    0x0010U

#define BOARD_CAN_LOOPBACK_REQUIRED_MASK \
    (BOARD_CAN_LOOPBACK_TX_OK |          \
     BOARD_CAN_LOOPBACK_NEW_DATA |       \
     BOARD_CAN_LOOPBACK_DATA_MATCH)

#define BOARD_CAN_LOOPBACK_DIAGNOSTIC_MASK \
    (BOARD_CAN_LOOPBACK_TX_OK |             \
     BOARD_CAN_LOOPBACK_RX_OK |             \
     BOARD_CAN_LOOPBACK_NEW_DATA |          \
     BOARD_CAN_LOOPBACK_DATA_MATCH |     \
     BOARD_CAN_LOOPBACK_NO_ERROR)

#define BOARD_CAN_LOOPBACK_EXPECTED_MASK BOARD_CAN_LOOPBACK_REQUIRED_MASK

#define BOARD_CAN_LOOPBACK_TX_PATTERN  0x5AA55A5AUL
#define BOARD_CAN_LOOPBACK_MSG_ID      0x155UL

#define BOARD_CAN_EXTERNAL_CONFIGURED       0x0001U
#define BOARD_CAN_EXTERNAL_RX_OBJECT_READY  0x0002U
#define BOARD_CAN_EXTERNAL_RX_FRAME_READY   0x0004U
#define BOARD_CAN_EXTERNAL_RX_PATTERN_MATCH 0x0008U
#define BOARD_CAN_EXTERNAL_TX_WRITTEN       0x0010U
#define BOARD_CAN_EXTERNAL_TX_OK            0x0020U
#define BOARD_CAN_EXTERNAL_NO_ERROR         0x0040U

#define BOARD_CAN_EXTERNAL_REQUIRED_MASK \
    (BOARD_CAN_EXTERNAL_CONFIGURED |       \
     BOARD_CAN_EXTERNAL_RX_OBJECT_READY |  \
     BOARD_CAN_EXTERNAL_RX_FRAME_READY |   \
     BOARD_CAN_EXTERNAL_RX_PATTERN_MATCH | \
     BOARD_CAN_EXTERNAL_TX_WRITTEN |       \
     BOARD_CAN_EXTERNAL_TX_OK |            \
     BOARD_CAN_EXTERNAL_NO_ERROR)

#define BOARD_CAN_EXTERNAL_DIAGNOSTIC_MASK BOARD_CAN_EXTERNAL_REQUIRED_MASK
#define BOARD_CAN_EXTERNAL_EXPECTED_MASK BOARD_CAN_EXTERNAL_REQUIRED_MASK

#define BOARD_CAN_EXTERNAL_REQUEST_ID      0x321UL
#define BOARD_CAN_EXTERNAL_RESPONSE_ID     0x001UL
#define BOARD_CAN_EXTERNAL_REQUEST_PATTERN 0x5AA55AA5UL
#define BOARD_CAN_EXTERNAL_DATA_BYTES      4U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 errorStatus;
    volatile BoardTest_U16 txLow;
    volatile BoardTest_U16 txHigh;
    volatile BoardTest_U16 rxLow;
    volatile BoardTest_U16 rxHigh;
    volatile BoardTest_U16 txRequestLow;
    volatile BoardTest_U16 newDataLow;
} BoardCan_LoopbackSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 errorStatus;
    volatile BoardTest_U16 rxMsgId;
    volatile BoardTest_U16 rxLength;
    volatile BoardTest_U16 rxLow;
    volatile BoardTest_U16 rxHigh;
    volatile BoardTest_U16 txMsgId;
    volatile BoardTest_U16 txLow;
    volatile BoardTest_U16 txHigh;
    volatile BoardTest_U16 txRequestLow;
    volatile BoardTest_U16 newDataLow;
} BoardCan_ExternalSnapshot;

extern volatile BoardCan_LoopbackSnapshot gBoardCanLoopbackSnapshot;
extern volatile BoardCan_ExternalSnapshot gBoardCanExternalSnapshot;

BoardTest_Result BoardCan_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U32 txData,
                                                 BoardTest_U32 rxData,
                                                 BoardTest_U16 errorStatus,
                                                 BoardTest_Record *record);

BoardTest_Result BoardCan_EvaluateExternalStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U32 rxMsgId,
                                                 BoardTest_U16 rxLength,
                                                 BoardTest_U32 rxData,
                                                 BoardTest_U32 txMsgId,
                                                 BoardTest_U32 txData,
                                                 BoardTest_U16 errorStatus,
                                                 BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardCan_RunLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardCan_RunExternalTest(BoardTest_Record *record);
void BoardCan_AbortExternalTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
