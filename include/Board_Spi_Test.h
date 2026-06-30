#ifndef BOARD_SPI_TEST_H
#define BOARD_SPI_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_SPI_LOOPBACK_TX_READY       0x0001U
#define BOARD_SPI_LOOPBACK_TX_WRITTEN     0x0002U
#define BOARD_SPI_LOOPBACK_RX_READY       0x0004U
#define BOARD_SPI_LOOPBACK_RX_MATCH       0x0008U
#define BOARD_SPI_LOOPBACK_NO_OVERRUN     0x0010U

#define BOARD_SPI_LOOPBACK_REQUIRED_MASK \
    (BOARD_SPI_LOOPBACK_TX_READY |       \
     BOARD_SPI_LOOPBACK_TX_WRITTEN |     \
     BOARD_SPI_LOOPBACK_RX_READY |       \
     BOARD_SPI_LOOPBACK_RX_MATCH |       \
     BOARD_SPI_LOOPBACK_NO_OVERRUN)

#define BOARD_SPI_LOOPBACK_DIAGNOSTIC_MASK BOARD_SPI_LOOPBACK_REQUIRED_MASK

#define BOARD_SPI_LOOPBACK_SPIA_PATTERN   0xA55AU
#define BOARD_SPI_LOOPBACK_SPIB_PATTERN   0x5AA5U
#define BOARD_SPI_LOOPBACK_SPIC_PATTERN   0x3CC3U

typedef struct
{
    volatile BoardTest_U16 spiaStatusMask;
    volatile BoardTest_U16 spibStatusMask;
    volatile BoardTest_U16 spicStatusMask;
    volatile BoardTest_U16 spiaRx;
    volatile BoardTest_U16 spibRx;
    volatile BoardTest_U16 spicRx;
} BoardSpi_LoopbackSnapshot;

extern volatile BoardSpi_LoopbackSnapshot gBoardSpiLoopbackSnapshot;

BoardTest_Result BoardSpi_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_U16 rxValue,
                                                 BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardSpi_RunSpiaLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpibLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpicLoopbackTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
