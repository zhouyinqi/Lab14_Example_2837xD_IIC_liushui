#ifndef BOARD_HDO_TEST_H
#define BOARD_HDO_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * HDO1/HDO2 are candidate BIT3/BIT4 assignments following the three
 * board-validated EDO bits in the FPGA DO data and enable registers.
 */
#define BOARD_HDO_FPGA_DO_DATA_ADDR        0x00002000UL
#define BOARD_HDO_FPGA_DO_ENABLE_ADDR      0x00002001UL

#define BOARD_HDO_CHANNEL_1                0x0001U
#define BOARD_HDO_CHANNEL_2                0x0002U
#define BOARD_HDO_ALLOWED_CHANNEL_MASK     \
    (BOARD_HDO_CHANNEL_1 | BOARD_HDO_CHANNEL_2)
#define BOARD_HDO_DEFAULT_CHANNEL_MASK     BOARD_HDO_ALLOWED_CHANNEL_MASK

#define BOARD_HDO_FPGA_OUTPUT_1_BIT        0x0008U
#define BOARD_HDO_FPGA_OUTPUT_2_BIT        0x0010U
#define BOARD_HDO_FPGA_OUTPUT_MASK         \
    (BOARD_HDO_FPGA_OUTPUT_1_BIT | BOARD_HDO_FPGA_OUTPUT_2_BIT)

#define BOARD_HDO_TEST_ARM_KEY             0xD014U
#define BOARD_HDO_PHASE_HOLD_US            2000000UL

#define BOARD_HDO_STATUS_CONFIGURED        0x0001U
#define BOARD_HDO_STATUS_MAGIC_VALID       0x0002U
#define BOARD_HDO_STATUS_CHANNEL_1_TOGGLED 0x0004U
#define BOARD_HDO_STATUS_CHANNEL_2_TOGGLED 0x0008U
#define BOARD_HDO_STATUS_OUTPUTS_DISABLED  0x0010U

#define BOARD_HDO_FAIL_NONE                0x0000U
#define BOARD_HDO_FAIL_NOT_ARMED           0x0001U
#define BOARD_HDO_FAIL_INVALID_MASK        0x0002U
#define BOARD_HDO_FAIL_CONFIG              0x0003U
#define BOARD_HDO_FAIL_MAGIC               0x0004U
#define BOARD_HDO_FAIL_OUTPUT_ACTIVE       0x0005U

#define BOARD_HDO_PHASE_IDLE               0U
#define BOARD_HDO_PHASE_DATA_SET           1U
#define BOARD_HDO_PHASE_DATA_CLEAR         2U
#define BOARD_HDO_PHASE_SAFE               3U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 requestedChannelMask;
    volatile BoardTest_U16 initialDataValue;
    volatile BoardTest_U16 initialEnableValue;
    volatile BoardTest_U16 finalDataValue;
    volatile BoardTest_U16 finalEnableValue;
    volatile BoardTest_U16 activeOutputBit;
    volatile BoardTest_U16 activeDataValue;
    volatile BoardTest_U16 phase;
} BoardHdo_FpgaSnapshot;

extern volatile BoardHdo_FpgaSnapshot gBoardHdoFpgaSnapshot;

BoardTest_Result BoardHdo_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 channelMask,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardHdo_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 armKey,
    BoardTest_U16 channelMask);
void BoardHdo_ForceFpgaSafe(void);
#endif

#ifdef __cplusplus
}
#endif

#endif
