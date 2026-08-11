#ifndef BOARD_DI_TEST_H
#define BOARD_DI_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_DI_FPGA_INPUT_ADDR          0x00002000UL

#define BOARD_DI_EDI1_BIT                 0x0001U
#define BOARD_DI_EDI2_BIT                 0x0002U
#define BOARD_DI_EDI3_BIT                 0x0004U
#define BOARD_DI_EDI4_BIT                 0x0008U
#define BOARD_DI_EDI5_BIT                 0x0010U
#define BOARD_DI_EDI6_BIT                 0x0020U
#define BOARD_DI_HDI1_BIT                 0x0040U
#define BOARD_DI_HDI2_BIT                 0x0080U
#define BOARD_DI_INPUT_MASK               0x00FFU
#define BOARD_DI_DEFAULT_CHANNEL_MASK     BOARD_DI_INPUT_MASK

#define BOARD_DI_SAMPLE_INTERVAL_US       1000UL
#define BOARD_DI_SAMPLE_LIMIT             5000U

#define BOARD_DI_STATUS_CONFIGURED        0x0001U
#define BOARD_DI_STATUS_MAGIC_VALID       0x0002U
#define BOARD_DI_STATUS_INACTIVE_SEEN     0x0004U
#define BOARD_DI_STATUS_ACTIVE_SEEN       0x0008U
#define BOARD_DI_STATUS_TRANSITION_SEEN   0x0010U
#define BOARD_DI_REQUIRED_STATUS_MASK     \
    (BOARD_DI_STATUS_CONFIGURED |         \
     BOARD_DI_STATUS_MAGIC_VALID |        \
     BOARD_DI_STATUS_INACTIVE_SEEN |      \
     BOARD_DI_STATUS_ACTIVE_SEEN |        \
     BOARD_DI_STATUS_TRANSITION_SEEN)

#define BOARD_DI_FAIL_NONE                0x0000U
#define BOARD_DI_FAIL_CONFIG              0x0001U
#define BOARD_DI_FAIL_MAGIC               0x0002U
#define BOARD_DI_FAIL_TIMEOUT             0x0003U
#define BOARD_DI_FAIL_CHANNEL_MASK        0x0004U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 requestedChannelMask;
    volatile BoardTest_U16 firstRawValue;
    volatile BoardTest_U16 lastRawValue;
    volatile BoardTest_U16 observedInactiveMask;
    volatile BoardTest_U16 observedActiveMask;
    volatile BoardTest_U16 transitionMask;
    volatile BoardTest_U16 sampleCount;
} BoardDi_FpgaSnapshot;

extern volatile BoardDi_FpgaSnapshot gBoardDiFpgaSnapshot;

BoardTest_Result BoardDi_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardDi_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask);
#endif

#ifdef __cplusplus
}
#endif

#endif
