#include "Board_Test.h"

#define BOARD_TEST_INVALID_INDEX 0xFFFFU
#define BOARD_TEST_STAGE_COUNT   3U

#define BOARD_TEST_STAGE_BOARD_MASK \
    BOARD_TEST_STAGE_MASK_BOARD_ONLY
#define BOARD_TEST_STAGE_EXTERNAL_MASK \
    BOARD_TEST_STAGE_MASK_EXTERNAL_CONNECTED
#define BOARD_TEST_STAGE_HPD_MASK \
    BOARD_TEST_STAGE_MASK_HPD_INJECTION
#define BOARD_TEST_STAGE_BOARD_HPD_MASK \
    (BOARD_TEST_STAGE_MASK_BOARD_ONLY | BOARD_TEST_STAGE_MASK_HPD_INJECTION)

static const BoardTest_Descriptor BoardTest_Descriptors[BOARD_TEST_ITEM_COUNT] =
{
    {BOARD_TEST_ID_SYS_STARTUP, "SYS_STARTUP", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SYS_CLOCK, "SYS_CLOCK", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SYS_INTERRUPT, "SYS_INTERRUPT", 0U, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SYS_TIMER, "SYS_TIMER", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SYS_WATCHDOG, "SYS_WATCHDOG", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_RAM, "RAM", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_FLASH, "FLASH", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_GPIO, "GPIO", BOARD_TEST_CAP_IMPLEMENTED | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_ADC_BASIC, "ADC_BASIC", BOARD_TEST_CAP_IMPLEMENTED | BOARD_TEST_CAP_EXTERNAL_INJECT, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_HPD_MASK, 1U},
    {BOARD_TEST_ID_PWM_BASIC, "PWM_BASIC", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 0U},
    {BOARD_TEST_ID_CAN_HOST, "CAN_INTERNAL", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SCI_HOST, "SCI_INTERNAL", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SPIA_INTERNAL, "SPIA_INTERNAL", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SPIB_INTERNAL, "SPIB_INTERNAL", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SPIC_INTERNAL, "SPIC_INTERNAL", BOARD_TEST_CAP_IMPLEMENTED, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_SPI_EXTERNAL, "SPI_EXTERNAL", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_I2C_RTC, "I2C_RTC", BOARD_TEST_CAP_IMPLEMENTED | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_EMIF_EXTERNAL, "EMIF_EXTERNAL", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_ETHERNET, "ETHERNET", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_I2C_B_TMP116, "I2C_B_TMP116", BOARD_TEST_CAP_IMPLEMENTED | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_BOARD_MASK, 1U},
    {BOARD_TEST_ID_CAN_EXTERNAL, "CAN_USB_EXTERNAL", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_I2C_EXTERNAL, "I2C_EXTERNAL", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_SCI_RS485_EXTERNAL, "SCI_RS485_EXTERNAL", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_HPD_ESTOP_DI3, "HPD_ESTOP_DI3", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_HPD_INDICATOR_DO3, "HPD_INDICATOR_DO3", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_SAFE, BOARD_TEST_STAGE_EXTERNAL_MASK, 1U},
    {BOARD_TEST_ID_HPD_DRIVER_1, "HPD_DRIVER_1", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 0U},
    {BOARD_TEST_ID_HPD_DRIVER_2, "HPD_DRIVER_2", BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 0U},
    {BOARD_TEST_ID_HPD_NTC_1, "HPD_NTC_1", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_NTC_2, "HPD_NTC_2", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_BUS_1, "HPD_BUS_1", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_BUS_2, "HPD_BUS_2", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_LOW_VOLTAGE, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_LINE_1, "HPD_LINE_1", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_LINE_2, "HPD_LINE_2", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_CURRENT_1, "HPD_CURRENT_1", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_CURRENT_2, "HPD_CURRENT_2", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_PHASE_1, "HPD_PHASE_1", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U},
    {BOARD_TEST_ID_HPD_PHASE_2, "HPD_PHASE_2", BOARD_TEST_CAP_SOFTWARE_MOCK | BOARD_TEST_CAP_EXTERNAL_INJECT | BOARD_TEST_CAP_TEST_BENCH, BOARD_TEST_RISK_HIGH_POWER, BOARD_TEST_STAGE_HPD_MASK, 1U}
};

BoardTest_Manager gBoardTestManager;

static BoardTest_U16 BoardTest_FindIndex(BoardTest_U16 testId)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_TEST_ITEM_COUNT; index++)
    {
        if(BoardTest_Descriptors[index].id == testId)
        {
            return index;
        }
    }

    return BOARD_TEST_INVALID_INDEX;
}

static BoardTest_U16 BoardTest_StageToMask(BoardTest_U16 stage)
{
    switch(stage)
    {
        case BOARD_TEST_STAGE_BOARD_ONLY:
            return BOARD_TEST_STAGE_MASK_BOARD_ONLY;

        case BOARD_TEST_STAGE_EXTERNAL_CONNECTED:
            return BOARD_TEST_STAGE_MASK_EXTERNAL_CONNECTED;

        case BOARD_TEST_STAGE_HPD_INJECTION:
            return BOARD_TEST_STAGE_MASK_HPD_INJECTION;

        default:
            return 0U;
    }
}

static BoardTest_U16 BoardTest_IsStageSupported(BoardTest_U16 index,
                                                BoardTest_U16 stage)
{
    BoardTest_U16 stageMask = BoardTest_StageToMask(stage);

    if(stageMask == 0U)
    {
        return 0U;
    }

    return (BoardTest_Descriptors[index].stageMask & stageMask) != 0U;
}

static BoardTest_U16 BoardTest_IsSafetyAllowed(BoardTest_U16 index,
                                               BoardTest_U16 stage)
{
    if(BoardTest_Descriptors[index].risk == BOARD_TEST_RISK_SAFE)
    {
        return 1U;
    }

    if((stage == BOARD_TEST_STAGE_HPD_INJECTION) &&
       (BoardTest_Descriptors[index].risk == BOARD_TEST_RISK_LOW_VOLTAGE))
    {
        return 1U;
    }

    return 0U;
}

static void BoardTest_ResetRecords(void)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_TEST_ITEM_COUNT; index++)
    {
        gBoardTestManager.records[index].id = BoardTest_Descriptors[index].id;
        gBoardTestManager.records[index].result = BOARD_TEST_RESULT_NOT_RUN;
        gBoardTestManager.records[index].errorCode = BOARD_TEST_ERROR_NONE;
        gBoardTestManager.records[index].rawValue = 0UL;
        gBoardTestManager.records[index].measuredValue = 0.0F;
        gBoardTestManager.records[index].expectedMin = 0.0F;
        gBoardTestManager.records[index].expectedMax = 0.0F;
    }
}

static BoardTest_Result BoardTest_RunItem(BoardTest_U16 index,
                                          BoardTest_Executor executor)
{
    BoardTest_Result result;
    BoardTest_Record *record = &gBoardTestManager.records[index];

    record->result = BOARD_TEST_RESULT_RUNNING;
    record->errorCode = BOARD_TEST_ERROR_NONE;

    if(executor == 0)
    {
        record->errorCode = BOARD_TEST_ERROR_UNSUPPORTED;
        result = BOARD_TEST_RESULT_NOT_SUPPORTED;
    }
    else
    {
        result = executor(record->id, record);
    }

    record->result = (BoardTest_U16)result;
    return result;
}

void BoardTest_Init(void)
{
    gBoardTestManager.mode = BOARD_TEST_MODE_BOOT_SAFE;
    gBoardTestManager.activeTestId = BOARD_TEST_INVALID_ID;
    gBoardTestManager.autoIndex = 0U;
    gBoardTestManager.activeStage = BOARD_TEST_STAGE_BOARD_ONLY;
    gBoardTestManager.hpdInputSource = BOARD_TEST_HPD_INPUT_SOFTWARE_PHYSICAL;
    gBoardTestManager.pwmAllowed = 0U;
    gBoardTestManager.driverAllowed = 0U;
    BoardTest_ResetRecords();
    gBoardTestManager.mode = BOARD_TEST_MODE_IDLE;
}

BoardTest_Result BoardTest_StartAuto(void)
{
    return BoardTest_StartStageAuto(BOARD_TEST_STAGE_BOARD_ONLY);
}

BoardTest_Result BoardTest_StartStageAuto(BoardTest_U16 stage)
{
    if(gBoardTestManager.mode != BOARD_TEST_MODE_IDLE)
    {
        return BOARD_TEST_RESULT_FAIL;
    }

    if(BoardTest_StageToMask(stage) == 0U)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    BoardTest_ResetRecords();
    gBoardTestManager.autoIndex = 0U;
    gBoardTestManager.activeTestId = BOARD_TEST_INVALID_ID;
    gBoardTestManager.activeStage = stage;
    gBoardTestManager.pwmAllowed = 0U;
    gBoardTestManager.driverAllowed = 0U;
    gBoardTestManager.mode = BOARD_TEST_MODE_AUTO;
    return BOARD_TEST_RESULT_RUNNING;
}

BoardTest_Result BoardTest_StartHpdInjectionAuto(BoardTest_U16 inputSource)
{
    if((inputSource != BOARD_TEST_HPD_INPUT_SOFTWARE_PHYSICAL) &&
       (inputSource != BOARD_TEST_HPD_INPUT_REAL_ADC))
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    gBoardTestManager.hpdInputSource = inputSource;
    return BoardTest_StartStageAuto(BOARD_TEST_STAGE_HPD_INJECTION);
}

BoardTest_Result BoardTest_StartSingle(BoardTest_U16 testId)
{
    return BoardTest_StartSingleInStage(testId, BOARD_TEST_STAGE_BOARD_ONLY);
}

BoardTest_Result BoardTest_StartSingleInStage(BoardTest_U16 testId,
                                              BoardTest_U16 stage)
{
    BoardTest_U16 index;

    if(gBoardTestManager.mode != BOARD_TEST_MODE_IDLE)
    {
        return BOARD_TEST_RESULT_FAIL;
    }

    if(BoardTest_StageToMask(stage) == 0U)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    index = BoardTest_FindIndex(testId);
    if(index == BOARD_TEST_INVALID_INDEX)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    if(BoardTest_IsStageSupported(index, stage) == 0U)
    {
        gBoardTestManager.records[index].result = BOARD_TEST_RESULT_SKIP;
        gBoardTestManager.records[index].errorCode =
            BOARD_TEST_ERROR_STAGE_MISMATCH;
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    if(BoardTest_IsSafetyAllowed(index, stage) == 0U)
    {
        gBoardTestManager.records[index].result = BOARD_TEST_RESULT_SAFETY_LOCKED;
        gBoardTestManager.records[index].errorCode = BOARD_TEST_ERROR_SAFETY_LOCK;
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    gBoardTestManager.activeTestId = testId;
    gBoardTestManager.activeStage = stage;
    gBoardTestManager.records[index].result = BOARD_TEST_RESULT_RUNNING;
    gBoardTestManager.records[index].errorCode = BOARD_TEST_ERROR_NONE;
    gBoardTestManager.mode = BOARD_TEST_MODE_MANUAL;
    return BOARD_TEST_RESULT_RUNNING;
}

void BoardTest_Stop(void)
{
    BoardTest_U16 index = BoardTest_FindIndex(gBoardTestManager.activeTestId);

    if((index != BOARD_TEST_INVALID_INDEX) &&
       (gBoardTestManager.records[index].result == BOARD_TEST_RESULT_RUNNING))
    {
        gBoardTestManager.records[index].result = BOARD_TEST_RESULT_SKIP;
        gBoardTestManager.records[index].errorCode = BOARD_TEST_ERROR_ABORTED;
    }

    gBoardTestManager.activeTestId = BOARD_TEST_INVALID_ID;
    gBoardTestManager.autoIndex = 0U;
    gBoardTestManager.activeStage = BOARD_TEST_STAGE_BOARD_ONLY;
    gBoardTestManager.pwmAllowed = 0U;
    gBoardTestManager.driverAllowed = 0U;
    gBoardTestManager.mode = BOARD_TEST_MODE_IDLE;
}

void BoardTest_Process(BoardTest_Executor executor)
{
    BoardTest_U16 index;
    BoardTest_Result result;

    if(gBoardTestManager.mode == BOARD_TEST_MODE_MANUAL)
    {
        index = BoardTest_FindIndex(gBoardTestManager.activeTestId);
        if(index == BOARD_TEST_INVALID_INDEX)
        {
            BoardTest_Stop();
            return;
        }

        result = BoardTest_RunItem(index, executor);
        if(result != BOARD_TEST_RESULT_RUNNING)
        {
            gBoardTestManager.activeTestId = BOARD_TEST_INVALID_ID;
            gBoardTestManager.mode = BOARD_TEST_MODE_IDLE;
        }
        return;
    }

    if(gBoardTestManager.mode != BOARD_TEST_MODE_AUTO)
    {
        return;
    }

    if(gBoardTestManager.autoIndex >= BOARD_TEST_ITEM_COUNT)
    {
        BoardTest_Stop();
        return;
    }

    index = gBoardTestManager.autoIndex;
    gBoardTestManager.activeTestId = BoardTest_Descriptors[index].id;

    if((BoardTest_Descriptors[index].enabledInAuto == 0U) ||
       (BoardTest_IsStageSupported(index,
                                   gBoardTestManager.activeStage) == 0U) ||
       (BoardTest_IsSafetyAllowed(index,
                                  gBoardTestManager.activeStage) == 0U))
    {
        gBoardTestManager.records[index].result = BOARD_TEST_RESULT_SKIP;
        gBoardTestManager.autoIndex++;
        return;
    }

    result = BoardTest_RunItem(index, executor);
    if(result != BOARD_TEST_RESULT_RUNNING)
    {
        gBoardTestManager.autoIndex++;
    }
}

BoardTest_U16 BoardTest_GetCount(void)
{
    return BOARD_TEST_ITEM_COUNT;
}

const BoardTest_Descriptor *BoardTest_GetDescriptor(BoardTest_U16 index)
{
    if(index >= BOARD_TEST_ITEM_COUNT)
    {
        return 0;
    }

    return &BoardTest_Descriptors[index];
}

const BoardTest_Record *BoardTest_GetRecord(BoardTest_U16 testId)
{
    BoardTest_U16 index = BoardTest_FindIndex(testId);

    if(index == BOARD_TEST_INVALID_INDEX)
    {
        return 0;
    }

    return &gBoardTestManager.records[index];
}
