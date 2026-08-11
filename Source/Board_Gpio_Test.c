#include "Board_Gpio_Test.h"
#include "Board_Profile.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"

static BoardTest_U16 BoardGpio_HeartbeatConfigured = 0U;
static BoardTest_U16 BoardGpio_HeartbeatState = 0U;
static BoardTest_U16 BoardGpio_HeartbeatLed1 = BOARD_PROFILE_PIN_UNUSED;
static BoardTest_U16 BoardGpio_HeartbeatLed2 = BOARD_PROFILE_PIN_UNUSED;
#endif

BoardTest_Result BoardGpio_EvaluateProgramLedReadback(BoardTest_U16 readbackMask,
                                                       BoardTest_Record *record)
{
    record->rawValue = (BoardTest_U32)readbackMask;
    record->measuredValue = (float)readbackMask;
    record->expectedMin = (float)BOARD_GPIO_LED_READBACK_MASK;
    record->expectedMax = (float)BOARD_GPIO_LED_READBACK_MASK;

    if(readbackMask == BOARD_GPIO_LED_READBACK_MASK)
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_GPIO_READBACK;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static BoardTest_U16 BoardGpio_CheckPin(BoardTest_U16 pin,
                                        BoardTest_U16 expected)
{
    return (GPIO_ReadPin(pin) == expected) ? 1U : 0U;
}

static BoardTest_U16 BoardGpio_GetProgramLedPins(BoardTest_U16 *led1,
                                                  BoardTest_U16 *led2)
{
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    if(hardware == 0)
    {
        return 0U;
    }

    *led1 = hardware->pins.led1;
    *led2 = hardware->pins.led2;
    if((*led1 == BOARD_PROFILE_PIN_UNUSED) ||
       (*led2 == BOARD_PROFILE_PIN_UNUSED) ||
       (*led1 == *led2))
    {
        return 0U;
    }
    return 1U;
}

BoardTest_Result BoardGpio_RunProgramLedTest(BoardTest_Record *record)
{
    BoardTest_U16 led1;
    BoardTest_U16 led2;
    BoardTest_U16 readbackMask = 0U;

    if(BoardGpio_GetProgramLedPins(&led1, &led2) == 0U)
    {
        record->rawValue = 0UL;
        record->measuredValue = 0.0F;
        record->expectedMin = 2.0F;
        record->expectedMax = 2.0F;
        record->errorCode = BOARD_TEST_ERROR_PROFILE_PINMAP;
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    GPIO_SetupPinMux(led1, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinMux(led2, GPIO_MUX_CPU1, 0U);

    GPIO_WritePin(led1, 0U);
    GPIO_WritePin(led2, 0U);
    GPIO_SetupPinOptions(led1, GPIO_OUTPUT, 0U);
    GPIO_SetupPinOptions(led2, GPIO_OUTPUT, 0U);

    GPIO_WritePin(led1, 1U);
    GPIO_WritePin(led2, 0U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
    readbackMask |= BoardGpio_CheckPin(led1, 1U) << 0U;
    readbackMask |= BoardGpio_CheckPin(led2, 0U) << 1U;

    GPIO_WritePin(led1, 0U);
    GPIO_WritePin(led2, 1U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
    readbackMask |= BoardGpio_CheckPin(led1, 0U) << 2U;
    readbackMask |= BoardGpio_CheckPin(led2, 1U) << 3U;

    GPIO_WritePin(led1, 0U);
    GPIO_WritePin(led2, 0U);
    readbackMask |= BoardGpio_CheckPin(led1, 0U) << 4U;
    readbackMask |= BoardGpio_CheckPin(led2, 0U) << 5U;

    GPIO_SetupPinOptions(led1, GPIO_INPUT, 0U);
    GPIO_SetupPinOptions(led2, GPIO_INPUT, 0U);
    BoardGpio_HeartbeatConfigured = 0U;

    return BoardGpio_EvaluateProgramLedReadback(readbackMask, record);
}

void BoardGpio_ServiceHeartbeat(void)
{
    BoardTest_U16 led1;
    BoardTest_U16 led2;

    if(BoardGpio_GetProgramLedPins(&led1, &led2) == 0U)
    {
        BoardGpio_HeartbeatConfigured = 0U;
        return;
    }

    if((BoardGpio_HeartbeatLed1 != led1) ||
       (BoardGpio_HeartbeatLed2 != led2))
    {
        BoardGpio_HeartbeatConfigured = 0U;
        BoardGpio_HeartbeatLed1 = led1;
        BoardGpio_HeartbeatLed2 = led2;
    }

    if(BoardGpio_HeartbeatConfigured == 0U)
    {
        GPIO_SetupPinMux(led1, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinMux(led2, GPIO_MUX_CPU1, 0U);
        GPIO_WritePin(led1, 0U);
        GPIO_WritePin(led2, 0U);
        GPIO_SetupPinOptions(led1, GPIO_OUTPUT, 0U);
        GPIO_SetupPinOptions(led2, GPIO_OUTPUT, 0U);
        BoardGpio_HeartbeatConfigured = 1U;
    }

    BoardGpio_HeartbeatState ^= 1U;
    GPIO_WritePin(led1, BoardGpio_HeartbeatState);
    GPIO_WritePin(led2, BoardGpio_HeartbeatState ^ 1U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
}
#endif
