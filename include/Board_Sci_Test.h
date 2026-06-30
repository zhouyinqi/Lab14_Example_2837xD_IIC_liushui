#ifndef BOARD_SCI_TEST_H
#define BOARD_SCI_TEST_H

#include "Board_Test.h"

#define BOARD_SCI_LOOPBACK_SCIA_PASS       0x0001UL
#define BOARD_SCI_LOOPBACK_SCIB_PASS       0x0002UL
#define BOARD_SCI_LOOPBACK_EXPECTED_MASK   0x0003UL
#define BOARD_SCI_LOOPBACK_SCIA_PATTERN    0x005AU
#define BOARD_SCI_LOOPBACK_SCIB_PATTERN    0x00A5U

#define BOARD_SCI_RS485_EXTERNAL_CONFIGURED     0x0001U
#define BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW  0x0002U
#define BOARD_SCI_RS485_EXTERNAL_RX_READY       0x0004U
#define BOARD_SCI_RS485_EXTERNAL_RX_MATCH       0x0008U
#define BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH 0x0010U
#define BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN     0x0020U
#define BOARD_SCI_RS485_EXTERNAL_TX_DONE        0x0040U
#define BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX     0x0080U

#define BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK \
    (BOARD_SCI_RS485_EXTERNAL_CONFIGURED |     \
     BOARD_SCI_RS485_EXTERNAL_RX_ENABLE_LOW |  \
     BOARD_SCI_RS485_EXTERNAL_RX_READY |       \
     BOARD_SCI_RS485_EXTERNAL_RX_MATCH |       \
     BOARD_SCI_RS485_EXTERNAL_TX_ENABLE_HIGH | \
     BOARD_SCI_RS485_EXTERNAL_TX_WRITTEN |     \
     BOARD_SCI_RS485_EXTERNAL_TX_DONE |        \
     BOARD_SCI_RS485_EXTERNAL_BACK_TO_RX)

#define BOARD_SCI_RS485_EXTERNAL_DIAGNOSTIC_MASK \
    BOARD_SCI_RS485_EXTERNAL_REQUIRED_MASK

#define BOARD_SCI_RS485_EXTERNAL_REQUEST   0x00A5U
#define BOARD_SCI_RS485_EXTERNAL_RESPONSE  0x005AU

#define BOARD_SCI_DETAIL_TX_READY          0x0001U
#define BOARD_SCI_DETAIL_TX_WRITTEN        0x0002U
#define BOARD_SCI_DETAIL_RX_READY          0x0004U
#define BOARD_SCI_DETAIL_RX_MATCH          0x0008U
#define BOARD_SCI_DETAIL_RX_ERROR          0x1000U
#define BOARD_SCI_DETAIL_TX_TIMEOUT        0x2000U
#define BOARD_SCI_DETAIL_RX_TIMEOUT        0x4000U


typedef struct
{
    volatile BoardTest_U16 sciaRx;
    volatile BoardTest_U16 scibRx;
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 sciaDetail;
    volatile BoardTest_U16 scibDetail;
} BoardSci_LoopbackSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rxValue;
    volatile BoardTest_U16 txValue;
    volatile BoardTest_U16 detail;
    volatile BoardTest_U16 directionLevel;
} BoardSci_Rs485ExternalSnapshot;

BoardTest_Result BoardSci_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                  BoardTest_U16 sciaRx,
                                                  BoardTest_U16 scibRx,
                                                  BoardTest_Record *record);

BoardTest_Result BoardSci_EvaluateRs485ExternalStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 rxValue,
    BoardTest_U16 txValue,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
extern volatile BoardSci_LoopbackSnapshot gBoardSciLoopbackSnapshot;
extern volatile BoardSci_Rs485ExternalSnapshot
    gBoardSciRs485ExternalSnapshot;

BoardTest_Result BoardSci_RunLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSci_RunRs485ExternalTest(BoardTest_Record *record);
void BoardSci_AbortRs485ExternalTest(void);
#endif

#endif
