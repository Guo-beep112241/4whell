#ifndef __APP_PROTOCOL_H__
#define __APP_PROTOCOL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Protocol constants — must match C board uart_comm.h */
#define APP_PROTOCOL_SOF              0xA5U
#define APP_PROTOCOL_MIN_FRAME_LEN    5U     /* SOF(1) + Len(1) + Cmd(1) + CRC(2) */
#define APP_PROTOCOL_MAX_FRAME_LEN    255U
#define APP_PROTOCOL_CRC16_LEN        2U
#define APP_PROTOCOL_FRAME_OVERHEAD   3U

/* C → F401 commands */
#define APP_PROTOCOL_CMD_SET_WHEEL_SPEEDS  0x01U
#define APP_PROTOCOL_CMD_EMERGENCY_STOP    0x02U
#define APP_PROTOCOL_CMD_HEARTBEAT         0x03U

/* F401 → C commands */
#define APP_PROTOCOL_CMD_WHEEL_FEEDBACK    0x81U
#define APP_PROTOCOL_CMD_MOTOR_STATUS      0x82U
#define APP_PROTOCOL_CMD_HEARTBEAT_ACK     0x83U

/* ========================================================================
 * Public API
 * ======================================================================== */

void App_Protocol_Init(void);
uint16_t App_Protocol_CalcCRC16(const uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* __APP_PROTOCOL_H__ */
