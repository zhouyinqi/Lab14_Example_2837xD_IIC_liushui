#ifndef BOARD_EMIF_TEST_H
#define BOARD_EMIF_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_EMIF_BASIC_DEVICE_PRESENT   0x0001U
#define BOARD_EMIF_BASIC_CLOCKS_ENABLED   0x0002U
#define BOARD_EMIF_BASIC_RESETS_RELEASED  0x0004U
#define BOARD_EMIF_BASIC_EMIF1_CPU1       0x0008U
#define BOARD_EMIF_BASIC_EMIF1_ASYNC_CS2  0x0010U
#define BOARD_EMIF_BASIC_EMIF2_ASYNC_CS2  0x0020U
#define BOARD_EMIF_BASIC_CONFIG_UNLOCKED  0x0040U
#define BOARD_EMIF_BASIC_PINMAP_VALID     0x0080U

#define BOARD_EMIF_BASIC_REQUIRED_MASK \
    (BOARD_EMIF_BASIC_DEVICE_PRESENT |  \
     BOARD_EMIF_BASIC_CLOCKS_ENABLED |  \
     BOARD_EMIF_BASIC_RESETS_RELEASED | \
     BOARD_EMIF_BASIC_EMIF1_CPU1 |      \
     BOARD_EMIF_BASIC_EMIF1_ASYNC_CS2 | \
     BOARD_EMIF_BASIC_EMIF2_ASYNC_CS2 | \
     BOARD_EMIF_BASIC_CONFIG_UNLOCKED | \
     BOARD_EMIF_BASIC_PINMAP_VALID)

#define BOARD_EMIF_BASIC_DIAGNOSTIC_MASK BOARD_EMIF_BASIC_REQUIRED_MASK

#define BOARD_EMIF_ASYNC_WIDTH_16         0x0001U
#define BOARD_EMIF_ASYNC_TIMING_VALID     0x0002U
#define BOARD_EMIF_ASYNC_REQUIRED_MASK \
    (BOARD_EMIF_ASYNC_WIDTH_16 | BOARD_EMIF_ASYNC_TIMING_VALID)

#define BOARD_EMIF_SRAM_CONFIGURED      0x0001U
#define BOARD_EMIF_SRAM_CS3_TIMING      0x0002U
#define BOARD_EMIF_SRAM_PATTERN_5555    0x0004U
#define BOARD_EMIF_SRAM_PATTERN_AAAA    0x0008U
#define BOARD_EMIF_SRAM_ADDRESS_PATTERN 0x0010U
#define BOARD_EMIF_SRAM_LIVE_SAMPLE     0x0020U

#define BOARD_EMIF_SRAM_REQUIRED_MASK \
    (BOARD_EMIF_SRAM_CONFIGURED |      \
     BOARD_EMIF_SRAM_CS3_TIMING |      \
     BOARD_EMIF_SRAM_PATTERN_5555 |    \
     BOARD_EMIF_SRAM_PATTERN_AAAA |    \
     BOARD_EMIF_SRAM_ADDRESS_PATTERN | \
     BOARD_EMIF_SRAM_LIVE_SAMPLE)

#define BOARD_EMIF_SRAM_DIAGNOSTIC_MASK BOARD_EMIF_SRAM_REQUIRED_MASK

#define BOARD_EMIF_SRAM_FAIL_NONE       0x0000U
#define BOARD_EMIF_SRAM_FAIL_5555       0x0001U
#define BOARD_EMIF_SRAM_FAIL_AAAA       0x0002U
#define BOARD_EMIF_SRAM_FAIL_ADDRESS    0x0003U
#define BOARD_EMIF_SRAM_FAIL_LIVE       0x0004U
#define BOARD_EMIF_SRAM_FAIL_CONFIG     0x0005U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 emif1AsyncMask;
    volatile BoardTest_U16 emif2AsyncMask;
    volatile BoardTest_U16 emif1MasterSelect;
    volatile BoardTest_U16 deviceCapability;
} BoardEmif_BasicSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 cs3AsyncMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 expected;
    volatile BoardTest_U16 actual;
    volatile BoardTest_U16 failOffsetLow;
    volatile BoardTest_U16 failOffsetHigh;
    volatile BoardTest_U16 testWordsLow;
    volatile BoardTest_U16 testWordsHigh;
} BoardEmif_SramSnapshot;

extern volatile BoardEmif_BasicSnapshot gBoardEmifBasicSnapshot;
extern volatile BoardEmif_SramSnapshot gBoardEmifSramSnapshot;

BoardTest_Result BoardEmif_EvaluateBasicStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 emif1AsyncMask,
                                               BoardTest_U16 emif2AsyncMask,
                                               BoardTest_Record *record);

BoardTest_Result BoardEmif_EvaluateSramStatus(BoardTest_U16 statusMask,
                                              BoardTest_U16 failCode,
                                              BoardTest_U16 expected,
                                              BoardTest_U16 actual,
                                              BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
void BoardEmif_ConfigureExternalAsync(void);
BoardTest_Result BoardEmif_RunBasicConfigTest(BoardTest_Record *record);
BoardTest_Result BoardEmif_RunSramExternalTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
