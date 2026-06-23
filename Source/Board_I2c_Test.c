#include "Board_I2c_Test.h"

#ifndef BOARD_TEST_HOST
#include "F28x_Project.h"
#include "Board_Pinmap.h"

#define BOARD_I2C_TIMEOUT              200000UL
#define BOARD_I2C_TIMEOUT_ERROR        0x2000U
#define BOARD_I2C_MDR_TX_START_STOP    0x6E20U
#define BOARD_I2C_MDR_TX_START         0x6620U
#define BOARD_I2C_MDR_RX_START_STOP    0x6C20U

volatile BoardI2c_Tmp116Snapshot gBoardI2cTmp116Snapshot =
{
    0U,
    0U,
    0U,
    BOARD_I2C_TIMEOUT_ERROR
};
#endif

static float BoardI2c_Tmp116RawToC(BoardTest_U16 raw)
{
    long signedValue;

    if((raw & 0x8000U) != 0U)
    {
        signedValue = (long)raw - 65536L;
    }
    else
    {
        signedValue = (long)raw;
    }

    return (float)signedValue * BOARD_I2C_TMP116_LSB_C;
}

BoardTest_Result BoardI2c_EvaluateTmp116Read(BoardTest_U16 status,
                                              BoardTest_U16 address,
                                              BoardTest_U16 deviceId,
                                              BoardTest_U16 temperatureRaw,
                                              BoardTest_Record *record)
{
    record->rawValue = (((BoardTest_U32)address & 0x00FFUL) << 24U) |
                       (((BoardTest_U32)deviceId & 0x0000FFFFUL) << 8U) |
                       ((BoardTest_U32)status & 0x000000FFUL);
    record->measuredValue = BoardI2c_Tmp116RawToC(temperatureRaw);
    record->expectedMin = -55.0F;
    record->expectedMax = 125.0F;

    if((status == BOARD_I2C_STATUS_SUCCESS) &&
       (deviceId == BOARD_I2C_TMP116_DEVICE_ID) &&
       (address >= BOARD_I2C_TMP116_ADDR_MIN) &&
       (address <= BOARD_I2C_TMP116_ADDR_MAX))
    {
        record->errorCode = BOARD_TEST_ERROR_NONE;
        return BOARD_TEST_RESULT_PASS;
    }

    record->errorCode = BOARD_TEST_ERROR_I2C_B_TMP116;
    return BOARD_TEST_RESULT_FAIL;
}

#ifndef BOARD_TEST_HOST
static void BoardI2cB_Init(void)
{
    I2cBGpioConfig(I2C_B_GPIO2_GPIO3);

    GPIO_SetupPinMux(BOARD_PIN_I2CB_INT, GPIO_MUX_CPU1, 0U);
    GPIO_SetupPinOptions(BOARD_PIN_I2CB_INT, GPIO_INPUT, GPIO_PULLUP | GPIO_ASYNC);

    I2cbRegs.I2CMDR.all = 0x0000U;
    I2cbRegs.I2CPSC.all = 19U;
    I2cbRegs.I2CCLKL = 45U;
    I2cbRegs.I2CCLKH = 45U;
    I2cbRegs.I2CIER.all = 0x0000U;
    I2cbRegs.I2CMDR.all = 0x0020U;
    I2cbRegs.I2CFFTX.all = 0x6000U;
    I2cbRegs.I2CFFRX.all = 0x2040U;
}

static BoardTest_U16 BoardI2c_WaitStopReady(volatile struct I2C_REGS *regs)
{
    BoardTest_U32 timeout = BOARD_I2C_TIMEOUT;

    while(regs->I2CMDR.bit.STP == 1U)
    {
        if(timeout-- == 0UL)
        {
            return BOARD_I2C_TIMEOUT_ERROR;
        }
    }

    return BOARD_I2C_STATUS_SUCCESS;
}

static BoardTest_U16 BoardI2c_CheckError(volatile struct I2C_REGS *regs)
{
    if(regs->I2CSTR.bit.ARBL == 1U)
    {
        regs->I2CSTR.all = I2C_CLR_AL_BIT;
        return I2C_ARB_LOST_ERROR;
    }

    if(regs->I2CSTR.bit.NACK == 1U)
    {
        regs->I2CMDR.bit.STP = 1U;
        regs->I2CSTR.all = I2C_CLR_NACK_BIT;
        return I2C_NACK_ERROR;
    }

    return BOARD_I2C_STATUS_SUCCESS;
}

static BoardTest_U16 BoardI2c_WaitArdy(volatile struct I2C_REGS *regs)
{
    BoardTest_U16 status;
    BoardTest_U32 timeout = BOARD_I2C_TIMEOUT;

    while(regs->I2CSTR.bit.ARDY == 0U)
    {
        status = BoardI2c_CheckError(regs);
        if(status != BOARD_I2C_STATUS_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return BOARD_I2C_TIMEOUT_ERROR;
        }
    }

    regs->I2CSTR.all = I2C_CLR_ARDY_BIT;
    return BOARD_I2C_STATUS_SUCCESS;
}

static BoardTest_U16 BoardI2c_WaitRxData(volatile struct I2C_REGS *regs)
{
    BoardTest_U16 status;
    BoardTest_U32 timeout = BOARD_I2C_TIMEOUT;

    while(regs->I2CFFRX.bit.RXFFST == 0U)
    {
        status = BoardI2c_CheckError(regs);
        if(status != BOARD_I2C_STATUS_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return BOARD_I2C_TIMEOUT_ERROR;
        }
    }

    return BOARD_I2C_STATUS_SUCCESS;
}

static BoardTest_U16 BoardI2c_WaitStopDetected(volatile struct I2C_REGS *regs)
{
    BoardTest_U16 status;
    BoardTest_U32 timeout = BOARD_I2C_TIMEOUT;

    while(regs->I2CSTR.bit.SCD == 0U)
    {
        status = BoardI2c_CheckError(regs);
        if(status != BOARD_I2C_STATUS_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return BOARD_I2C_TIMEOUT_ERROR;
        }
    }

    regs->I2CSTR.all = I2C_CLR_SCD_BIT;
    return BOARD_I2C_STATUS_SUCCESS;
}

static BoardTest_U16 BoardI2c_ReadRegister(volatile struct I2C_REGS *regs,
                                           BoardTest_U16 i2cAddr,
                                           BoardTest_U16 regAddr,
                                           BoardTest_U16 *data,
                                           BoardTest_U16 len)
{
    BoardTest_U16 index;
    BoardTest_U16 status;
    BoardTest_U16 dummy = 0U;

    status = BoardI2c_WaitStopReady(regs);
    if(status != BOARD_I2C_STATUS_SUCCESS)
    {
        return status;
    }

    while(regs->I2CFFRX.bit.RXFFST != 0U)
    {
        dummy = regs->I2CDRR;
    }
    (void)dummy;

    regs->I2CSAR = i2cAddr;
    if(regs->I2CSTR.bit.BB == 1U)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    regs->I2CSTR.all = I2C_CLR_AL_BIT |
                       I2C_CLR_NACK_BIT |
                       I2C_CLR_ARDY_BIT |
                       I2C_CLR_SCD_BIT;
    regs->I2CCNT = 1U;
    regs->I2CDXR = regAddr & 0x00FFU;
    regs->I2CMDR.all = BOARD_I2C_MDR_TX_START;

    status = BoardI2c_WaitArdy(regs);
    if(status != BOARD_I2C_STATUS_SUCCESS)
    {
        return status;
    }

    regs->I2CCNT = len;
    regs->I2CMDR.all = BOARD_I2C_MDR_RX_START_STOP;

    for(index = 0U; index < len; index++)
    {
        status = BoardI2c_WaitRxData(regs);
        if(status != BOARD_I2C_STATUS_SUCCESS)
        {
            return status;
        }
        data[index] = regs->I2CDRR & 0x00FFU;
    }

    return BoardI2c_WaitStopDetected(regs);
}

static BoardTest_U16 BoardI2c_ReadRegister16(volatile struct I2C_REGS *regs,
                                             BoardTest_U16 i2cAddr,
                                             BoardTest_U16 regAddr,
                                             BoardTest_U16 *value)
{
    BoardTest_U16 data[2];
    BoardTest_U16 status;

    status = BoardI2c_ReadRegister(regs, i2cAddr, regAddr, data, 2U);
    if(status != BOARD_I2C_STATUS_SUCCESS)
    {
        return status;
    }

    *value = ((data[0] & 0x00FFU) << 8U) | (data[1] & 0x00FFU);
    return BOARD_I2C_STATUS_SUCCESS;
}

BoardTest_Result BoardI2c_RunTmp116Test(BoardTest_Record *record)
{
    BoardTest_U16 address;
    BoardTest_U16 status;
    BoardTest_U16 deviceId = 0U;
    BoardTest_U16 temperatureRaw = 0U;
    BoardTest_U16 lastStatus = BOARD_I2C_TIMEOUT_ERROR;

    BoardI2cB_Init();

    for(address = BOARD_I2C_TMP116_ADDR_MIN;
        address <= BOARD_I2C_TMP116_ADDR_MAX;
        address++)
    {
        status = BoardI2c_ReadRegister16(&I2cbRegs,
                                         address,
                                         BOARD_I2C_TMP116_REG_DEVICE_ID,
                                         &deviceId);
        lastStatus = status;
        if((status == BOARD_I2C_STATUS_SUCCESS) &&
           (deviceId == BOARD_I2C_TMP116_DEVICE_ID))
        {
            status = BoardI2c_ReadRegister16(&I2cbRegs,
                                             address,
                                             BOARD_I2C_TMP116_REG_TEMP,
                                             &temperatureRaw);
            lastStatus = status;
            break;
        }
    }

    if(address > BOARD_I2C_TMP116_ADDR_MAX)
    {
        address = 0U;
        deviceId = 0U;
        temperatureRaw = 0U;
    }

    gBoardI2cTmp116Snapshot.address = address;
    gBoardI2cTmp116Snapshot.deviceId = deviceId;
    gBoardI2cTmp116Snapshot.temperatureRaw = temperatureRaw;
    gBoardI2cTmp116Snapshot.status = lastStatus;

    return BoardI2c_EvaluateTmp116Read(lastStatus,
                                       address,
                                       deviceId,
                                       temperatureRaw,
                                       record);
}
#endif
