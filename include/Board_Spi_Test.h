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

#define BOARD_SPI_EXTERNAL_CONFIGURED       0x0001U
#define BOARD_SPI_EXTERNAL_SD_RESERVED      0x0002U
#define BOARD_SPI_EXTERNAL_DSP_FLASH_ID     0x0004U
#define BOARD_SPI_EXTERNAL_FRAM_STATUS      0x0008U
#define BOARD_SPI_EXTERNAL_FRAM_ID          0x0010U
#define BOARD_SPI_EXTERNAL_FRAM_RW          0x0020U
#define BOARD_SPI_EXTERNAL_FPGA_FLASH_ID    0x0040U
#define BOARD_SPI_EXTERNAL_NO_OVERRUN       0x0080U

#define BOARD_SPI_EXTERNAL_REQUIRED_MASK \
    (BOARD_SPI_EXTERNAL_CONFIGURED |     \
     BOARD_SPI_EXTERNAL_SD_RESERVED |    \
     BOARD_SPI_EXTERNAL_DSP_FLASH_ID |   \
     BOARD_SPI_EXTERNAL_FRAM_STATUS |    \
     BOARD_SPI_EXTERNAL_FRAM_ID |        \
     BOARD_SPI_EXTERNAL_FRAM_RW |        \
     BOARD_SPI_EXTERNAL_FPGA_FLASH_ID |  \
     BOARD_SPI_EXTERNAL_NO_OVERRUN)

#define BOARD_SPI_EXTERNAL_DIAGNOSTIC_MASK BOARD_SPI_EXTERNAL_REQUIRED_MASK

#define BOARD_SPI_EXTERNAL_FAIL_NONE       0x0000U
#define BOARD_SPI_EXTERNAL_FAIL_CONFIG     0x0001U
#define BOARD_SPI_EXTERNAL_FAIL_DSP_FLASH  0x0002U
#define BOARD_SPI_EXTERNAL_FAIL_FRAM_STATUS 0x0003U
#define BOARD_SPI_EXTERNAL_FAIL_FRAM_ID    0x0004U
#define BOARD_SPI_EXTERNAL_FAIL_FRAM_RW    0x0005U
#define BOARD_SPI_EXTERNAL_FAIL_FPGA_FLASH 0x0006U

typedef struct
{
    volatile BoardTest_U16 spiaStatusMask;
    volatile BoardTest_U16 spibStatusMask;
    volatile BoardTest_U16 spicStatusMask;
    volatile BoardTest_U16 spiaRx;
    volatile BoardTest_U16 spibRx;
    volatile BoardTest_U16 spicRx;
} BoardSpi_LoopbackSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 dspFlashId0;
    volatile BoardTest_U16 dspFlashId1;
    volatile BoardTest_U16 dspFlashId2;
    volatile BoardTest_U16 framStatus;
    volatile BoardTest_U16 framId6;
    volatile BoardTest_U16 framId7;
    volatile BoardTest_U16 framId8;
    volatile BoardTest_U16 framOriginal;
    volatile BoardTest_U16 framReadback;
    volatile BoardTest_U16 framRestored;
    volatile BoardTest_U16 fpgaFlashId0;
    volatile BoardTest_U16 fpgaFlashId1;
    volatile BoardTest_U16 fpgaFlashId2;
    volatile BoardTest_U16 sdReserved;
} BoardSpi_ExternalSnapshot;

extern volatile BoardSpi_LoopbackSnapshot gBoardSpiLoopbackSnapshot;
extern volatile BoardSpi_ExternalSnapshot gBoardSpiExternalSnapshot;

BoardTest_Result BoardSpi_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_U16 rxValue,
                                                 BoardTest_Record *record);

BoardTest_Result BoardSpi_EvaluateExternalStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 failCode,
                                                 BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardSpi_RunSpiaLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpibLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpicLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpicExternalTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
