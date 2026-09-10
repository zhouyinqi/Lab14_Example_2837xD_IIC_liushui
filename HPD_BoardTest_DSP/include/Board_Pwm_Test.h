#ifndef BOARD_PWM_TEST_H
#define BOARD_PWM_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_PWM_SAFE_CLOCKS_ENABLED      0x0001U
#define BOARD_PWM_SAFE_CPU1_SELECTED       0x0002U
#define BOARD_PWM_SAFE_TBCLK_STOPPED       0x0004U
#define BOARD_PWM_SAFE_COUNTERS_FROZEN     0x0008U
#define BOARD_PWM_SAFE_COMPARES_ZERO       0x0010U
#define BOARD_PWM_SAFE_FORCE_LOW           0x0020U
#define BOARD_PWM_SAFE_DEADBAND_DISABLED   0x0040U
#define BOARD_PWM_SAFE_TRIGGERS_DISABLED   0x0080U

#define BOARD_PWM_SAFE_REQUIRED_MASK \
    (BOARD_PWM_SAFE_CLOCKS_ENABLED |    \
     BOARD_PWM_SAFE_CPU1_SELECTED |     \
     BOARD_PWM_SAFE_TBCLK_STOPPED |     \
     BOARD_PWM_SAFE_COUNTERS_FROZEN |   \
     BOARD_PWM_SAFE_COMPARES_ZERO |     \
     BOARD_PWM_SAFE_FORCE_LOW |         \
     BOARD_PWM_SAFE_DEADBAND_DISABLED | \
     BOARD_PWM_SAFE_TRIGGERS_DISABLED)

#define BOARD_PWM_SAFE_DIAGNOSTIC_MASK BOARD_PWM_SAFE_REQUIRED_MASK

#define BOARD_PWM_DIDO_TEST_ARM_KEY       0xD01FU
#define BOARD_PWM_DIDO_OUTPUT_COUNT       12U
#define BOARD_PWM_DIDO_OUTPUTS_PER_WINDING 6U
#define BOARD_PWM_DIDO_OUTPUT_MASK        0x0FFFU
#define BOARD_PWM_DIDO_HIGH_HOLD_US       2000000UL
#define BOARD_PWM_DIDO_LOW_HOLD_US         100000UL
#define BOARD_PWM_DIDO_READBACK_SETTLE_US      10UL
#define BOARD_PWM_DIDO_TIMER_DIVIDER           199U

#define BOARD_PWM_DIDO_PHASE_IDLE               0U
#define BOARD_PWM_DIDO_PHASE_HIGH               1U
#define BOARD_PWM_DIDO_PHASE_LOW                2U

#define BOARD_PWM_DIDO_STATUS_CONFIGURED       0x0001U
#define BOARD_PWM_DIDO_STATUS_ENABLES_SAFE     0x0002U
#define BOARD_PWM_DIDO_STATUS_SEQUENCE_DONE    0x0004U
#define BOARD_PWM_DIDO_STATUS_OUTPUTS_SAFE     0x0008U
#define BOARD_PWM_DIDO_REQUIRED_MASK \
    (BOARD_PWM_DIDO_STATUS_CONFIGURED |    \
     BOARD_PWM_DIDO_STATUS_ENABLES_SAFE |  \
     BOARD_PWM_DIDO_STATUS_SEQUENCE_DONE | \
     BOARD_PWM_DIDO_STATUS_OUTPUTS_SAFE)

#define BOARD_PWM_DIDO_FAIL_NONE          0U
#define BOARD_PWM_DIDO_FAIL_NOT_ARMED     1U
#define BOARD_PWM_DIDO_FAIL_PROFILE       2U
#define BOARD_PWM_DIDO_FAIL_CONFIG        3U
#define BOARD_PWM_DIDO_FAIL_ENABLE_UNSAFE 4U
#define BOARD_PWM_DIDO_FAIL_OUTPUT_UNSAFE 5U
#define BOARD_PWM_DIDO_FAIL_OUTPUT_HIGH   6U
#define BOARD_PWM_DIDO_FAIL_SELECTION     7U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 checkedPairs;
    volatile BoardTest_U16 failedChannel;
    volatile BoardTest_U16 firstFailureMask;
    volatile BoardTest_U16 forceLowReadback;
} BoardPwm_SafetySnapshot;

extern volatile BoardPwm_SafetySnapshot gBoardPwmSafetySnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 requestedOutputMask;
    volatile BoardTest_U16 toggledOutputMask;
    volatile BoardTest_U16 activeOutputIndex;
    volatile BoardTest_U16 activeEnableReadbackMask;
    volatile BoardTest_U16 enableReadbackMask;
    volatile BoardTest_U16 powerEnableReadbackMask;
    volatile BoardTest_U16 finalOutputMask;
    volatile BoardTest_U16 phase;
    volatile BoardTest_U16 elapsedMilliseconds;
    volatile BoardTest_U16 liveOutputReadbackMask;
} BoardPwm_DidoSnapshot;

extern volatile BoardPwm_DidoSnapshot gBoardPwmDidoSnapshot;

BoardTest_Result BoardPwm_EvaluateSafetyStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 checkedPairs,
                                               BoardTest_U16 failedChannel,
                                               BoardTest_Record *record);
BoardTest_Result BoardPwm_EvaluateDidoStatus(BoardTest_U16 statusMask,
                                             BoardTest_U16 failCode,
                                             BoardTest_U16 toggledOutputMask,
                                             BoardTest_U16 expectedOutputMask,
                                             BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardPwm_RunSafetyConfigTest(BoardTest_Record *record);
BoardTest_Result BoardPwm_RunDidoExternalTest(BoardTest_Record *record,
                                              BoardTest_U16 armKey,
                                              BoardTest_U16 outputMask);
void BoardPwm_ForceDidoSafe(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
