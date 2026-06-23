/*
 * User_Iic_Led.c
 *
 * I2C-A RTC debug support for the PCA/PCF85063-style RTC on GPIO42/43.
 */

#include "F2837xD_Device.h"
#include "F2837xD_Examples.h"
#include "Board_Pinmap.h"

#define RTC_I2C_ADDR            0x51U
#define RTC_REG_CONTROL1        0x00U
#define RTC_REG_SECONDS         0x04U
#define RTC_TIME_BYTES          7U
#define RTC_INT_GPIO            BOARD_PIN_I2CA_INT

#define I2C_TIMEOUT_ERROR       0x2000U
#define I2C_TIMEOUT             200000UL
#define I2C_MDR_TX_START_STOP   0x6E20U
#define I2C_MDR_TX_START        0x6620U
#define I2C_MDR_RX_START_STOP   0x6C20U

struct RTC_TIME {
    Uint16 seconds;
    Uint16 minutes;
    Uint16 hours;
    Uint16 day;
    Uint16 month;
    Uint16 weekday;
    Uint16 year;
    Uint16 oscillatorStopped;
};

volatile struct RTC_TIME RtcNow;
volatile struct RTC_TIME RtcSetValue = {0U, 0U, 12U, 12U, 6U, 5U, 26U, 0U};
volatile Uint16 RtcRaw[RTC_TIME_BYTES];
volatile Uint16 RtcStatus = I2C_ERROR;
volatile Uint16 RtcSetRequest = 0U;

static void I2CA_Init(void);
static void InitRtcIntGpio(void);
static Uint16 I2cWriteBytes(Uint16 i2cAddr, Uint16 reg, const Uint16 *data, Uint16 len);
static Uint16 I2cReadBytes(Uint16 i2cAddr, Uint16 reg, Uint16 *data, Uint16 len);
static Uint16 I2cWaitStopReady(void);
static Uint16 I2cWaitArdy(void);
static Uint16 I2cWaitRxData(void);
static Uint16 I2cWaitStopDetected(void);
static Uint16 I2cCheckError(void);
static Uint16 BcdToDec(Uint16 bcd);
static Uint16 DecToBcd(Uint16 dec);
static Uint16 RtcInit(void);
static Uint16 RtcReadTime(struct RTC_TIME *time);
static Uint16 RtcSetTime(const struct RTC_TIME *time);

void Led_Test(void);

static void I2CA_Init(void)
{
    I2caRegs.I2CMDR.all = 0x0000;

    I2caRegs.I2CPSC.all = 19;      /* 200 MHz SYSCLK -> 10 MHz I2C module clock. */
    I2caRegs.I2CCLKL = 45;         /* About 100 kHz SCL for RTC debug. */
    I2caRegs.I2CCLKH = 45;
    I2caRegs.I2CIER.all = 0x0000;  /* Polling mode. */

    I2caRegs.I2CMDR.all = 0x0020;
    I2caRegs.I2CFFTX.all = 0x6000;
    I2caRegs.I2CFFRX.all = 0x2040;
}

static void InitRtcIntGpio(void)
{
    GPIO_SetupPinMux(RTC_INT_GPIO, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(RTC_INT_GPIO, GPIO_INPUT, GPIO_PULLUP | GPIO_ASYNC);
}

static Uint16 I2cWaitStopReady(void)
{
    Uint32 timeout = I2C_TIMEOUT;

    while(I2caRegs.I2CMDR.bit.STP == 1U)
    {
        if(timeout-- == 0UL)
        {
            return I2C_TIMEOUT_ERROR;
        }
    }

    return I2C_SUCCESS;
}

static Uint16 I2cCheckError(void)
{
    if(I2caRegs.I2CSTR.bit.ARBL == 1U)
    {
        I2caRegs.I2CSTR.all = I2C_CLR_AL_BIT;
        return I2C_ARB_LOST_ERROR;
    }

    if(I2caRegs.I2CSTR.bit.NACK == 1U)
    {
        I2caRegs.I2CMDR.bit.STP = 1U;
        I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
        return I2C_NACK_ERROR;
    }

    return I2C_SUCCESS;
}

static Uint16 I2cWaitArdy(void)
{
    Uint16 status;
    Uint32 timeout = I2C_TIMEOUT;

    while(I2caRegs.I2CSTR.bit.ARDY == 0U)
    {
        status = I2cCheckError();
        if(status != I2C_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return I2C_TIMEOUT_ERROR;
        }
    }

    I2caRegs.I2CSTR.all = I2C_CLR_ARDY_BIT;
    return I2C_SUCCESS;
}

static Uint16 I2cWaitRxData(void)
{
    Uint16 status;
    Uint32 timeout = I2C_TIMEOUT;

    while(I2caRegs.I2CFFRX.bit.RXFFST == 0U)
    {
        status = I2cCheckError();
        if(status != I2C_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return I2C_TIMEOUT_ERROR;
        }
    }

    return I2C_SUCCESS;
}

static Uint16 I2cWaitStopDetected(void)
{
    Uint16 status;
    Uint32 timeout = I2C_TIMEOUT;

    while(I2caRegs.I2CSTR.bit.SCD == 0U)
    {
        status = I2cCheckError();
        if(status != I2C_SUCCESS)
        {
            return status;
        }
        if(timeout-- == 0UL)
        {
            return I2C_TIMEOUT_ERROR;
        }
    }

    I2caRegs.I2CSTR.all = I2C_CLR_SCD_BIT;
    return I2C_SUCCESS;
}

static Uint16 I2cWriteBytes(Uint16 i2cAddr, Uint16 reg, const Uint16 *data, Uint16 len)
{
    Uint16 i;
    Uint16 status;

    if(len > 15U)
    {
        return I2C_ERROR;
    }

    status = I2cWaitStopReady();
    if(status != I2C_SUCCESS)
    {
        return status;
    }

    I2caRegs.I2CSAR = i2cAddr;
    if(I2caRegs.I2CSTR.bit.BB == 1U)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CSTR.all = I2C_CLR_AL_BIT | I2C_CLR_NACK_BIT | I2C_CLR_ARDY_BIT | I2C_CLR_SCD_BIT;
    I2caRegs.I2CCNT = len + 1U;
    I2caRegs.I2CDXR = reg & 0x00FFU;

    for(i = 0U; i < len; i++)
    {
        I2caRegs.I2CDXR = data[i] & 0x00FFU;
    }

    I2caRegs.I2CMDR.all = I2C_MDR_TX_START_STOP;
    return I2cWaitStopDetected();
}

static Uint16 I2cReadBytes(Uint16 i2cAddr, Uint16 reg, Uint16 *data, Uint16 len)
{
    Uint16 i;
    Uint16 status;
    Uint16 dummy;

    if(len == 0U)
    {
        return I2C_SUCCESS;
    }

    status = I2cWaitStopReady();
    if(status != I2C_SUCCESS)
    {
        return status;
    }

    while(I2caRegs.I2CFFRX.bit.RXFFST != 0U)
    {
        dummy = I2caRegs.I2CDRR;
    }
    (void)dummy;

    I2caRegs.I2CSAR = i2cAddr;
    if(I2caRegs.I2CSTR.bit.BB == 1U)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CSTR.all = I2C_CLR_AL_BIT | I2C_CLR_NACK_BIT | I2C_CLR_ARDY_BIT | I2C_CLR_SCD_BIT;
    I2caRegs.I2CCNT = 1U;
    I2caRegs.I2CDXR = reg & 0x00FFU;
    I2caRegs.I2CMDR.all = I2C_MDR_TX_START;

    status = I2cWaitArdy();
    if(status != I2C_SUCCESS)
    {
        return status;
    }

    I2caRegs.I2CCNT = len;
    I2caRegs.I2CMDR.all = I2C_MDR_RX_START_STOP;

    for(i = 0U; i < len; i++)
    {
        status = I2cWaitRxData();
        if(status != I2C_SUCCESS)
        {
            return status;
        }
        data[i] = I2caRegs.I2CDRR & 0x00FFU;
    }

    return I2cWaitStopDetected();
}

static Uint16 BcdToDec(Uint16 bcd)
{
    return (((bcd >> 4U) & 0x0FU) * 10U) + (bcd & 0x0FU);
}

static Uint16 DecToBcd(Uint16 dec)
{
    return (((dec / 10U) << 4U) | (dec % 10U));
}

static Uint16 RtcInit(void)
{
    Uint16 data[2];

    data[0] = 0x00U;
    data[1] = 0x00U;

    return I2cWriteBytes(RTC_I2C_ADDR, RTC_REG_CONTROL1, data, 2U);
}

static Uint16 RtcReadTime(struct RTC_TIME *time)
{
    Uint16 raw[RTC_TIME_BYTES];
    Uint16 i;
    Uint16 status;

    status = I2cReadBytes(RTC_I2C_ADDR, RTC_REG_SECONDS, raw, RTC_TIME_BYTES);
    if(status != I2C_SUCCESS)
    {
        return status;
    }

    for(i = 0U; i < RTC_TIME_BYTES; i++)
    {
        RtcRaw[i] = raw[i];
    }

    time->oscillatorStopped = (raw[0] & 0x80U) ? 1U : 0U;
    time->seconds = BcdToDec(raw[0] & 0x7FU);
    time->minutes = BcdToDec(raw[1] & 0x7FU);
    time->hours = BcdToDec(raw[2] & 0x3FU);
    time->day = BcdToDec(raw[3] & 0x3FU);
    time->weekday = raw[4] & 0x07U;
    time->month = BcdToDec(raw[5] & 0x1FU);
    time->year = BcdToDec(raw[6]);

    return I2C_SUCCESS;
}

static Uint16 RtcSetTime(const struct RTC_TIME *time)
{
    Uint16 data[RTC_TIME_BYTES];

    data[0] = DecToBcd(time->seconds) & 0x7FU;
    data[1] = DecToBcd(time->minutes) & 0x7FU;
    data[2] = DecToBcd(time->hours) & 0x3FU;
    data[3] = DecToBcd(time->day) & 0x3FU;
    data[4] = time->weekday & 0x07U;
    data[5] = DecToBcd(time->month) & 0x1FU;
    data[6] = DecToBcd(time->year);

    return I2cWriteBytes(RTC_I2C_ADDR, RTC_REG_SECONDS, data, RTC_TIME_BYTES);
}

void Led_Test(void)
{
    static Uint16 initialized = 0U;

    if(initialized == 0U)
    {
        I2cAGpioConfig(I2C_A_GPIO42_GPIO43);
        InitRtcIntGpio();
        I2CA_Init();
        RtcStatus = RtcInit();
        initialized = 1U;
    }

    if(RtcSetRequest != 0U)
    {
        RtcStatus = RtcSetTime((const struct RTC_TIME *)&RtcSetValue);
        RtcSetRequest = 0U;
    }
    else
    {
        RtcStatus = RtcReadTime((struct RTC_TIME *)&RtcNow);
    }

    DELAY_US(500000);
}

/* End of file. */
