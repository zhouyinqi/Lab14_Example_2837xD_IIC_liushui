#ifndef BOARD_DIDO_TEST_H
#define BOARD_DIDO_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * These two candidate offsets are isolated here so they can be replaced when
 * the FPGA engineer publishes the production EMIF2 register table. They are
 * inferred from the proven EMIF1 write map and the validated magic probes.
 */
#define BOARD_DIDO_FPGA_DO_DATA_ADDR       0x00002000UL
#define BOARD_DIDO_FPGA_DO_ENABLE_ADDR     0x00002001UL

#define BOARD_DIDO_CHANNEL_1               0x0001U
#define BOARD_DIDO_CHANNEL_2               0x0002U
#define BOARD_DIDO_CHANNEL_3               0x0004U
#define BOARD_DIDO_ALLOWED_CHANNEL_MASK    \
    (BOARD_DIDO_CHANNEL_1 | BOARD_DIDO_CHANNEL_2 | BOARD_DIDO_CHANNEL_3)
#define BOARD_DIDO_DEFAULT_CHANNEL_MASK    BOARD_DIDO_ALLOWED_CHANNEL_MASK

#define BOARD_DIDO_TEST_ARM_KEY            0xD012U
#define BOARD_DIDO_PHASE_HOLD_US            2000000UL
#define BOARD_DRIVER_RESET_HIGH_HOLD_US    10000000UL
#define BOARD_DRIVER_RESET_SERVICE_STEP_US     1000UL

#define BOARD_DIDO_STATUS_CONFIGURED       0x0001U
#define BOARD_DIDO_STATUS_MAGIC_VALID      0x0002U
#define BOARD_DIDO_STATUS_CHANNEL_1_TOGGLED 0x0004U
#define BOARD_DIDO_STATUS_CHANNEL_2_TOGGLED 0x0008U
#define BOARD_DIDO_STATUS_CHANNEL_3_TOGGLED 0x0010U
#define BOARD_DIDO_STATUS_OUTPUTS_DISABLED 0x0020U

#define BOARD_DIDO_FAIL_NONE               0x0000U
#define BOARD_DIDO_FAIL_NOT_ARMED          0x0001U
#define BOARD_DIDO_FAIL_INVALID_MASK       0x0002U
#define BOARD_DIDO_FAIL_CONFIG             0x0003U
#define BOARD_DIDO_FAIL_MAGIC              0x0004U

#define BOARD_DIDO_PHASE_IDLE              0U
#define BOARD_DIDO_PHASE_DATA_SET          1U
#define BOARD_DIDO_PHASE_DATA_CLEAR        2U
#define BOARD_DIDO_PHASE_SAFE              3U

#define BOARD_DRIVER_RESET_CHANNEL_URST1   0x0001U
#define BOARD_DRIVER_RESET_CHANNEL_DRST1   0x0002U
#define BOARD_DRIVER_RESET_CHANNEL_URST2   0x0004U
#define BOARD_DRIVER_RESET_CHANNEL_DRST2   0x0008U
#define BOARD_DRIVER_RESET_ALLOWED_MASK    0x000FU

#define BOARD_DRIVER_RESET_STATUS_CONFIGURED 0x0001U
#define BOARD_DRIVER_RESET_STATUS_ASSERTED   0x0002U
#define BOARD_DRIVER_RESET_STATUS_READBACK   0x0004U
#define BOARD_DRIVER_RESET_STATUS_SAFE_FINAL 0x0008U
#define BOARD_DRIVER_RESET_REQUIRED_STATUS   0x000FU

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 requestedChannelMask;
    volatile BoardTest_U16 initialDataValue;
    volatile BoardTest_U16 initialEnableValue;
    volatile BoardTest_U16 finalDataValue;
    volatile BoardTest_U16 finalEnableValue;
    volatile BoardTest_U16 activeChannelMask;
    volatile BoardTest_U16 activeDataValue;
    volatile BoardTest_U16 phase;
} BoardDido_FpgaDoSnapshot;

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 requestedChannelMask;
    volatile BoardTest_U16 initialOutputMask;
    volatile BoardTest_U16 toggledChannelMask;
    volatile BoardTest_U16 finalOutputMask;
    volatile BoardTest_U16 activeChannelMask;
    volatile BoardTest_U16 phase;
} BoardDido_DspDoSnapshot;

extern volatile BoardDido_FpgaDoSnapshot gBoardDidoFpgaDoSnapshot;
extern volatile BoardDido_DspDoSnapshot gBoardDidoDspDoSnapshot;
extern volatile BoardDido_DspDoSnapshot gBoardDidoDriverResetSnapshot;

BoardTest_Result BoardDido_EvaluateFpgaDoStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record);
BoardTest_Result BoardDido_EvaluateDspDoStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record);
BoardTest_Result BoardDido_EvaluateDriverResetStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardDido_RunFpgaDoExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask);
BoardTest_Result BoardDido_RunDspDoExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask);
BoardTest_Result BoardDido_RunDriverResetExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask);
void BoardDido_ForceFpgaDoSafe(void);
void BoardDido_ForceDspDoSafe(void);
void BoardDido_ForceDriverResetSafe(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
