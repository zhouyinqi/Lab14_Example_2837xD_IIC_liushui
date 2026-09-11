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

#define BOARD_DI_DSP_DI1_BIT              0x0001U
#define BOARD_DI_DSP_DI2_BIT              0x0002U
#define BOARD_DI_DSP_DI3_BIT              0x0004U
#define BOARD_DI_DSP_INPUT_MASK           0x0007U
#define BOARD_DI_DSP_DEFAULT_CHANNEL_MASK BOARD_DI_DSP_INPUT_MASK

#define BOARD_DI_DRIVER_FAULT_1U_BIT       0x0001U
#define BOARD_DI_DRIVER_FAULT_1D_BIT       0x0002U
#define BOARD_DI_DRIVER_FAULT_2U_BIT       0x0004U
#define BOARD_DI_DRIVER_FAULT_2D_BIT       0x0008U
#define BOARD_DI_DRIVER_FAULT_INPUT_MASK   0x000FU
#define BOARD_DI_DRIVER_FAULT_GROUP_1_MASK 0x0003U
#define BOARD_DI_DRIVER_FAULT_GROUP_2_MASK 0x000CU
#define BOARD_DI_DRIVER_FAULT_DEFAULT_CHANNEL_MASK \
    BOARD_DI_DRIVER_FAULT_INPUT_MASK

#define BOARD_DI_PROTECTION_OC1_BIT       0x0001U
#define BOARD_DI_PROTECTION_OC2_BIT       0x0002U
#define BOARD_DI_PROTECTION_UOV1_BIT      0x0004U
#define BOARD_DI_PROTECTION_UOV2_BIT      0x0008U
#define BOARD_DI_PROTECTION_FAULT_INPUT_MASK 0x000FU
#define BOARD_DI_PROTECTION_FAULT_DEFAULT_CHANNEL_MASK \
    BOARD_DI_PROTECTION_FAULT_INPUT_MASK

#define BOARD_DI_SAMPLE_INTERVAL_US       1000UL
#define BOARD_DI_SAMPLE_LIMIT             30000U

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
#define BOARD_DI_DSP_REQUIRED_STATUS_MASK \
    (BOARD_DI_STATUS_CONFIGURED |         \
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
} BoardDi_DspSnapshot;

extern volatile BoardDi_FpgaSnapshot gBoardDiFpgaSnapshot;
extern volatile BoardDi_DspSnapshot gBoardDiDspSnapshot;
extern volatile BoardDi_DspSnapshot gBoardDiDriverFaultSnapshot;
extern volatile BoardDi_DspSnapshot gBoardDiProtectionFaultSnapshot;

#define BOARD_DI_LV_TIMEOUT_MS 30000UL
typedef struct
{
    BoardTest_U16 expectedMask;
    BoardTest_U16 lowSeen;
    BoardTest_U16 highSeen;
    BoardTest_U16 transitions;
    BoardTest_U16 previous;
    BoardTest_U16 sampled;
} BoardDi_LowVoltageCapture;
extern volatile BoardDi_DspSnapshot gBoardDiLowVoltageSnapshot;
BoardTest_U16 BoardDi_LowVoltageSelectionMask(BoardTest_U16 testId,
                                             BoardTest_U16 selection);
void BoardDi_InitLowVoltageCapture(BoardDi_LowVoltageCapture *capture,
                                  BoardTest_U16 mask);
BoardTest_Result BoardDi_UpdateLowVoltageCapture(BoardDi_LowVoltageCapture *capture,
    BoardTest_U16 currentMask, BoardTest_U32 elapsedMs, BoardTest_Record *record);

BoardTest_Result BoardDi_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record);
BoardTest_Result BoardDi_EvaluateDspStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record);
BoardTest_Result BoardDi_EvaluateDriverFaultStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record);
BoardTest_Result BoardDi_EvaluateProtectionFaultStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardDi_RunLowVoltageInputTest(BoardTest_U16 testId,
    BoardTest_U16 selection, BoardTest_Record *record);
void BoardDi_AbortLowVoltageInputTest(void);
BoardTest_Result BoardDi_RunLowVoltageOutputTest(BoardTest_U16 testId,
    BoardTest_U16 selection, BoardTest_U16 armKey, BoardTest_Record *record);
BoardTest_Result BoardDi_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask);
BoardTest_Result BoardDi_RunDspExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask);
BoardTest_Result BoardDi_RunDriverFaultExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask);
BoardTest_Result BoardDi_RunProtectionFaultExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask);
#endif

#ifdef __cplusplus
}
#endif

#endif
