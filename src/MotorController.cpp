#include "MotorController.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

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
        printf("DEBUG: MotorController already initialized.\n");
        return true;
    }

    printf("DEBUG: Initializing MotorController...\n");
    printf("DEBUG: Pin configuration - Left: GPIO%u,GPIO%u | Right: GPIO%u,GPIO%u\n",
           config_.leftMotorPin1, config_.leftMotorPin2, 
           config_.rightMotorPin1, config_.rightMotorPin2);

    try {
        // Configure PWM for all motor pins
        printf("DEBUG: Configuring PWM for motor pins...\n");
        leftPwmSlice_ = configurePwmPin(config_.leftMotorPin1);
        configurePwmPin(config_.leftMotorPin2);
        rightPwmSlice_ = configurePwmPin(config_.rightMotorPin1);
        configurePwmPin(config_.rightMotorPin2);

        printf("DEBUG: PWM slices - Left: %u, Right: %u\n", leftPwmSlice_, rightPwmSlice_);

        // Get PWM channels for each pin
        leftPwmChannelA_ = pwm_gpio_to_channel(config_.leftMotorPin1);
        leftPwmChannelB_ = pwm_gpio_to_channel(config_.leftMotorPin2);
        rightPwmChannelA_ = pwm_gpio_to_channel(config_.rightMotorPin1);
        rightPwmChannelB_ = pwm_gpio_to_channel(config_.rightMotorPin2);

        printf("DEBUG: PWM channels - Left: %u,%u | Right: %u,%u\n",
               leftPwmChannelA_, leftPwmChannelB_, rightPwmChannelA_, rightPwmChannelB_);

    // Set PWM to requested frequency: f = sys_clk / (clkdiv * (wrap + 1))
    // Prefer clkdiv = 1 for maximum resolution and compute wrap accordingly
    constexpr uint32_t sys_clk_hz = 125000000; // 125 MHz default
    uint32_t target = config_.pwmFrequency == 0 ? 20000 : config_.pwmFrequency; // default 20kHz
    if (target < 100) target = 100; // avoid extremely low frequencies
    uint32_t wrap = (sys_clk_hz / target);
    if (wrap == 0) wrap = 1;
    if (wrap > 0) wrap -= 1;
    if (wrap > 65535u) wrap = 65535u;

        printf("DEBUG: PWM setup - Target freq: %uHz, Wrap: %u\n", target, wrap);

    // Initialize PWM slices with proper configuration (like Pimoroni examples)
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv(&cfg, 1.0f);
    pwm_config_set_wrap(&cfg, static_cast<uint16_t>(wrap));
    
    // Apply configuration and start PWM
    pwm_init(leftPwmSlice_, &cfg, true);
    pwm_init(rightPwmSlice_, &cfg, true);

        // Start with motors stopped
        printf("DEBUG: Stopping all motors initially...\n");
        stopAllMotors();

        initialized_ = true;
        printf("DEBUG: MotorController initialization successful!\n");
        return true;
    }
    catch (...) {
        printf("ERROR: MotorController initialization failed with exception!\n");
        return false;
    }
}

void MotorController::setMotorSpeed(Motor motor, float speed)
{
    if (!initialized_) {
        printf("DEBUG: MotorController not initialized! Cannot set motor speed.\n");
        return;
    }

    // Debug: Log all motor speed commands
    printf("DEBUG: setMotorSpeed - Motor=%s, Speed=%.3f\n", 
           motor == Motor::LEFT ? "LEFT" : "RIGHT", speed);

    // Constrain speed to valid range
    speed = constrain(speed, -1.0f, 1.0f);

    uint8_t pin1, pin2;
    
    if (motor == Motor::LEFT) {
        pin1 = config_.leftMotorPin1;
        pin2 = config_.leftMotorPin2;
        printf("DEBUG: Left motor pins - Pin1=%u (GPIO%u), Pin2=%u (GPIO%u)\n", pin1, pin1, pin2, pin2);
    } else {
        pin1 = config_.rightMotorPin1;
        pin2 = config_.rightMotorPin2;
        printf("DEBUG: Right motor pins - Pin1=%u (GPIO%u), Pin2=%u (GPIO%u)\n", pin1, pin1, pin2, pin2);
    }

    if (speed > 0.0f) {
        // Forward direction (now both motors use what was previously 'reverse' logic)
        if (motor == Motor::LEFT) {
            printf("DEBUG: Motor forward (LEFT now reverse logic) - GPIO%u=%.3f, GPIO%u=0.0\n", pin1, speed, pin2);
            setPwmDutyCycle(pin1, speed);
            setPwmDutyCycle(pin2, 0.0f);
        } else {
            printf("DEBUG: Motor forward (RIGHT now reverse logic) - GPIO%u=0.0, GPIO%u=%.3f\n", pin1, pin2, speed);
            setPwmDutyCycle(pin1, 0.0f);
            setPwmDutyCycle(pin2, speed);
        }
    } else if (speed < 0.0f) {
        // Reverse direction (now both motors use what was previously 'forward' logic)
        if (motor == Motor::LEFT) {
            printf("DEBUG: Motor reverse (LEFT now forward logic) - GPIO%u=0.0, GPIO%u=%.3f\n", pin1, pin2, -speed);
            setPwmDutyCycle(pin1, 0.0f);
            setPwmDutyCycle(pin2, -speed);
        } else {
            printf("DEBUG: Motor reverse (RIGHT now forward logic) - GPIO%u=%.3f, GPIO%u=0.0\n", pin1, -speed, pin2);
            setPwmDutyCycle(pin1, -speed);
            setPwmDutyCycle(pin2, 0.0f);
        }
    } else {
        // Stop (brake)
        printf("DEBUG: Motor stop - GPIO%u=0.0, GPIO%u=0.0\n", pin1, pin2);
        setPwmDutyCycle(pin1, 0.0f);
        setPwmDutyCycle(pin2, 0.0f);
    }
}

void MotorController::setDifferentialDrive(float forward, float turn)
{
    if (!initialized_) {
        printf("DEBUG: MotorController not initialized! Cannot drive.\n");
        return;
    }

    // Debug: Log all differential drive calls
    printf("DEBUG: setDifferentialDrive called - forward=%.3f, turn=%.3f\n", forward, turn);

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

    // Debug: Log calculated motor speeds
    printf("DEBUG: Motor speeds - Left=%.3f, Right=%.3f\n", leftSpeed, rightSpeed);

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
    printf("DEBUG: Configuring GPIO%u for PWM...\n", pin);
    
    // Initialize GPIO first (like Pimoroni examples do)
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, false);  // Start with pin low
    
    // Set GPIO function to PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);
    
    // Get PWM slice number
    uint8_t slice = pwm_gpio_to_slice_num(pin);
    printf("DEBUG: GPIO%u assigned to PWM slice %u\n", pin, slice);
    
    return slice;
}

void MotorController::setPwmDutyCycle(uint8_t pin, float dutyCycle)
{
    // Debug: Log PWM commands
    printf("DEBUG: setPwmDutyCycle - GPIO%u, DutyCycle=%.3f", pin, dutyCycle);
    
    // Constrain duty cycle
    dutyCycle = constrain(dutyCycle, 0.0f, 1.0f);
    
    // Calculate PWM level (0-65535)
    uint16_t level = static_cast<uint16_t>(dutyCycle * 65535);
    
    printf(" -> PWM Level=%u\n", level);
    
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
