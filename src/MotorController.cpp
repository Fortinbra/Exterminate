#include "MotorController.h"
#include <algorithm>
#include <cmath>

namespace Exterminate {

MotorController::MotorController(const Config& config)
    : config_(config)
    , initialized_(false)
    , leftPwmSlice_(0)
    , rightPwmSlice_(0)
    , leftPwmChannelA_(0)
    , leftPwmChannelB_(0)
    , rightPwmChannelA_(0)
    , rightPwmChannelB_(0)
{
    // Constructor only stores configuration - actual initialization happens in initialize()
}

MotorController::~MotorController()
{
    if (initialized_) {
        stopAllMotors();
        
        // Disable PWM slices
        pwm_set_enabled(leftPwmSlice_, false);
        pwm_set_enabled(rightPwmSlice_, false);
        
        // Reset GPIO pins to input mode
        gpio_set_function(config_.leftMotorPin1, GPIO_FUNC_SIO);
        gpio_set_function(config_.leftMotorPin2, GPIO_FUNC_SIO);
        gpio_set_function(config_.rightMotorPin1, GPIO_FUNC_SIO);
        gpio_set_function(config_.rightMotorPin2, GPIO_FUNC_SIO);
        
        gpio_set_dir(config_.leftMotorPin1, GPIO_IN);
        gpio_set_dir(config_.leftMotorPin2, GPIO_IN);
        gpio_set_dir(config_.rightMotorPin1, GPIO_IN);
        gpio_set_dir(config_.rightMotorPin2, GPIO_IN);
    }
}

bool MotorController::initialize()
{
    if (initialized_) {
        return true;
    }

    try {
        // Configure PWM for all motor pins
        leftPwmSlice_ = configurePwmPin(config_.leftMotorPin1);
        configurePwmPin(config_.leftMotorPin2);
        rightPwmSlice_ = configurePwmPin(config_.rightMotorPin1);
        configurePwmPin(config_.rightMotorPin2);

        // Get PWM channels for each pin
        leftPwmChannelA_ = pwm_gpio_to_channel(config_.leftMotorPin1);
        leftPwmChannelB_ = pwm_gpio_to_channel(config_.leftMotorPin2);
        rightPwmChannelA_ = pwm_gpio_to_channel(config_.rightMotorPin1);
        rightPwmChannelB_ = pwm_gpio_to_channel(config_.rightMotorPin2);

        // Set PWM frequency
        uint32_t clockDiv = 125000000 / (config_.pwmFrequency * 65536);
        if (clockDiv < 1) clockDiv = 1;
        if (clockDiv > 255) clockDiv = 255;

        pwm_set_clkdiv(leftPwmSlice_, static_cast<float>(clockDiv));
        pwm_set_clkdiv(rightPwmSlice_, static_cast<float>(clockDiv));

        // Set PWM wrap value (controls resolution)
        pwm_set_wrap(leftPwmSlice_, 65535);
        pwm_set_wrap(rightPwmSlice_, 65535);

        // Start with motors stopped
        stopAllMotors();

        // Enable PWM
        pwm_set_enabled(leftPwmSlice_, true);
        pwm_set_enabled(rightPwmSlice_, true);

        initialized_ = true;
        return true;
    }
    catch (...) {
        return false;
    }
}

void MotorController::setMotorSpeed(Motor motor, float speed)
{
    if (!initialized_) {
        return;
    }

    // Constrain speed to valid range
    speed = constrain(speed, -1.0f, 1.0f);

    uint8_t pin1, pin2;
    
    if (motor == Motor::LEFT) {
        pin1 = config_.leftMotorPin1;
        pin2 = config_.leftMotorPin2;
    } else {
        pin1 = config_.rightMotorPin1;
        pin2 = config_.rightMotorPin2;
    }

    if (speed > 0.0f) {
        // Forward direction
        setPwmDutyCycle(pin1, speed);
        setPwmDutyCycle(pin2, 0.0f);
    } else if (speed < 0.0f) {
        // Reverse direction
        setPwmDutyCycle(pin1, 0.0f);
        setPwmDutyCycle(pin2, -speed);
    } else {
        // Stop (brake)
        setPwmDutyCycle(pin1, 0.0f);
        setPwmDutyCycle(pin2, 0.0f);
    }
}

void MotorController::setDifferentialDrive(float forward, float turn)
{
    if (!initialized_) {
        return;
    }

    // Constrain inputs
    forward = constrain(forward, -1.0f, 1.0f);
    turn = constrain(turn, -1.0f, 1.0f);

    // Calculate differential drive speeds
    // For a two-wheel robot:
    // - Forward motion: both wheels move at the same speed
    // - Turning: outer wheel moves faster than inner wheel
    
    float leftSpeed = forward - turn;
    float rightSpeed = forward + turn;

    // Normalize speeds if they exceed the motor capability
    float maxSpeed = std::max(std::abs(leftSpeed), std::abs(rightSpeed));
    if (maxSpeed > 1.0f) {
        leftSpeed /= maxSpeed;
        rightSpeed /= maxSpeed;
    }

    // Set motor speeds
    setMotorSpeed(Motor::LEFT, leftSpeed);
    setMotorSpeed(Motor::RIGHT, rightSpeed);
}

void MotorController::stopAllMotors()
{
    if (!initialized_) {
        return;
    }

    setMotorSpeed(Motor::LEFT, 0.0f);
    setMotorSpeed(Motor::RIGHT, 0.0f);
}

uint8_t MotorController::configurePwmPin(uint8_t pin)
{
    // Set GPIO function to PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    
    // Get PWM slice number
    return pwm_gpio_to_slice_num(pin);
}

void MotorController::setPwmDutyCycle(uint8_t pin, float dutyCycle)
{
    // Constrain duty cycle
    dutyCycle = constrain(dutyCycle, 0.0f, 1.0f);
    
    // Calculate PWM level (0-65535)
    uint16_t level = static_cast<uint16_t>(dutyCycle * 65535);
    
    // Set PWM level
    pwm_set_gpio_level(pin, level);
}

float MotorController::constrain(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

} // namespace Exterminate
