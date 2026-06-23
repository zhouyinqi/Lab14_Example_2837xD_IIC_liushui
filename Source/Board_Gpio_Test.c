#include "Board_Gpio_Test.h"
#include "Board_Pinmap.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"

static BoardTest_U16 BoardGpio_HeartbeatConfigured = 0U;
static BoardTest_U16 BoardGpio_HeartbeatState = 0U;
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

BoardTest_Result BoardGpio_RunProgramLedTest(BoardTest_Record *record)
{
    BoardTest_U16 readbackMask = 0U;

    GPIO_SetupPinMux(BOARD_PIN_LED1, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinMux(BOARD_PIN_LED2, GPIO_MUX_CPU1, 0U);

    GPIO_WritePin(BOARD_PIN_LED1, 0U);
    GPIO_WritePin(BOARD_PIN_LED2, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_LED1, GPIO_OUTPUT, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_LED2, GPIO_OUTPUT, 0U);

    GPIO_WritePin(BOARD_PIN_LED1, 1U);
    GPIO_WritePin(BOARD_PIN_LED2, 0U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED1, 1U) << 0U;
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED2, 0U) << 1U;

    GPIO_WritePin(BOARD_PIN_LED1, 0U);
    GPIO_WritePin(BOARD_PIN_LED2, 1U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED1, 0U) << 2U;
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED2, 1U) << 3U;

    GPIO_WritePin(BOARD_PIN_LED1, 0U);
    GPIO_WritePin(BOARD_PIN_LED2, 0U);
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED1, 0U) << 4U;
    readbackMask |= BoardGpio_CheckPin(BOARD_PIN_LED2, 0U) << 5U;

    GPIO_SetupPinOptions(BOARD_PIN_LED1, GPIO_INPUT, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_LED2, GPIO_INPUT, 0U);
    BoardGpio_HeartbeatConfigured = 0U;

    return BoardGpio_EvaluateProgramLedReadback(readbackMask, record);
}

void BoardGpio_ServiceHeartbeat(void)
{
    if(BoardGpio_HeartbeatConfigured == 0U)
    {
        GPIO_SetupPinMux(BOARD_PIN_LED1, GPIO_MUX_CPU1, 0U);
        GPIO_SetupPinMux(BOARD_PIN_LED2, GPIO_MUX_CPU1, 0U);
        GPIO_WritePin(BOARD_PIN_LED1, 0U);
        GPIO_WritePin(BOARD_PIN_LED2, 0U);
        GPIO_SetupPinOptions(BOARD_PIN_LED1, GPIO_OUTPUT, 0U);
        GPIO_SetupPinOptions(BOARD_PIN_LED2, GPIO_OUTPUT, 0U);
        BoardGpio_HeartbeatConfigured = 1U;
    }

    BoardGpio_HeartbeatState ^= 1U;
    GPIO_WritePin(BOARD_PIN_LED1, BoardGpio_HeartbeatState);
    GPIO_WritePin(BOARD_PIN_LED2, BoardGpio_HeartbeatState ^ 1U);
    DELAY_US(BOARD_GPIO_LED_HOLD_US);
}
#endif
