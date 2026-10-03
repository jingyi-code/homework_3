//
// Created by Administrator on 2026/10/3.
//
#include "Motor.hpp"

Motor::Motor(float ratio)
    : ratio_(ratio)
{
}

void Motor::canRxMsgCallback(const uint8_t rx_data[8])
{
    const uint16_t ecd = (static_cast<uint16_t>(rx_data[0]) << 8) | rx_data[1];
    const int16_t speed_raw = static_cast<int16_t>(
        (static_cast<uint16_t>(rx_data[2]) << 8) | rx_data[3]);
    const int16_t current_raw = static_cast<int16_t>(
        (static_cast<uint16_t>(rx_data[4]) << 8) | rx_data[5]);
    ecd_angle_ = static_cast<float>(ecd) * 360.0f / kEncoderRange;
    speedRpm_ = static_cast<float>(speed_raw);
    currentA_ = static_cast<float>(current_raw) * 20.0f / 16384.0f;
    tempC_ = static_cast<float>(rx_data[6]);
    if(!received_){
        last_ecd_ = static_cast<float>(ecd);
        received_ = true;
        return;
    }
    float delta = static_cast<float>(ecd) - last_ecd_;
    if(delta > kEncoderRange / 2.0f){
        delta -= kEncoderRange;
    }
    else if(delta < -kEncoderRange / 2.0f){
        delta += kEncoderRange;
    }
    angle_ += delta * 360.0f / kEncoderRange / ratio_;
    last_ecd_ = static_cast<float>(ecd);
}

float Motor::angle() const
{
    return angle_;
}

float Motor::speedRpm() const
{
    return speedRpm_;
}

float Motor::currentAmps() const
{
    return currentA_;
}

float Motor::temperatureC() const
{
    return tempC_;
}

bool Motor::hasFeedback() const
{
    return received_;
}

void Motor::setTxCurrent(float amperes, uint8_t motor_id)
{

    if (motor_id < 1 || motor_id > 4)//这里0x200只控制1到4
    {
        return;
    }
    if (amperes > 20.0f) amperes = 20.0f;
    if (amperes < -20.0f) amperes = -20.0f;

    const int16_t current_command = static_cast<int16_t>(
        amperes * 16384.0f / 20.0f);
    const uint16_t packed = static_cast<uint16_t>(current_command);
    const uint8_t index = static_cast<uint8_t>((motor_id - 1) * 2);
    tx_data_[index] = static_cast<uint8_t>(packed >> 8);  //写到tx_data里面去（高八位）
    tx_data_[index + 1] = static_cast<uint8_t>(packed & 0xFF);//低八位
}

uint8_t* Motor::getTxData()
{
    return tx_data_;//将写好的tx_data_发送
}
