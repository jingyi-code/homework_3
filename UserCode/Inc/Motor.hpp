//
// Created by Administrator on 2026/10/3.
//

#ifndef HOMEWORK3_MOTOR_HPP
#define HOMEWORK3_MOTOR_HPP

#include <cstdint>

class Motor {
public:
    explicit Motor(float ratio);

    void canRxMsgCallback(const uint8_t rx_data[8]);

    float angle() const;
    float speedRpm() const;
    float currentAmps() const;
    float temperatureC() const;
    bool hasFeedback() const;

    void setTxCurrent(float amperes, uint8_t motor_id);
    //设置发送电流

    uint8_t* getTxData();
    //获取can发送数组

private:
    const float ratio_;

    float ecd_angle_ = 0; // 当前转子机械角度（度）
    float speedRpm_ = 0;
    float currentA_ = 0;
    float tempC_ = 0;

    float angle_ = 0;
    float last_ecd_ = 0;
    bool received_ = false;

    uint8_t tx_data_[8] = {};

    // TODO:
    // 1. 拆 8 字节为字段（字节序 + 符号）
    // 2. 角度换算（角度 * 360 / 8192）
    // 3. 增量累加 + 过零点环绕修正
    // 4. 换算到输出轴（除以减速比）
    static constexpr uint16_t kEncoderRange = 8192;
};

#endif //HOMEWORK3_MOTOR_HPP
