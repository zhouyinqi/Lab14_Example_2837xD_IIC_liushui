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

#define BOARD_SPIB_FLASH_CONFIGURED         0x0001U
#define BOARD_SPIB_FLASH_ID_READ            0x0002U
#define BOARD_SPIB_FLASH_ID_VALID           0x0004U
#define BOARD_SPIB_FLASH_NO_OVERRUN         0x0008U

#define BOARD_SPIB_FLASH_REQUIRED_MASK \
    (BOARD_SPIB_FLASH_CONFIGURED |     \
     BOARD_SPIB_FLASH_ID_READ |        \
     BOARD_SPIB_FLASH_ID_VALID |       \
     BOARD_SPIB_FLASH_NO_OVERRUN)

#define BOARD_SPIB_FLASH_FAIL_NONE          0x0000U
#define BOARD_SPIB_FLASH_FAIL_PROFILE       0x0001U
#define BOARD_SPIB_FLASH_FAIL_TRANSFER      0x0002U
#define BOARD_SPIB_FLASH_FAIL_ID            0x0003U
#define BOARD_SPIB_FLASH_MANUFACTURER_ID    0x00EFU
#define BOARD_SPIB_FLASH_MEMORY_TYPE        0x0040U
#define BOARD_SPIB_FLASH_CAPACITY_64_MBIT   0x0017U

#define BOARD_SPIB_FRAM_CONFIGURED          0x0001U
#define BOARD_SPIB_FRAM_STATUS_READ         0x0002U
#define BOARD_SPIB_FRAM_ID_READ             0x0004U
#define BOARD_SPIB_FRAM_ID_VALID            0x0008U
#define BOARD_SPIB_FRAM_ADDRESS_WRITABLE    0x0010U
#define BOARD_SPIB_FRAM_ORIGINAL_READ       0x0020U
#define BOARD_SPIB_FRAM_WRITE_ENABLED       0x0040U
#define BOARD_SPIB_FRAM_TEST_WRITTEN        0x0080U
#define BOARD_SPIB_FRAM_TEST_READBACK       0x0100U
#define BOARD_SPIB_FRAM_RESTORE_WRITTEN     0x0200U
#define BOARD_SPIB_FRAM_RESTORE_VERIFIED    0x0400U
#define BOARD_SPIB_FRAM_WRITE_DISABLED      0x0800U
#define BOARD_SPIB_FRAM_NO_OVERRUN          0x1000U

#define BOARD_SPIB_FRAM_REQUIRED_MASK \
    (BOARD_SPIB_FRAM_CONFIGURED |       \
     BOARD_SPIB_FRAM_STATUS_READ |      \
     BOARD_SPIB_FRAM_ID_READ |          \
     BOARD_SPIB_FRAM_ID_VALID |         \
     BOARD_SPIB_FRAM_ADDRESS_WRITABLE | \
     BOARD_SPIB_FRAM_ORIGINAL_READ |    \
     BOARD_SPIB_FRAM_WRITE_ENABLED |    \
     BOARD_SPIB_FRAM_TEST_WRITTEN |     \
     BOARD_SPIB_FRAM_TEST_READBACK |    \
     BOARD_SPIB_FRAM_RESTORE_WRITTEN |  \
     BOARD_SPIB_FRAM_RESTORE_VERIFIED | \
     BOARD_SPIB_FRAM_WRITE_DISABLED |   \
     BOARD_SPIB_FRAM_NO_OVERRUN)

#define BOARD_SPIB_FRAM_FAIL_NONE           0x0000U
#define BOARD_SPIB_FRAM_FAIL_PROFILE        0x0001U
#define BOARD_SPIB_FRAM_FAIL_STATUS         0x0002U
#define BOARD_SPIB_FRAM_FAIL_TRANSFER       0x0003U
#define BOARD_SPIB_FRAM_FAIL_ID             0x0004U
#define BOARD_SPIB_FRAM_FAIL_PROTECTED      0x0005U
#define BOARD_SPIB_FRAM_FAIL_ORIGINAL_READ  0x0006U
#define BOARD_SPIB_FRAM_FAIL_WRITE_ENABLE   0x0007U
#define BOARD_SPIB_FRAM_FAIL_WRITE          0x0008U
#define BOARD_SPIB_FRAM_FAIL_TEST_READBACK  0x0009U
#define BOARD_SPIB_FRAM_FAIL_RESTORE_WRITE  0x000AU
#define BOARD_SPIB_FRAM_FAIL_RESTORE_VERIFY 0x000BU
#define BOARD_SPIB_FRAM_FAIL_WRITE_DISABLE  0x000CU
#define BOARD_SPIB_FRAM_MANUFACTURER_ID     0x00C2U
#define BOARD_SPIB_FRAM_PRODUCT_ID_HIGH     0x0025U
#define BOARD_SPIB_FRAM_PRODUCT_ID_LOW      0x0008U
#define BOARD_SPIB_FRAM_PRODUCT_CODE        0x2508U
#define BOARD_SPIB_FRAM_STATUS_WEL           0x0002U
#define BOARD_SPIB_FRAM_STATUS_BP_MASK       0x000CU
#define BOARD_SPIB_FRAM_MAX_ADDRESS          0x0003FFFFUL
/* This byte is reserved for the non-destructive board test. */
#define BOARD_SPIB_FRAM_TEST_ADDRESS         0x0003FFF0UL
#define BOARD_SPIB_FRAM_TEST_XOR_PATTERN     0x00A5U

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

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 manufacturerId;
    volatile BoardTest_U16 memoryType;
    volatile BoardTest_U16 capacityCode;
    volatile BoardTest_U16 simoPin;
    volatile BoardTest_U16 somiPin;
    volatile BoardTest_U16 clockPin;
    volatile BoardTest_U16 chipSelectPin;
} BoardSpi_FlashSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 statusRegister;
    volatile BoardTest_U16 manufacturerId;
    volatile BoardTest_U16 productIdHigh;
    volatile BoardTest_U16 productIdLow;
    volatile BoardTest_U16 continuationCount;
    volatile BoardTest_U16 chipSelectPin;
    volatile BoardTest_U32 testAddress;
    volatile BoardTest_U16 originalValue;
    volatile BoardTest_U16 testValue;
    volatile BoardTest_U16 testReadback;
    volatile BoardTest_U16 restoreReadback;
    volatile BoardTest_U16 statusAfterWriteEnable;
    volatile BoardTest_U16 statusAfterRestoreEnable;
    volatile BoardTest_U16 finalStatusRegister;
} BoardSpi_FramSnapshot;

extern volatile BoardSpi_LoopbackSnapshot gBoardSpiLoopbackSnapshot;
extern volatile BoardSpi_ExternalSnapshot gBoardSpiExternalSnapshot;
extern volatile BoardSpi_FlashSnapshot gBoardSpiFlashSnapshot;
extern volatile BoardSpi_FramSnapshot gBoardSpiFramSnapshot;

BoardTest_Result BoardSpi_EvaluateLoopbackStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 txValue,
                                                 BoardTest_U16 rxValue,
                                                 BoardTest_Record *record);

BoardTest_Result BoardSpi_EvaluateExternalStatus(BoardTest_U16 statusMask,
                                                 BoardTest_U16 failCode,
                                                 BoardTest_Record *record);

BoardTest_Result BoardSpi_EvaluateSpibFlashStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 manufacturerId,
    BoardTest_U16 memoryType,
    BoardTest_U16 capacityCode,
    BoardTest_Record *record);

BoardTest_Result BoardSpi_EvaluateSpibFramStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 statusRegister,
    BoardTest_U16 manufacturerId,
    BoardTest_U16 productIdHigh,
    BoardTest_U16 productIdLow,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardSpi_RunSpiaLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpibLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpicLoopbackTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpicExternalTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpibFlashExternalTest(BoardTest_Record *record);
BoardTest_Result BoardSpi_RunSpibFramExternalTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
