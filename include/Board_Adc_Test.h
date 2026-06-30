#ifndef BOARD_ADC_TEST_H
#define BOARD_ADC_TEST_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_ADC_RESOLUTION_BITS        12U
#define BOARD_ADC_RAW_MAX                4095U
#define BOARD_ADC_REFERENCE_VOLTAGE_V    3.3F

#define BOARD_ADC_BASIC_CLOCK_ENABLED    0x0001U
#define BOARD_ADC_BASIC_POWERED          0x0002U
#define BOARD_ADC_BASIC_SOC_CONFIGURED   0x0004U
#define BOARD_ADC_BASIC_CONVERSION_DONE  0x0008U
#define BOARD_ADC_BASIC_RAW_IN_RANGE     0x0010U

#define BOARD_ADC_BASIC_STATUS_REQUIRED_MASK \
    (BOARD_ADC_BASIC_CLOCK_ENABLED |         \
     BOARD_ADC_BASIC_POWERED |               \
     BOARD_ADC_BASIC_SOC_CONFIGURED |        \
     BOARD_ADC_BASIC_CONVERSION_DONE |       \
     BOARD_ADC_BASIC_RAW_IN_RANGE)

#define BOARD_ADC_BASIC_STATUS_DIAGNOSTIC_MASK \
    BOARD_ADC_BASIC_STATUS_REQUIRED_MASK

#define BOARD_ADC_BASIC_ADCA_CHANNEL      0U

typedef struct
{
    volatile BoardTest_U16 statusMask;
    volatile BoardTest_U16 rawValue;
    volatile BoardTest_U16 channel;
    volatile float inputVoltage;
} BoardAdc_BasicSnapshot;

extern volatile BoardAdc_BasicSnapshot gBoardAdcBasicSnapshot;

BoardTest_U16 BoardAdc_VoltageToRaw(float inputVoltage);
float BoardAdc_RawToVoltage(BoardTest_U16 rawValue);
BoardTest_Result BoardAdc_EvaluateBasicSample(BoardTest_U16 statusMask,
                                              BoardTest_U16 rawValue,
                                              BoardTest_Record *record);

#ifndef BOARD_TEST_HOST
BoardTest_Result BoardAdc_RunBasicSampleTest(BoardTest_Record *record);
#endif

#ifdef __cplusplus
}
#endif

#endif
