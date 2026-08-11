#include "Board_Di_Test.h"
#include "Board_Fpga_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#endif

volatile BoardDi_FpgaSnapshot gBoardDiFpgaSnapshot =
{
    0U,
    BOARD_DI_FAIL_NONE,
    BOARD_DI_DEFAULT_CHANNEL_MASK,
    0U,
    0U,
    0U,
    0U,
    0U,
    0U
};

BoardTest_Result BoardDi_EvaluateFpgaStatus(
    BoardTest_U16 statusMask,
    BoardTest_U16 failCode,
    BoardTest_U16 requestedChannelMask,
    BoardTest_U16 observedInactiveMask,
    BoardTest_U16 observedActiveMask,
    BoardTest_U16 transitionMask,
    BoardTest_Record *record)
{
    BoardTest_U16 expectedMask;

    expectedMask = requestedChannelMask & BOARD_DI_INPUT_MASK;
    record->rawValue =
        (((BoardTest_U32)observedActiveMask & 0xFFFFUL) << 16U) |
        ((BoardTest_U32)observedInactiveMask & 0xFFFFUL);
    record->measuredValue = (float)(transitionMask & expectedMask);
    record->expectedMin = (float)expectedMask;
    record->expectedMax = (float)expectedMask;

    if((expectedMask != 0U) &&
       ((statusMask & BOARD_DI_REQUIRED_STATUS_MASK) ==
        BOARD_DI_REQUIRED_STATUS_MASK) &&
       (failCode == BOARD_DI_FAIL_NONE) &&
       ((observedInactiveMask & expectedMask) == expectedMask) &&
       ((observedActiveMask & expectedMask) == expectedMask) &&
       ((transitionMask & expectedMask) == expectedMask))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_DI_EXTERNAL;
    return (failCode == BOARD_DI_FAIL_TIMEOUT) ?
           BOARD_TEST_RESULT_TIMEOUT : BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardDi_Read16(BoardTest_U32 address)
{
    return *((volatile Uint16 *)address);
}

BoardTest_Result BoardDi_RunFpgaExternalTest(
    BoardTest_Record *record,
    BoardTest_U16 requestedChannelMask)
{
    BoardTest_U16 statusMask;
    BoardTest_U16 failCode;
    BoardTest_U16 expectedMask;
    BoardTest_U16 rawValue;
    BoardTest_U16 previousRawValue;
    BoardTest_U16 observedInactiveMask;
    BoardTest_U16 observedActiveMask;
    BoardTest_U16 transitionMask;
    BoardTest_U16 sampleCount;
    BoardTest_U16 magic2Value;
    BoardTest_U16 magic3Value;

    statusMask = 0U;
    failCode = BOARD_DI_FAIL_NONE;
    expectedMask = requestedChannelMask & BOARD_DI_INPUT_MASK;
    observedInactiveMask = 0U;
    observedActiveMask = 0U;
    transitionMask = 0U;
    sampleCount = 0U;

    gBoardDiFpgaSnapshot.statusMask = 0U;
    gBoardDiFpgaSnapshot.failCode = BOARD_DI_FAIL_NONE;
    gBoardDiFpgaSnapshot.requestedChannelMask = expectedMask;
    gBoardDiFpgaSnapshot.firstRawValue = 0U;
    gBoardDiFpgaSnapshot.lastRawValue = 0U;
    gBoardDiFpgaSnapshot.observedInactiveMask = 0U;
    gBoardDiFpgaSnapshot.observedActiveMask = 0U;
    gBoardDiFpgaSnapshot.transitionMask = 0U;
    gBoardDiFpgaSnapshot.sampleCount = 0U;

    if(expectedMask == 0U)
    {
        failCode = BOARD_DI_FAIL_CHANNEL_MASK;
    }
    else if(BoardFpga_PrepareEmif2Interface() != 0U)
    {
        statusMask |= BOARD_DI_STATUS_CONFIGURED;
    }
    else
    {
        failCode = BOARD_DI_FAIL_CONFIG;
    }

    if(failCode == BOARD_DI_FAIL_NONE)
    {
        magic2Value = BoardDi_Read16(BOARD_FPGA_EMIF2_TEST2_ADDR);
        magic3Value = BoardDi_Read16(BOARD_FPGA_EMIF2_TEST3_ADDR);
        if((magic2Value == BOARD_FPGA_TEST2_EXPECTED) &&
           (magic3Value == BOARD_FPGA_TEST3_EXPECTED))
        {
            statusMask |= BOARD_DI_STATUS_MAGIC_VALID;
        }
        else
        {
            failCode = BOARD_DI_FAIL_MAGIC;
        }
    }

    if(failCode == BOARD_DI_FAIL_NONE)
    {
        previousRawValue =
            BoardDi_Read16(BOARD_DI_FPGA_INPUT_ADDR) &
            BOARD_DI_INPUT_MASK;
        gBoardDiFpgaSnapshot.firstRawValue = previousRawValue;
        observedInactiveMask |= previousRawValue;
        observedActiveMask |=
            (BoardTest_U16)(~previousRawValue) & BOARD_DI_INPUT_MASK;
        sampleCount = 1U;

        while((sampleCount < BOARD_DI_SAMPLE_LIMIT) &&
              (((observedInactiveMask & expectedMask) != expectedMask) ||
               ((observedActiveMask & expectedMask) != expectedMask) ||
               ((transitionMask & expectedMask) != expectedMask)))
        {
            DELAY_US(BOARD_DI_SAMPLE_INTERVAL_US);
            rawValue = BoardDi_Read16(BOARD_DI_FPGA_INPUT_ADDR) &
                       BOARD_DI_INPUT_MASK;
            observedInactiveMask |= rawValue;
            observedActiveMask |=
                (BoardTest_U16)(~rawValue) & BOARD_DI_INPUT_MASK;
            transitionMask |= rawValue ^ previousRawValue;
            previousRawValue = rawValue;
            sampleCount++;
        }

        gBoardDiFpgaSnapshot.lastRawValue = previousRawValue;

        if((observedInactiveMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_INACTIVE_SEEN;
        }

        if((observedActiveMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_ACTIVE_SEEN;
        }

        if((transitionMask & expectedMask) == expectedMask)
        {
            statusMask |= BOARD_DI_STATUS_TRANSITION_SEEN;
        }

        if((statusMask & BOARD_DI_REQUIRED_STATUS_MASK) !=
           BOARD_DI_REQUIRED_STATUS_MASK)
        {
            failCode = BOARD_DI_FAIL_TIMEOUT;
        }
    }

    gBoardDiFpgaSnapshot.statusMask = statusMask;
    gBoardDiFpgaSnapshot.failCode = failCode;
    gBoardDiFpgaSnapshot.observedInactiveMask = observedInactiveMask;
    gBoardDiFpgaSnapshot.observedActiveMask = observedActiveMask;
    gBoardDiFpgaSnapshot.transitionMask = transitionMask;
    gBoardDiFpgaSnapshot.sampleCount = sampleCount;

    return BoardDi_EvaluateFpgaStatus(statusMask,
                                      failCode,
                                      expectedMask,
                                      observedInactiveMask,
                                      observedActiveMask,
                                      transitionMask,
                                      record);
}
#endif
