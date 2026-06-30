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

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 checkedPairs;
    volatile BoardTest_U16 failedChannel;
    volatile BoardTest_U16 firstFailureMask;
    volatile BoardTest_U16 forceLowReadback;
} BoardPwm_SafetySnapshot;

extern volatile BoardPwm_SafetySnapshot gBoardPwmSafetySnapshot;

BoardTest_Result BoardPwm_EvaluateSafetyStatus(BoardTest_U16 statusMask,
                                               BoardTest_U16 checkedPairs,
                                               BoardTest_U16 failedChannel,
                                               BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardPwm_RunSafetyConfigTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
