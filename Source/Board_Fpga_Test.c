#include "Board_Fpga_Test.h"
#include "Board_Emif_Test.h"
#include "Board_Pinmap.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardFpga_Emif2Snapshot gBoardFpgaEmif2Snapshot =
{
    0U,
    BOARD_FPGA_FAIL_NONE,
    0U,
    0U,
    0U
};

BoardTest_Result BoardFpga_EvaluateEmif2Status(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 test1Value,
    BoardTest_U16 test2Value,
    BoardTest_U16 test3Value,
    BoardTest_Record *record)
{
    (void)test1Value;

    record->rawValue = (((BoardTest_U32)statusMask & 0xFFFFUL) << 16U) |
                       ((BoardTest_U32)failCode & 0xFFFFUL);
    record->measuredValue = (float)statusMask;
    record->expectedMin = (float)BOARD_FPGA_EMIF2_REQUIRED_MASK;
    record->expectedMax = (float)BOARD_FPGA_EMIF2_DIAGNOSTIC_MASK;

    if(((statusMask & BOARD_FPGA_EMIF2_REQUIRED_MASK) ==
        BOARD_FPGA_EMIF2_REQUIRED_MASK) &&
       (failCode == BOARD_FPGA_FAIL_NONE) &&
       (test2Value == BOARD_FPGA_TEST2_EXPECTED) &&
       (test3Value == BOARD_FPGA_TEST3_EXPECTED))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_FPGA_EMIF2_BASIC;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static void BoardFpga_ConfigureEmif2Gpio(void)
{
    BoardTest_U16 index;

    for(index = 0U; index < 16U; index++)
    {
        GPIO_SetupPinMux(BOARD_PIN_EMIF2_XD(index), GPIO_MUX_CPU1, 3U);
        GPIO_SetupPinOptions(BOARD_PIN_EMIF2_XD(index),
                            GPIO_INPUT,
                            GPIO_ASYNC);
    }

    for(index = 0U; index < 12U; index++)
    {
        GPIO_SetupPinMux(BOARD_PIN_EMIF2_XA(index), GPIO_MUX_CPU1, 3U);
        GPIO_SetupPinOptions(BOARD_PIN_EMIF2_XA(index),
                            GPIO_OUTPUT,
                            GPIO_PUSHPULL);
    }

    GPIO_SetupPinMux(BOARD_PIN_EMIF2_XBA0, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_XBA1, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_CS2, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_WAIT, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_CLK, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_RNW, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_WE, GPIO_MUX_CPU1, 3U);
    GPIO_SetupPinMux(BOARD_PIN_EMIF2_OE, GPIO_MUX_CPU1, 3U);

    GPIO_SetupPinOptions(BOARD_PIN_EMIF2_WAIT, GPIO_INPUT, GPIO_ASYNC);
}

static BoardTest_U16 BoardFpga_IsEmif2PinmapValid(void)
{
    return (BOARD_PIN_EMIF2_XA(0U) == 98U) &&
           (BOARD_PIN_EMIF2_XA(11U) == 109U) &&
           (BOARD_PIN_EMIF2_XD(0U) == 68U) &&
           (BOARD_PIN_EMIF2_XD(15U) == 53U) &&
           (BOARD_PIN_EMIF2_XBA0 == 111U) &&
           (BOARD_PIN_EMIF2_XBA1 == 112U) &&
           (BOARD_PIN_EMIF2_CS2 == 116U) &&
           (BOARD_PIN_EMIF2_RNW == 119U) &&
           (BOARD_PIN_EMIF2_WE == 120U) &&
           (BOARD_PIN_EMIF2_OE == 121U);
}

static BoardTest_U16 BoardFpga_Read16(BoardTest_U32 address)
{
    return *((volatile Uint16 *)address);
}

BoardTest_U16 BoardFpga_PrepareEmif2Interface(void)
{
    BoardEmif_ConfigureExternalAsync();
    BoardFpga_ConfigureEmif2Gpio();
    asm(" RPT #7 || NOP");
    return BoardFpga_IsEmif2PinmapValid();
}

BoardTest_Result BoardFpga_RunEmif2BasicTest(BoardTest_Record *record)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 test1Value;
    BoardTest_U16 test2Value;
    BoardTest_U16 test3Value;

    statusMask = 0U;
    failCode = BOARD_FPGA_FAIL_NONE;
    test1Value = 0U;
    test2Value = 0U;
    test3Value = 0U;

    BoardFpga_PrepareEmif2Interface();
    statusMask |= BOARD_FPGA_EMIF2_CONFIGURED;

    if(BoardFpga_IsEmif2PinmapValid() != 0U)
    {
        statusMask |= BOARD_FPGA_EMIF2_PINMAP_VALID;
    }
    else
    {
        failCode = BOARD_FPGA_FAIL_CONFIG;
    }

    if(failCode == BOARD_FPGA_FAIL_NONE)
    {
        test1Value = BoardFpga_Read16(BOARD_FPGA_EMIF2_TEST1_ADDR);
        test2Value = BoardFpga_Read16(BOARD_FPGA_EMIF2_TEST2_ADDR);
        test3Value = BoardFpga_Read16(BOARD_FPGA_EMIF2_TEST3_ADDR);

        if(test1Value == BOARD_FPGA_TEST1_EXPECTED)
        {
            statusMask |= BOARD_FPGA_EMIF2_TEST1_MATCH;
        }

        if(test2Value == BOARD_FPGA_TEST2_EXPECTED)
        {
            statusMask |= BOARD_FPGA_EMIF2_TEST2_MATCH;
        }
        else if(failCode == BOARD_FPGA_FAIL_NONE)
        {
            failCode = BOARD_FPGA_FAIL_TEST2;
        }

        if(test3Value == BOARD_FPGA_TEST3_EXPECTED)
        {
            statusMask |= BOARD_FPGA_EMIF2_TEST3_MATCH;
        }
        else if(failCode == BOARD_FPGA_FAIL_NONE)
        {
            failCode = BOARD_FPGA_FAIL_TEST3;
        }
    }

    gBoardFpgaEmif2Snapshot.statusMask = statusMask;
    gBoardFpgaEmif2Snapshot.failCode = failCode;
    gBoardFpgaEmif2Snapshot.test1Value = test1Value;
    gBoardFpgaEmif2Snapshot.test2Value = test2Value;
    gBoardFpgaEmif2Snapshot.test3Value = test3Value;

    return BoardFpga_EvaluateEmif2Status(statusMask,
                                         failCode,
                                         test1Value,
                                         test2Value,
                                         test3Value,
                                         record);
}
#endif
