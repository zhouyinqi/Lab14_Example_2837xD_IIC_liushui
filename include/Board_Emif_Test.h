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

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 emif1AsyncMask;
    volatile BoardTest_U16 emif2AsyncMask;
    volatile BoardTest_U16 emif1MasterSelect;
    volatile BoardTest_U16 deviceCapability;
} BoardEmif_BasicSnapshot;

extern volatile BoardEmif_BasicSnapshot gBoardEmifBasicSnapshot;

BoardTest_Result BoardEmif_EvaluateBasicStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 emif1AsyncMask,
                                               BoardTest_U16 emif2AsyncMask,
                                               BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
void BoardEmif_ConfigureExternalAsync(void);
BoardTest_Result BoardEmif_RunBasicConfigTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
