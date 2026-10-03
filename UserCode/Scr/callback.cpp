#include "can_user.h"
#include "Motor.hpp"

#include "can.h"
#include "tim.h"

Motor motor(19.2f);//电机的转速比

static constexpr uint32_t kMotor1FeedbackId = 0x204;//电机的ID

volatile float motor1_debug_current_a = 0.2f;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)  //CAN FIFO0接收中断回调
{
    uint8_t data[8];

    if (hcan != &hcan1) return;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, data) != HAL_OK) return;
    //从FIFO0中取出一帧数据

    if (rx_header.IDE == CAN_ID_STD
        && rx_header.RTR == CAN_RTR_DATA
        && rx_header.DLC == 8
        && rx_header.StdId == kMotor1FeedbackId) {
        motor.canRxMsgCallback(data);
        }
}  //如果符合要求，就解析并存储反馈的数据

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim->Instance != TIM6) return;//确保Timer配对了

    motor.setTxCurrent(motor1_debug_current_a, 4);//motor1_debug_current_a是控制电机转动电流的变量
  
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
        HAL_CAN_AddTxMessage(
            &hcan1,
            &tx_header,
            motor.getTxData(),
            &can_tx_mailbox);
    }   //当CAN发送有空位的时，就把电机控制的数据发送
}
