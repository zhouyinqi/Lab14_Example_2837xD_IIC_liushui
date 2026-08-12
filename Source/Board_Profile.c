#include "Board_Profile.h"

static const BoardProfile_Descriptor BoardProfile_Descriptors[] =
{
    {
        BOARD_PROFILE_ID_SYSTEM_MASTER,
        28377U,
        BOARD_PROFILE_HARDWARE_REVISION_SYSTEM_MASTER_V01,
        BOARD_PROFILE_CAP_EXTERNAL_SRAM |
        BOARD_PROFILE_CAP_FPGA_EMIF2 |
        BOARD_PROFILE_CAP_GPIO |
        BOARD_PROFILE_CAP_ADC |
        BOARD_PROFILE_CAP_PWM |
        BOARD_PROFILE_CAP_FPGA_DIDO |
        BOARD_PROFILE_CAP_SCIA |
        BOARD_PROFILE_CAP_SCIB |
        BOARD_PROFILE_CAP_CAN_B |
        BOARD_PROFILE_CAP_SPIA |
        BOARD_PROFILE_CAP_SPIB |
        BOARD_PROFILE_CAP_SPIB_FLASH_FRAM |
        BOARD_PROFILE_CAP_SPIC_EXTERNAL |
        BOARD_PROFILE_CAP_I2C_RTC |
        BOARD_PROFILE_CAP_I2C_TMP116,
        "System master board"
    },
    {
        BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER,
        28377U,
        BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V03,
        BOARD_PROFILE_CAP_GPIO |
        BOARD_PROFILE_CAP_ADC |
        BOARD_PROFILE_CAP_PWM |
        BOARD_PROFILE_CAP_DSP_DI |
        BOARD_PROFILE_CAP_DSP_DO |
        BOARD_PROFILE_CAP_DSP_HDO |
        BOARD_PROFILE_CAP_SCIA |
        BOARD_PROFILE_CAP_SCIB |
        BOARD_PROFILE_CAP_CAN_B |
        BOARD_PROFILE_CAP_SPIA |
        BOARD_PROFILE_CAP_SPIB |
        BOARD_PROFILE_CAP_SPIB_FLASH_FRAM,
        "Low voltage industrial inverter board"
    },
    {
        BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED,
        28377U,
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01,
        BOARD_PROFILE_CAP_GPIO |
        BOARD_PROFILE_CAP_ADC |
        BOARD_PROFILE_CAP_PWM |
        BOARD_PROFILE_CAP_DSP_DI |
        BOARD_PROFILE_CAP_DSP_DO |
        BOARD_PROFILE_CAP_RS422 |
        BOARD_PROFILE_CAP_CAN_A |
        BOARD_PROFILE_CAP_CAN_B |
        BOARD_PROFILE_CAP_SPIB |
        BOARD_PROFILE_CAP_SPIB_FLASH_FRAM |
        BOARD_PROFILE_CAP_SPIC_EXTERNAL,
        "Low altitude unified control board"
    }
};

#define BOARD_PROFILE_SYSTEM_W5300_IMPLEMENTED \
    (BOARD_PROFILE_CAP_EMIF_W5300 | \
     BOARD_PROFILE_CAP_EXTERNAL_SRAM | \
     BOARD_PROFILE_CAP_FPGA_EMIF2 | \
     BOARD_PROFILE_CAP_GPIO | \
     BOARD_PROFILE_CAP_ADC | \
     BOARD_PROFILE_CAP_PWM | \
     BOARD_PROFILE_CAP_FPGA_DIDO | \
     BOARD_PROFILE_CAP_SCIA | \
     BOARD_PROFILE_CAP_SCIB | \
     BOARD_PROFILE_CAP_CAN_B | \
     BOARD_PROFILE_CAP_SPIA | \
     BOARD_PROFILE_CAP_SPIB | \
     BOARD_PROFILE_CAP_SPIB_FLASH_FRAM | \
     BOARD_PROFILE_CAP_SPIC_EXTERNAL | \
     BOARD_PROFILE_CAP_I2C_RTC | \
     BOARD_PROFILE_CAP_I2C_TMP116)

#define BOARD_PROFILE_SYSTEM_W5300_VALIDATED \
    (BOARD_PROFILE_SYSTEM_W5300_IMPLEMENTED & \
     (~BOARD_PROFILE_CAP_SCIA))

#define BOARD_PROFILE_LOW_ALTITUDE_W5500_IMPLEMENTED \
    (BOARD_PROFILE_CAP_SPIC_W5500 | \
     BOARD_PROFILE_CAP_GPIO | \
     BOARD_PROFILE_CAP_CAN_B | \
     BOARD_PROFILE_CAP_RS422 | \
     BOARD_PROFILE_CAP_SPIB)

#define BOARD_PROFILE_LOW_ALTITUDE_W5500_VALIDATED \
    (BOARD_PROFILE_CAP_SPIC_W5500 | \
     BOARD_PROFILE_CAP_GPIO | \
     BOARD_PROFILE_CAP_CAN_B | \
     BOARD_PROFILE_CAP_RS422 | \
     BOARD_PROFILE_CAP_SPIB)

static const BoardProfile_HardwareDescriptor BoardProfile_HardwareDescriptors[] =
{
    {
        BOARD_PROFILE_ID_SYSTEM_MASTER,
        BOARD_PROFILE_ETHERNET_EMIF_W5300,
        BOARD_PROFILE_HARDWARE_REVISION_SYSTEM_MASTER_V01,
        1U,
        BOARD_PROFILE_CAP_EMIF_W5300,
        BOARD_PROFILE_SYSTEM_W5300_IMPLEMENTED,
        BOARD_PROFILE_SYSTEM_W5300_VALIDATED,
        "System master + EMIF W5300",
        {
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            133U, 135U, 136U,
            11U, 14U, 15U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            12U, 13U,
            144U, 143U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            2U, 2U, 2U, 2U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED
        }
    },
    {
        BOARD_PROFILE_ID_LOW_VOLTAGE_INVERTER,
        BOARD_PROFILE_ETHERNET_EMIF_W5300,
        BOARD_PROFILE_HARDWARE_REVISION_LOW_VOLTAGE_V03,
        3U,
        BOARD_PROFILE_CAP_EMIF_W5300,
        BOARD_PROFILE_CAP_EMIF_W5300 |
        BOARD_PROFILE_CAP_GPIO |
        BOARD_PROFILE_CAP_CAN_B |
        BOARD_PROFILE_CAP_SCIB,
        0UL,
        "Low voltage inverter V03 + EMIF W5300",
        {
            0U, 1U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            30U, 8U, 9U,
            32U, 10U, 11U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            6U, 7U,
            89U, 88U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            6U, 6U, 6U, 6U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED
        }
    },
    {
        BOARD_PROFILE_ID_LOW_ALTITUDE_UNIFIED,
        BOARD_PROFILE_ETHERNET_SPIC_W5500,
        BOARD_PROFILE_HARDWARE_REVISION_LOW_ALTITUDE_V01,
        1U,
        BOARD_PROFILE_CAP_SPIC_W5500,
        BOARD_PROFILE_LOW_ALTITUDE_W5500_IMPLEMENTED,
        BOARD_PROFILE_LOW_ALTITUDE_W5500_VALIDATED,
        "Low altitude unified V01 + SPIC W5500",
        {
            73U, 68U, 69U, 70U, 71U, 72U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            6U, 7U,
            47U, 46U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            6U, 6U,
            BOARD_PROFILE_PIN_UNUSED, BOARD_PROFILE_PIN_UNUSED,
            38U, 39U, 5U, 5U
        }
    }
};

#define BOARD_PROFILE_DESCRIPTOR_COUNT 3U
#define BOARD_PROFILE_HARDWARE_DESCRIPTOR_COUNT 3U

typedef struct
{
    BoardTest_U16 testId;
    BoardTest_U32 requiredAll;
    BoardTest_U32 requiredAny;
    BoardTest_U32 forbidden;
} BoardProfile_TestCapabilityRule;

static const BoardProfile_TestCapabilityRule BoardProfile_TestRules[] =
{
    {BOARD_TEST_ID_SYS_STARTUP, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_SYS_CLOCK, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_SYS_INTERRUPT, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_SYS_TIMER, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_SYS_WATCHDOG, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_RAM, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_FLASH, 0UL, 0UL, 0UL},
    {BOARD_TEST_ID_GPIO, BOARD_PROFILE_CAP_GPIO, 0UL, 0UL},
    {BOARD_TEST_ID_ADC_BASIC, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_PWM_BASIC, BOARD_PROFILE_CAP_PWM, 0UL, 0UL},
    {BOARD_TEST_ID_EMIF_BASIC, 0UL,
        BOARD_PROFILE_CAP_EMIF_W5300 |
        BOARD_PROFILE_CAP_EXTERNAL_SRAM |
        BOARD_PROFILE_CAP_FPGA_EMIF2, 0UL},
    {BOARD_TEST_ID_CAN_HOST, BOARD_PROFILE_CAP_CAN_B, 0UL, 0UL},
    {BOARD_TEST_ID_SCI_HOST, BOARD_PROFILE_CAP_SCIB, 0UL, 0UL},
    {BOARD_TEST_ID_SPIA_INTERNAL, BOARD_PROFILE_CAP_SPIA, 0UL, 0UL},
    {BOARD_TEST_ID_SPIB_INTERNAL, BOARD_PROFILE_CAP_SPIB, 0UL, 0UL},
    {BOARD_TEST_ID_SPIC_INTERNAL, BOARD_PROFILE_CAP_SPIC_EXTERNAL, 0UL,
        BOARD_PROFILE_CAP_SPIC_W5500},
    {BOARD_TEST_ID_SPI_EXTERNAL, BOARD_PROFILE_CAP_SPIC_EXTERNAL, 0UL,
        BOARD_PROFILE_CAP_SPIC_W5500},
    {BOARD_TEST_ID_I2C_RTC, BOARD_PROFILE_CAP_I2C_RTC, 0UL, 0UL},
    {BOARD_TEST_ID_EMIF_EXTERNAL, BOARD_PROFILE_CAP_EXTERNAL_SRAM, 0UL, 0UL},
    {BOARD_TEST_ID_ETHERNET, 0UL,
        BOARD_PROFILE_CAP_EMIF_W5300 | BOARD_PROFILE_CAP_SPIC_W5500, 0UL},
    {BOARD_TEST_ID_I2C_B_TMP116, BOARD_PROFILE_CAP_I2C_TMP116, 0UL, 0UL},
    {BOARD_TEST_ID_CAN_EXTERNAL, BOARD_PROFILE_CAP_CAN_B, 0UL, 0UL},
    {BOARD_TEST_ID_I2C_EXTERNAL, BOARD_PROFILE_CAP_I2C_EXTERNAL, 0UL, 0UL},
    {BOARD_TEST_ID_SCI_RS485_EXTERNAL, BOARD_PROFILE_CAP_SCIB, 0UL, 0UL},
    {BOARD_TEST_ID_RS422_EXTERNAL, BOARD_PROFILE_CAP_RS422, 0UL, 0UL},
    {BOARD_TEST_ID_SCIA_HANDHELD_EXTERNAL, BOARD_PROFILE_CAP_SCIA, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_ESTOP_DI3, 0UL,
        BOARD_PROFILE_CAP_DSP_DI | BOARD_PROFILE_CAP_FPGA_DIDO, 0UL},
    {BOARD_TEST_ID_HPD_INDICATOR_DO3, 0UL,
        BOARD_PROFILE_CAP_DSP_DO | BOARD_PROFILE_CAP_FPGA_DIDO, 0UL},
    {BOARD_TEST_ID_HPD_DRIVER_1, 0UL,
        BOARD_PROFILE_CAP_DSP_DO | BOARD_PROFILE_CAP_FPGA_DIDO, 0UL},
    {BOARD_TEST_ID_HPD_DRIVER_2, 0UL,
        BOARD_PROFILE_CAP_DSP_DO | BOARD_PROFILE_CAP_FPGA_DIDO, 0UL},
    {BOARD_TEST_ID_HPD_NTC_1, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_NTC_2, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_BUS_1, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_BUS_2, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_LINE_1, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_LINE_2, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_CURRENT_1, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_CURRENT_2, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_PHASE_1, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_HPD_PHASE_2, BOARD_PROFILE_CAP_ADC, 0UL, 0UL},
    {BOARD_TEST_ID_ETHERNET_SOCKET, BOARD_PROFILE_CAP_EMIF_W5300, 0UL, 0UL},
    {BOARD_TEST_ID_ETHERNET_TCP_LINK, BOARD_PROFILE_CAP_EMIF_W5300, 0UL, 0UL},
    {BOARD_TEST_ID_ETHERNET_TCP_ECHO, BOARD_PROFILE_CAP_EMIF_W5300, 0UL, 0UL},
    {BOARD_TEST_ID_ETHERNET_TCP_STABILITY, BOARD_PROFILE_CAP_EMIF_W5300, 0UL, 0UL},
    {BOARD_TEST_ID_FPGA_EMIF2_BASIC, BOARD_PROFILE_CAP_FPGA_EMIF2, 0UL, 0UL},
    {BOARD_TEST_ID_DIDO_FPGA_DO_EXTERNAL,
        BOARD_PROFILE_CAP_FPGA_EMIF2 | BOARD_PROFILE_CAP_FPGA_DIDO,
        0UL, 0UL},
    {BOARD_TEST_ID_HDO_FPGA_EXTERNAL,
        BOARD_PROFILE_CAP_FPGA_EMIF2 | BOARD_PROFILE_CAP_FPGA_DIDO,
        0UL, 0UL},
    {BOARD_TEST_ID_DI_FPGA_EXTERNAL,
        BOARD_PROFILE_CAP_FPGA_EMIF2 | BOARD_PROFILE_CAP_FPGA_DIDO,
        0UL, 0UL}
};

#define BOARD_PROFILE_TEST_RULE_COUNT \
    ((BoardTest_U16)(sizeof(BoardProfile_TestRules) / \
                     sizeof(BoardProfile_TestRules[0])))

volatile BoardProfile_RuntimeStatus gBoardProfileRuntimeStatus =
{
    BOARD_PROFILE_STATE_BOOT_SAFE,
    BOARD_PROFILE_ID_NONE,
    BOARD_PROFILE_HARDWARE_REVISION_NONE,
    BOARD_PROFILE_ETHERNET_NONE,
    BOARD_PROFILE_ETHERNET_NONE
};

const BoardProfile_Descriptor *BoardProfile_GetById(BoardTest_U16 boardId)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_DESCRIPTOR_COUNT; index++)
    {
        if(BoardProfile_Descriptors[index].id == boardId)
        {
            return &BoardProfile_Descriptors[index];
        }
    }

    return 0;
}

const BoardProfile_Descriptor *BoardProfile_GetCurrent(void)
{
    return BoardProfile_GetById(gBoardProfileRuntimeStatus.selectedId);
}

const BoardProfile_HardwareDescriptor *BoardProfile_GetHardwareDescriptor(
    BoardTest_U16 boardId,
    BoardTest_U16 hardwareRevision,
    BoardTest_U16 ethernetInterface)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_HARDWARE_DESCRIPTOR_COUNT; index++)
    {
        if((BoardProfile_HardwareDescriptors[index].boardId == boardId) &&
           (BoardProfile_HardwareDescriptors[index].hardwareRevision ==
            hardwareRevision) &&
           (BoardProfile_HardwareDescriptors[index].ethernetInterface ==
            ethernetInterface))
        {
            return &BoardProfile_HardwareDescriptors[index];
        }
    }

    return 0;
}

const BoardProfile_HardwareDescriptor *BoardProfile_GetCurrentHardware(void)
{
    BoardTest_U16 boardId;
    BoardTest_U16 hardwareRevision;
    BoardTest_U16 ethernetInterface;

    boardId = gBoardProfileRuntimeStatus.selectedId;
    if(boardId == BOARD_PROFILE_ID_NONE)
    {
        return 0;
    }

    hardwareRevision =
        gBoardProfileRuntimeStatus.selectedHardwareRevision;
    if(hardwareRevision == BOARD_PROFILE_HARDWARE_REVISION_NONE)
    {
        return 0;
    }

    ethernetInterface = gBoardProfileRuntimeStatus.selectedEthernetInterface;
    if(ethernetInterface == BOARD_PROFILE_ETHERNET_NONE)
    {
        ethernetInterface =
            gBoardProfileRuntimeStatus.detectedEthernetInterface;
    }
    return BoardProfile_GetHardwareDescriptor(
        boardId,
        hardwareRevision,
        ethernetInterface);
}

BoardTest_U32 BoardProfile_GetEffectiveCapabilities(void)
{
    const BoardProfile_Descriptor *profile;
    const BoardProfile_HardwareDescriptor *hardware;
    BoardTest_U32 capabilities;

    capabilities = 0UL;
    profile = BoardProfile_GetCurrent();
    hardware = BoardProfile_GetCurrentHardware();
    if(profile != 0)
    {
        capabilities |= profile->capabilities;
    }
    if(hardware != 0)
    {
        capabilities |= hardware->capabilities;
    }
    return capabilities;
}

BoardTest_U32 BoardProfile_GetImplementedCapabilities(void)
{
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    return (hardware != 0) ? hardware->implementedCapabilities : 0UL;
}

BoardTest_U32 BoardProfile_GetValidatedCapabilities(void)
{
    const BoardProfile_HardwareDescriptor *hardware;

    hardware = BoardProfile_GetCurrentHardware();
    return (hardware != 0) ? hardware->validatedCapabilities : 0UL;
}

BoardTest_U16 BoardProfile_IsCapabilityValidated(BoardTest_U32 capability)
{
    BoardTest_U32 validatedCapabilities;

    if(capability == 0UL)
    {
        return 0U;
    }

    validatedCapabilities = BoardProfile_GetValidatedCapabilities();
    return ((validatedCapabilities & capability) == capability) ? 1U : 0U;
}

void BoardProfile_Init(BoardTest_U16 detectedEthernetInterface)
{
    gBoardProfileRuntimeStatus.selectedId = BOARD_PROFILE_ID_NONE;
    gBoardProfileRuntimeStatus.selectedHardwareRevision =
        BOARD_PROFILE_HARDWARE_REVISION_NONE;
    gBoardProfileRuntimeStatus.selectedEthernetInterface =
        BOARD_PROFILE_ETHERNET_NONE;
    BoardProfile_SetDetectedEthernet(detectedEthernetInterface);
}

void BoardProfile_SetDetectedEthernet(
    BoardTest_U16 detectedEthernetInterface)
{
    if((detectedEthernetInterface != BOARD_PROFILE_ETHERNET_NONE) &&
       (detectedEthernetInterface != BOARD_PROFILE_ETHERNET_EMIF_W5300) &&
       (detectedEthernetInterface != BOARD_PROFILE_ETHERNET_SPIC_W5500))
    {
        detectedEthernetInterface = BOARD_PROFILE_ETHERNET_NONE;
    }

    if((gBoardProfileRuntimeStatus.selectedId != BOARD_PROFILE_ID_NONE) &&
       (gBoardProfileRuntimeStatus.selectedEthernetInterface !=
        BOARD_PROFILE_ETHERNET_NONE) &&
       (detectedEthernetInterface !=
        gBoardProfileRuntimeStatus.selectedEthernetInterface))
    {
        return;
    }

    gBoardProfileRuntimeStatus.detectedEthernetInterface =
        detectedEthernetInterface;
    if(gBoardProfileRuntimeStatus.selectedId == BOARD_PROFILE_ID_NONE)
    {
        gBoardProfileRuntimeStatus.state =
            (detectedEthernetInterface == BOARD_PROFILE_ETHERNET_NONE) ?
            BOARD_PROFILE_STATE_BOOT_SAFE :
            BOARD_PROFILE_STATE_NETWORK_READY;
    }
}

BoardTest_Result BoardProfile_Select(BoardTest_U16 boardId)
{
    const BoardProfile_Descriptor *profile;

    if(gBoardProfileRuntimeStatus.state != BOARD_PROFILE_STATE_NETWORK_READY)
    {
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    profile = BoardProfile_GetById(boardId);
    if(profile == 0)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    return BoardProfile_SelectVersion(
        boardId,
        profile->defaultHardwareRevision);
}

BoardTest_Result BoardProfile_SelectVersion(
    BoardTest_U16 boardId,
    BoardTest_U16 hardwareRevision)
{
    const BoardProfile_Descriptor *profile;
    const BoardProfile_HardwareDescriptor *hardware;

    if(gBoardProfileRuntimeStatus.state != BOARD_PROFILE_STATE_NETWORK_READY)
    {
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    profile = BoardProfile_GetById(boardId);
    if(profile == 0)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    hardware = BoardProfile_GetHardwareDescriptor(
        profile->id,
        hardwareRevision,
        gBoardProfileRuntimeStatus.detectedEthernetInterface);
    if(hardware == 0)
    {
        return BOARD_TEST_RESULT_NOT_SUPPORTED;
    }

    gBoardProfileRuntimeStatus.selectedId = profile->id;
    gBoardProfileRuntimeStatus.selectedHardwareRevision = hardwareRevision;
    gBoardProfileRuntimeStatus.selectedEthernetInterface =
        gBoardProfileRuntimeStatus.detectedEthernetInterface;
    gBoardProfileRuntimeStatus.state =
        BOARD_PROFILE_STATE_SELECTED_UNCONFIRMED;
    return BOARD_TEST_RESULT_PASS;
}

BoardTest_Result BoardProfile_Confirm(void)
{
    if((gBoardProfileRuntimeStatus.state !=
         BOARD_PROFILE_STATE_SELECTED_UNCONFIRMED) ||
       (BoardProfile_GetCurrent() == 0) ||
       (BoardProfile_GetCurrentHardware() == 0))
    {
        return BOARD_TEST_RESULT_SAFETY_LOCKED;
    }

    gBoardProfileRuntimeStatus.state = BOARD_PROFILE_STATE_CONFIRMED;
    return BOARD_TEST_RESULT_PASS;
}

void BoardProfile_Clear(void)
{
    gBoardProfileRuntimeStatus.selectedId = BOARD_PROFILE_ID_NONE;
    gBoardProfileRuntimeStatus.selectedHardwareRevision =
        BOARD_PROFILE_HARDWARE_REVISION_NONE;
    gBoardProfileRuntimeStatus.selectedEthernetInterface =
        BOARD_PROFILE_ETHERNET_NONE;
    gBoardProfileRuntimeStatus.state =
        (gBoardProfileRuntimeStatus.detectedEthernetInterface ==
         BOARD_PROFILE_ETHERNET_NONE) ?
        BOARD_PROFILE_STATE_BOOT_SAFE : BOARD_PROFILE_STATE_NETWORK_READY;
}

BoardTest_U16 BoardProfile_IsConfirmed(void)
{
    return gBoardProfileRuntimeStatus.state == BOARD_PROFILE_STATE_CONFIRMED;
}

static const BoardProfile_TestCapabilityRule *BoardProfile_GetTestRule(
    BoardTest_U16 testId)
{
    BoardTest_U16 index;

    for(index = 0U; index < BOARD_PROFILE_TEST_RULE_COUNT; index++)
    {
        if(BoardProfile_TestRules[index].testId == testId)
        {
            return &BoardProfile_TestRules[index];
        }
    }
    return 0;
}

static BoardTest_U16 BoardProfile_IsCoreIndependentTest(BoardTest_U16 testId)
{
    switch(testId)
    {
        case BOARD_TEST_ID_SYS_STARTUP:
        case BOARD_TEST_ID_SYS_CLOCK:
        case BOARD_TEST_ID_SYS_INTERRUPT:
        case BOARD_TEST_ID_SYS_TIMER:
        case BOARD_TEST_ID_SYS_WATCHDOG:
        case BOARD_TEST_ID_RAM:
        case BOARD_TEST_ID_FLASH:
            return 1U;

        default:
            return 0U;
    }
}

static BoardTest_U16 BoardProfile_AreCapabilitiesSatisfied(
    const BoardProfile_TestCapabilityRule *rule,
    BoardTest_U32 capabilities)
{
    if((capabilities & rule->requiredAll) != rule->requiredAll)
    {
        return 0U;
    }
    if((rule->requiredAny != 0UL) &&
       ((capabilities & rule->requiredAny) == 0UL))
    {
        return 0U;
    }
    if((capabilities & rule->forbidden) != 0UL)
    {
        return 0U;
    }
    return 1U;
}

static BoardTest_U16 BoardProfile_IsImplementedCapabilityTest(
    const BoardProfile_TestCapabilityRule *rule,
    const BoardProfile_HardwareDescriptor *hardware)
{
    if((rule->requiredAll == 0UL) && (rule->requiredAny == 0UL))
    {
        return 0U;
    }

    return BoardProfile_AreCapabilitiesSatisfied(
        rule,
        hardware->implementedCapabilities);
}

BoardTest_U16 BoardProfile_IsTestSupported(BoardTest_U16 testId)
{
    const BoardProfile_Descriptor *profile;
    const BoardProfile_HardwareDescriptor *hardware;
    const BoardProfile_TestCapabilityRule *rule;
    BoardTest_U32 capabilities;

    profile = BoardProfile_GetCurrent();
    hardware = BoardProfile_GetCurrentHardware();
    if((profile == 0) || (hardware == 0))
    {
        return 0U;
    }

    rule = BoardProfile_GetTestRule(testId);
    if(rule == 0)
    {
        return 0U;
    }

    capabilities = BoardProfile_GetEffectiveCapabilities();
    if(BoardProfile_AreCapabilitiesSatisfied(rule, capabilities) == 0U)
    {
        return 0U;
    }

    if(BoardProfile_IsCoreIndependentTest(testId) != 0U)
    {
        return 1U;
    }

    if(BoardProfile_IsImplementedCapabilityTest(rule, hardware) != 0U)
    {
        return 1U;
    }

    return 0U;
}
