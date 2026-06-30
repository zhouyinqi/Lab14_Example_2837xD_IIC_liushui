#include "F28x_Project.h"
#include "Board_Adc_Test.h"
#include "Board_Can_Test.h"
#include "Board_Emif_Test.h"
#include "Board_Ethernet_Test.h"
#include "Board_Gpio_Test.h"
#include "Board_I2c_Test.h"
#include "Board_Pwm_Test.h"
#include "Board_Sci_Test.h"
#include "Board_Spi_Test.h"
#include "Board_System_Test.h"
#include "Board_Test_Target.h"
#include "Hpd_Test_Limits.h"

extern void Led_Test(void);
extern volatile Uint16 RtcStatus;

volatile BoardTest_CommandMailbox gBoardTestCommandMailbox =
{
    BOARD_TEST_COMMAND_NONE,
    BOARD_TEST_INVALID_ID,
    BOARD_TEST_STAGE_BOARD_ONLY,
    BOARD_TEST_HPD_INPUT_SOFTWARE_PHYSICAL,
    HPD_BUS_VOLTAGE_TARGET_V,
    HPD_LINE_VOLTAGE_TARGET_V,
    HPD_LINE_FREQUENCY_TARGET_HZ,
    HPD_CURRENT_TARGET_A,
    HPD_CURRENT_FREQUENCY_TARGET_HZ,
    25.0F,
    BOARD_TEST_RESULT_NOT_RUN
};

BoardTest_Result BoardTest_TargetExecute(BoardTest_U16 testId,
                                         BoardTest_Record *record)
{
    switch(testId)
    {
        case BOARD_TEST_ID_SYS_STARTUP:
            record->measuredValue = 1.0F;
            record->expectedMin = 1.0F;
            record->expectedMax = 1.0F;
            return BOARD_TEST_RESULT_PASS;

        case BOARD_TEST_ID_SYS_CLOCK:
            return BoardSystem_RunClockTest(record);

        case BOARD_TEST_ID_SYS_TIMER:
            return BoardSystem_RunTimerTest(record);

        case BOARD_TEST_ID_SYS_WATCHDOG:
            return BoardSystem_RunWatchdogTest(record);

        case BOARD_TEST_ID_RAM:
            return BoardSystem_RunRamTest(record);

        case BOARD_TEST_ID_FLASH:
            return BoardSystem_RunFlashTest(record);

        case BOARD_TEST_ID_I2C_RTC:
            Led_Test();
            record->rawValue = (BoardTest_U32)RtcStatus;
            if(RtcStatus == 0U)
            {
                return BOARD_TEST_RESULT_PASS;
            }
            return BOARD_TEST_RESULT_FAIL;

        case BOARD_TEST_ID_I2C_B_TMP116:
            return BoardI2c_RunTmp116Test(record);

        case BOARD_TEST_ID_SCI_HOST:
            return BoardSci_RunLoopbackTest(record);

        case BOARD_TEST_ID_CAN_HOST:
            return BoardCan_RunLoopbackTest(record);

        case BOARD_TEST_ID_SPIA_INTERNAL:
            return BoardSpi_RunSpiaLoopbackTest(record);

        case BOARD_TEST_ID_SPIB_INTERNAL:
            return BoardSpi_RunSpibLoopbackTest(record);

        case BOARD_TEST_ID_SPIC_INTERNAL:
            return BoardSpi_RunSpicLoopbackTest(record);

        case BOARD_TEST_ID_GPIO:
            return BoardGpio_RunProgramLedTest(record);

        case BOARD_TEST_ID_ADC_BASIC:
            return BoardAdc_RunBasicSampleTest(record);

        case BOARD_TEST_ID_PWM_BASIC:
            return BoardPwm_RunSafetyConfigTest(record);

        case BOARD_TEST_ID_EMIF_BASIC:
            return BoardEmif_RunBasicConfigTest(record);

        case BOARD_TEST_ID_ETHERNET:
            return BoardEthernet_RunW5300BasicTest(record);

        case BOARD_TEST_ID_ETHERNET_SOCKET:
            return BoardEthernet_RunW5300SocketTest(record);

        case BOARD_TEST_ID_ETHERNET_TCP_LINK:
            return BoardEthernet_RunW5300TcpLinkTest(record);

        default:
            record->errorCode = BOARD_TEST_ERROR_UNSUPPORTED;
            return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
}

void BoardTest_TargetPoll(void)
{
    BoardTest_U16 command = gBoardTestCommandMailbox.command;

    if(command == BOARD_TEST_COMMAND_START_AUTO)
    {
        gBoardTestCommandMailbox.lastCommandResult = BoardTest_StartAuto();
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_EXTERNAL_CONNECTED)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartStageAuto(BOARD_TEST_STAGE_EXTERNAL_CONNECTED);
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_HPD_INJECTION)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartHpdInjectionAuto(
                gBoardTestCommandMailbox.hpdInputSource);
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_STAGE_AUTO)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartStageAuto(gBoardTestCommandMailbox.stage);
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_START_SINGLE)
    {
        gBoardTestCommandMailbox.lastCommandResult =
            BoardTest_StartSingleInStage(gBoardTestCommandMailbox.testId,
                                         gBoardTestCommandMailbox.stage);
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }
    else if(command == BOARD_TEST_COMMAND_STOP)
    {
        BoardEthernet_AbortW5300TcpLinkTest();
        BoardTest_Stop();
        gBoardTestCommandMailbox.lastCommandResult = BOARD_TEST_RESULT_PASS;
        gBoardTestCommandMailbox.command = BOARD_TEST_COMMAND_NONE;
    }

    BoardTest_Process(BoardTest_TargetExecute);
}
