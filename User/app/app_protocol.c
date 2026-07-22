#include "app_protocol.h"

#include <string.h>
#include "usart.h"
#include "net_uart.h"
#include "app_chasis.h"
#include "bsp_timer.h"
#include "drv_motor.h"

/* ========================================================================
 * CRC-16/XMODEM lookup table (poly 0x1021) — same as C board uart_comm.c
 * ======================================================================== */

static const uint16_t s_crc16_table[256] = {
    0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50A5U, 0x60C6U, 0x70E7U,
    0x8108U, 0x9129U, 0xA14AU, 0xB16BU, 0xC18CU, 0xD1ADU, 0xE1CEU, 0xF1EFU,
    0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52B5U, 0x4294U, 0x72F7U, 0x62D6U,
    0x9339U, 0x8318U, 0xB37BU, 0xA35AU, 0xD3BDU, 0xC39CU, 0xF3FFU, 0xE3DEU,
    0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64E6U, 0x74C7U, 0x44A4U, 0x5485U,
    0xA56AU, 0xB54BU, 0x8528U, 0x9509U, 0xE5EEU, 0xF5CFU, 0xC5ACU, 0xD58DU,
    0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76D7U, 0x66F6U, 0x5695U, 0x46B4U,
    0xB75BU, 0xA77AU, 0x9719U, 0x8738U, 0xF7DFU, 0xE7FEU, 0xD79DU, 0xC7BCU,
    0x48C4U, 0x58E5U, 0x6886U, 0x78A7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
    0xC9CCU, 0xD9EDU, 0xE98EU, 0xF9AFU, 0x8948U, 0x9969U, 0xA90AU, 0xB92BU,
    0x5AF5U, 0x4AD4U, 0x7AB7U, 0x6A96U, 0x1A71U, 0x0A50U, 0x3A33U, 0x2A12U,
    0xDBFDU, 0xCBDCU, 0xFBBFU, 0xEB9EU, 0x9B79U, 0x8B58U, 0xBB3BU, 0xAB1AU,
    0x6CA6U, 0x7C87U, 0x4CE4U, 0x5CC5U, 0x2C22U, 0x3C03U, 0x0C60U, 0x1C41U,
    0xEDAEU, 0xFD8FU, 0xCDECU, 0xDDCDU, 0xAD2AU, 0xBD0BU, 0x8D68U, 0x9D49U,
    0x7E97U, 0x6EB6U, 0x5ED5U, 0x4EF4U, 0x3E13U, 0x2E32U, 0x1E51U, 0x0E70U,
    0xFF9FU, 0xEFBEU, 0xDFDDU, 0xCFFCU, 0xBF1BU, 0xAF3AU, 0x9F59U, 0x8F78U,
    0x9188U, 0x81A9U, 0xB1CAU, 0xA1EBU, 0xD10CU, 0xC12DU, 0xF14EU, 0xE16FU,
    0x1080U, 0x00A1U, 0x30C2U, 0x20E3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
    0x83B9U, 0x9398U, 0xA3FBU, 0xB3DAU, 0xC33DU, 0xD31CU, 0xE37FU, 0xF35EU,
    0x02B1U, 0x1290U, 0x22F3U, 0x32D2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
    0xB5EAU, 0xA5CBU, 0x95A8U, 0x8589U, 0xF56EU, 0xE54FU, 0xD52CU, 0xC50DU,
    0x34E2U, 0x24C3U, 0x14A0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
    0xA7DBU, 0xB7FAU, 0x8799U, 0x97B8U, 0xE75FU, 0xF77EU, 0xC71DU, 0xD73CU,
    0x26D3U, 0x36F2U, 0x0691U, 0x16B0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
    0xD94CU, 0xC96DU, 0xF90EU, 0xE92FU, 0x99C8U, 0x89E9U, 0xB98AU, 0xA9ABU,
    0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18C0U, 0x08E1U, 0x3882U, 0x28A3U,
    0xCB7DU, 0xDB5CU, 0xEB3FU, 0xFB1EU, 0x8BF9U, 0x9BD8U, 0xABBBU, 0xBB9AU,
    0x4A75U, 0x5A54U, 0x6A37U, 0x7A16U, 0x0AF1U, 0x1AD0U, 0x2AB3U, 0x3A92U,
    0xFD2EU, 0xED0FU, 0xDD6CU, 0xCD4DU, 0xBDAAU, 0xAD8BU, 0x9DE8U, 0x8DC9U,
    0x7C26U, 0x6C07U, 0x5C64U, 0x4C45U, 0x3CA2U, 0x2C83U, 0x1CE0U, 0x0CC1U,
    0xEF1FU, 0xFF3EU, 0xCF5DU, 0xDF7CU, 0xAF9BU, 0xBFBAU, 0x8FD9U, 0x9FF8U,
    0x6E17U, 0x7E36U, 0x4E55U, 0x5E74U, 0x2E93U, 0x3EB2U, 0x0ED1U, 0x1EF0U
};

/* ========================================================================
 * Configuration
 * ======================================================================== */

#define APP_PROTOCOL_RX_BUF_SIZE        64U
#define APP_PROTOCOL_CONTROL_PERIOD_MS  10U
#define APP_PROTOCOL_FEEDBACK_PERIOD_MS 100U
#define APP_PROTOCOL_FEEDBACK_COUNT     (APP_PROTOCOL_FEEDBACK_PERIOD_MS / \
                                         APP_PROTOCOL_CONTROL_PERIOD_MS)

/* ========================================================================
 * Frame state machine
 * ======================================================================== */

typedef enum {
    STATE_SOF = 0,
    STATE_LENGTH,
    STATE_DATA
} FrameState;

/* ========================================================================
 * Static variables
 * ======================================================================== */

static uint8_t s_rx_buf[APP_PROTOCOL_RX_BUF_SIZE];

/* Target wheel RPM received from C board (volatile, ISR writes this) */
static volatile float s_target_fl_rpm;
static volatile float s_target_fr_rpm;
static volatile float s_target_rl_rpm;
static volatile float s_target_rr_rpm;

static uint8_t  s_initialized;
static uint32_t s_frame_err_count;
static uint32_t s_crc_err_count;
static uint16_t s_feedback_tick;

/* ========================================================================
 * CRC16
 * ======================================================================== */

uint16_t App_Protocol_CalcCRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0x0000U;
    uint16_t i;

    if ((data == NULL) || (len == 0U)) {
        return 0U;
    }

    for (i = 0U; i < len; i++) {
        crc = (uint16_t)((crc << 8) ^
               s_crc16_table[(uint8_t)((crc >> 8) ^ data[i])]);
    }

    return crc;
}

/* ========================================================================
 * Command handlers
 * ======================================================================== */

static void App_Protocol_HandleSetWheelSpeeds(const uint8_t *data, uint8_t len)
{
    int16_t fl, fr, rl, rr;

    if (len < 8U) {
        return;
    }

    fl = (int16_t)(((uint16_t)data[0] << 8) | (uint16_t)data[1]);
    fr = (int16_t)(((uint16_t)data[2] << 8) | (uint16_t)data[3]);
    rl = (int16_t)(((uint16_t)data[4] << 8) | (uint16_t)data[5]);
    rr = (int16_t)(((uint16_t)data[6] << 8) | (uint16_t)data[7]);

    s_target_fl_rpm = (float)fl;
    s_target_fr_rpm = (float)fr;
    s_target_rl_rpm = (float)rl;
    s_target_rr_rpm = (float)rr;
}

static void App_Protocol_HandleEmergencyStop(void)
{
    s_target_fl_rpm = 0.0f;
    s_target_fr_rpm = 0.0f;
    s_target_rl_rpm = 0.0f;
    s_target_rr_rpm = 0.0f;

    App_ChasisStop();
}

static void App_Protocol_HandleHeartbeat(void)
{
    /* Reply with heartbeat ACK */
    uint8_t ack_frame[6];

    ack_frame[0] = APP_PROTOCOL_SOF;
    ack_frame[1] = 3U;  /* Length = cmd(1) + crc(2) = 3 */
    ack_frame[2] = APP_PROTOCOL_CMD_HEARTBEAT_ACK;

    uint16_t crc = App_Protocol_CalcCRC16(&ack_frame[2], 1U);
    ack_frame[3] = (uint8_t)(crc >> 8);
    ack_frame[4] = (uint8_t)(crc & 0xFFU);

    NET_UART_Send(NET_UART_USART6, ack_frame, 5U, 10U);
}

/* ========================================================================
 * Feedback
 * ======================================================================== */

static void App_Protocol_SendWheelFeedback(void)
{
    uint8_t frame[13];  /* SOF + Len + Cmd + 8 data + 2 CRC = 13 bytes */
    int16_t fl, fr, rl, rr;
    uint16_t crc;

    /* Skip if previous DMA TX is still in flight (avoids blocking in ISR) */
    if (huart6.gState == HAL_UART_STATE_BUSY_TX) {
        return;
    }

    fl = (int16_t)(Motor_GetSpeedRPM(MOTOR_FL) + 0.5f);
    fr = (int16_t)(Motor_GetSpeedRPM(MOTOR_FR) + 0.5f);
    rl = (int16_t)(Motor_GetSpeedRPM(MOTOR_RL) + 0.5f);
    rr = (int16_t)(Motor_GetSpeedRPM(MOTOR_RR) + 0.5f);

    frame[0] = APP_PROTOCOL_SOF;
    frame[1] = 11U;  /* cmd(1) + 8 data + crc(2) = 11 */
    frame[2] = APP_PROTOCOL_CMD_WHEEL_FEEDBACK;
    frame[3] = (uint8_t)(fl >> 8);
    frame[4] = (uint8_t)(fl & 0xFFU);
    frame[5] = (uint8_t)(fr >> 8);
    frame[6] = (uint8_t)(fr & 0xFFU);
    frame[7] = (uint8_t)(rl >> 8);
    frame[8] = (uint8_t)(rl & 0xFFU);
    frame[9] = (uint8_t)(rr >> 8);
    frame[10] = (uint8_t)(rr & 0xFFU);

    crc = App_Protocol_CalcCRC16(&frame[2], 9U);
    frame[11] = (uint8_t)(crc >> 8);
    frame[12] = (uint8_t)(crc & 0xFFU);

    /* Non-blocking DMA send — safe to call from ISR context */
    (void)HAL_UART_Transmit_DMA(&huart6, frame, 13U);
}

/* ========================================================================
 * Control loop (100 Hz)
 * ======================================================================== */

static void App_Protocol_Control100HzCallback(void)
{
    ChassisMecanumWheelRPM_t target_rpm;
    ChassisMecanumCmd_t cmd;

    /* Update encoder readings */
    Motor_UpdateSpeed(APP_PROTOCOL_CONTROL_PERIOD_MS);

    /* Read target from protocol (ISR-safe: simple float reads are atomic on M4) */
    target_rpm.fl_rpm = s_target_fl_rpm;
    target_rpm.fr_rpm = s_target_fr_rpm;
    target_rpm.rl_rpm = s_target_rl_rpm;
    target_rpm.rr_rpm = s_target_rr_rpm;

    /* If all targets are zero, skip control (saves CPU) */
    if ((target_rpm.fl_rpm == 0.0f) && (target_rpm.fr_rpm == 0.0f) &&
        (target_rpm.rl_rpm == 0.0f) && (target_rpm.rr_rpm == 0.0f)) {
        /* Periodic feedback even when stopped */
        s_feedback_tick++;
        if (s_feedback_tick >= APP_PROTOCOL_FEEDBACK_COUNT) {
            s_feedback_tick = 0U;
            App_Protocol_SendWheelFeedback();
        }
        return;
    }

    /* Convert target wheel RPM → chassis velocity command via forward kinematics,
     * then VelocityControl runs InverseKinematics → wheel RPM → speed PID.
     * This re-uses the existing tuned PID loops in app_chasis.c. */
    cmd = ChassisMecanum_ForwardKinematics(target_rpm);
    App_ChasisVelocityControl(cmd, APP_PROTOCOL_CONTROL_PERIOD_MS);

    /* Periodic feedback */
    s_feedback_tick++;
    if (s_feedback_tick >= APP_PROTOCOL_FEEDBACK_COUNT) {
        s_feedback_tick = 0U;
        App_Protocol_SendWheelFeedback();
    }
}

/* ========================================================================
 * UART RX callback (ISR context → keep it short)
 * ======================================================================== */

static void App_Protocol_RxCallback(NET_UARTId_t uart, uint8_t *data, uint16_t len)
{
    static FrameState state = STATE_SOF;
    static uint8_t frame_buf[APP_PROTOCOL_MAX_FRAME_LEN];
    static uint8_t frame_idx;
    static uint8_t expected_len;

    uint16_t i;

    for (i = 0U; i < len; i++) {
        uint8_t byte = data[i];

        switch (state) {
        case STATE_SOF:
            if (byte == APP_PROTOCOL_SOF) {
                state = STATE_LENGTH;
            }
            break;

        case STATE_LENGTH:
            if ((byte >= APP_PROTOCOL_MIN_FRAME_LEN - APP_PROTOCOL_FRAME_OVERHEAD + 1U)
                && (byte <= APP_PROTOCOL_MAX_FRAME_LEN - 1U)) {
                /* byte is Length = cmd + data + crc. Subtract 1 for SOF. */
                expected_len = byte;
                frame_idx = 0U;
                state = STATE_DATA;
            } else {
                state = STATE_SOF;
                s_frame_err_count++;
            }
            break;

        case STATE_DATA:
            frame_buf[frame_idx] = byte;
            frame_idx++;

            if (frame_idx >= expected_len) {
                uint16_t crc_received = ((uint16_t)frame_buf[expected_len - 2U] << 8)
                                      | (uint16_t)frame_buf[expected_len - 1U];
                uint16_t crc_calc = App_Protocol_CalcCRC16(frame_buf,
                                                           expected_len - 2U);

                if (crc_received == crc_calc) {
                    uint8_t cmd  = frame_buf[0];
                    uint8_t dlen = expected_len - APP_PROTOCOL_MIN_FRAME_LEN
                                   + APP_PROTOCOL_FRAME_OVERHEAD - 1U;

                    switch (cmd) {
                    case APP_PROTOCOL_CMD_SET_WHEEL_SPEEDS:
                        App_Protocol_HandleSetWheelSpeeds(&frame_buf[1], dlen);
                        break;
                    case APP_PROTOCOL_CMD_EMERGENCY_STOP:
                        App_Protocol_HandleEmergencyStop();
                        break;
                    case APP_PROTOCOL_CMD_HEARTBEAT:
                        App_Protocol_HandleHeartbeat();
                        break;
                    default:
                        break;
                    }
                } else {
                    s_crc_err_count++;
                }

                state = STATE_SOF;
            }
            break;

        default:
            state = STATE_SOF;
            break;
        }
    }

    /* Re-enable DMA-IDLE reception */
    NET_UART_ReceiveToIdleDMA(uart, s_rx_buf, APP_PROTOCOL_RX_BUF_SIZE);
}

/* ========================================================================
 * Public API
 * ======================================================================== */

void App_Protocol_Init(void)
{
    if (s_initialized) {
        return;
    }

    s_target_fl_rpm = 0.0f;
    s_target_fr_rpm = 0.0f;
    s_target_rl_rpm = 0.0f;
    s_target_rr_rpm = 0.0f;
    s_feedback_tick = 0U;

    /* Register UART RX callback */
    NET_UART_RegisterRxCallback(NET_UART_USART6, App_Protocol_RxCallback);

    /* Start DMA-IDLE reception */
    NET_UART_ReceiveToIdleDMA(NET_UART_USART6, s_rx_buf, APP_PROTOCOL_RX_BUF_SIZE);

    /* Register 100Hz control loop */
    BSP_Timer100Hz_RegisterCallback(App_Protocol_Control100HzCallback);
    BSP_Timer100Hz_Init();

    s_initialized = 1U;
}
