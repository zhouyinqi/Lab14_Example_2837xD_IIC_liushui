#ifndef BOARD_HOST_PROTOCOL_H
#define BOARD_HOST_PROTOCOL_H

#include "Board_Test.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOARD_HOST_PROTOCOL_VERSION              0x01U

#define BOARD_HOST_PROTOCOL_REQUEST_SIZE         12U
#define BOARD_HOST_PROTOCOL_RESPONSE_SIZE        40U

#define BOARD_HOST_PROTOCOL_REQUEST_MAGIC_0      0x42U
#define BOARD_HOST_PROTOCOL_REQUEST_MAGIC_1      0x54U
#define BOARD_HOST_PROTOCOL_RESPONSE_MAGIC_0     0x42U
#define BOARD_HOST_PROTOCOL_RESPONSE_MAGIC_1     0x52U

#define BOARD_HOST_PROTOCOL_COMMAND_PING         0x01U
#define BOARD_HOST_PROTOCOL_COMMAND_START_BOARD  0x10U
#define BOARD_HOST_PROTOCOL_COMMAND_START_EXTERNAL 0x11U
#define BOARD_HOST_PROTOCOL_COMMAND_START_HPD    0x12U
#define BOARD_HOST_PROTOCOL_COMMAND_START_SINGLE 0x13U
#define BOARD_HOST_PROTOCOL_COMMAND_STOP         0x14U
#define BOARD_HOST_PROTOCOL_COMMAND_SELECT_BOARD_PROFILE 0x15U
#define BOARD_HOST_PROTOCOL_COMMAND_CONFIRM_BOARD_PROFILE 0x16U
#define BOARD_HOST_PROTOCOL_COMMAND_CLEAR_BOARD_PROFILE 0x17U
#define BOARD_HOST_PROTOCOL_COMMAND_GET_STATUS   0x20U
#define BOARD_HOST_PROTOCOL_COMMAND_GET_RECORD   0x21U
#define BOARD_HOST_PROTOCOL_COMMAND_GET_BOARD_INFO 0x22U
#define BOARD_HOST_PROTOCOL_COMMAND_GET_TEST_AVAILABILITY 0x23U
#define BOARD_HOST_PROTOCOL_COMMAND_CONFIGURE_ADC_INJECTION 0x24U

/*
 * For a GET_STATUS response with no record, the existing result fields carry
 * communication standby masks without changing the 40-byte frame length:
 * rawValue[31:16]=enabled, rawValue[15:0]=waiting, measuredValue=received,
 * expectedMin=replied, expectedMax=failed.
 */

/*
 * A GET_BOARD_INFO response keeps recordId=0xFFFF and reuses the result
 * payload: rawValue=selected board plus detected hardware capability mask,
 * measuredValue=selected profile ID (zero before selection),
 * expectedMin=detected Ethernet interface, expectedMax=profile state,
 * recordResult=selected hardware revision,
 * recordError=detected hardware pin map revision.
 */

/*
 * A GET_TEST_AVAILABILITY response uses recordId for the requested Test ID,
 * recordResult for BoardProfile_TestAvailability, recordError for the test
 * descriptor capability flags, and rawValue[31:16]/[15:0] for stage/risk.
 */

#define BOARD_HOST_PROTOCOL_FLAG_OUTPUT_ARMED      0x01U
#define BOARD_HOST_PROTOCOL_FLAG_PWM_CHANNEL_SHIFT 1U
#define BOARD_HOST_PROTOCOL_FLAG_PWM_CHANNEL_MASK  0x1EU
#define BOARD_HOST_PROTOCOL_FLAG_SINGLE_SELECTION_SHIFT 1U
#define BOARD_HOST_PROTOCOL_FLAG_SINGLE_SELECTION_MASK  0x1EU
#define BOARD_HOST_PROTOCOL_ADC_TOLERANCE_STEP_MV 10U

#define BOARD_HOST_PROTOCOL_STATUS_OK            0x0000U
#define BOARD_HOST_PROTOCOL_STATUS_FRAME          0x0001U
#define BOARD_HOST_PROTOCOL_STATUS_VERSION        0x0002U
#define BOARD_HOST_PROTOCOL_STATUS_COMMAND        0x0003U
#define BOARD_HOST_PROTOCOL_STATUS_BUSY           0x0004U
#define BOARD_HOST_PROTOCOL_STATUS_ARGUMENT       0x0005U
#define BOARD_HOST_PROTOCOL_STATUS_TEST_ID        0x0006U
#define BOARD_HOST_PROTOCOL_STATUS_PROFILE        0x0007U

void BoardHostProtocol_HandleRequest(
    const BoardTest_U16 *request,
    BoardTest_U16 requestLength,
    BoardTest_U16 *response,
    BoardTest_U16 *responseLength);

#ifdef __cplusplus
}
#endif

#endif
