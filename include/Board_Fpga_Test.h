#ifndef BOARD_FPGA_TEST_H
#define BOARD_FPGA_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * F28377D EMIF2-CS2 exposes a 4K-word CPU window at 0x2000-0x2FFF.
 * TEST2 and TEST3 retain the low address patterns used by the proven EMIF1
 * FPGA test. 0x2000 is also sampled for diagnostics, but it is the first
 * register in the EMIF2 window and is not assumed to contain the EMIF1 TEST1
 * magic value. Production FPGA registers need a separate EMIF2 offset table
 * before DI/DO or PWM access is enabled.
 */
#define BOARD_FPGA_EMIF2_TEST1_ADDR     0x00002000UL
#define BOARD_FPGA_EMIF2_TEST2_ADDR     0x00002555UL
#define BOARD_FPGA_EMIF2_TEST3_ADDR     0x00002AAAUL

#define BOARD_FPGA_TEST1_EXPECTED       0x5A3CU
#define BOARD_FPGA_TEST2_EXPECTED       0xAAAAU
#define BOARD_FPGA_TEST3_EXPECTED       0x5555U

#define BOARD_FPGA_EMIF2_CONFIGURED     0x0001U
#define BOARD_FPGA_EMIF2_PINMAP_VALID   0x0002U
#define BOARD_FPGA_EMIF2_TEST1_MATCH    0x0004U
#define BOARD_FPGA_EMIF2_TEST2_MATCH    0x0008U
#define BOARD_FPGA_EMIF2_TEST3_MATCH    0x0010U

#define BOARD_FPGA_EMIF2_REQUIRED_MASK \
    (BOARD_FPGA_EMIF2_CONFIGURED |     \
     BOARD_FPGA_EMIF2_PINMAP_VALID |   \
     BOARD_FPGA_EMIF2_TEST2_MATCH |    \
     BOARD_FPGA_EMIF2_TEST3_MATCH)

#define BOARD_FPGA_EMIF2_DIAGNOSTIC_MASK \
    (BOARD_FPGA_EMIF2_REQUIRED_MASK | BOARD_FPGA_EMIF2_TEST1_MATCH)

#define BOARD_FPGA_FAIL_NONE            0x0000U
#define BOARD_FPGA_FAIL_CONFIG          0x0001U
#define BOARD_FPGA_FAIL_TEST1           0x0002U
#define BOARD_FPGA_FAIL_TEST2           0x0003U
#define BOARD_FPGA_FAIL_TEST3           0x0004U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 failCode;
    volatile BoardTest_U16 test1Value;
    volatile BoardTest_U16 test2Value;
    volatile BoardTest_U16 test3Value;
} BoardFpga_Emif2Snapshot;

extern volatile BoardFpga_Emif2Snapshot gBoardFpgaEmif2Snapshot;

BoardTest_Result BoardFpga_EvaluateEmif2Status(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 test1Value,
    BoardTest_U16 test2Value,
    BoardTest_U16 test3Value,
    BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_U16 BoardFpga_PrepareEmif2Interface(void);
BoardTest_Result BoardFpga_RunEmif2BasicTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
