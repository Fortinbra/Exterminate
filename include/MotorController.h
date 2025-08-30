#pragma once

#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include <cstdint>

namespace Exterminate {

/**
 * @brief Driver for DRV8833-based motor H-bridge (e.g. Pimoroni Motor SHIM for Pico)
 * 
 * This class provides control for two DC motors using a DRV8833-based H-bridge
 * such as the Pimoroni Motor SHIM for Pico. The SHIM simplifies wiring and
 * power distribution while the underlying code targets the DRV8833 control
 * interface (direction pins + PWM for speed control).
 * It follows RAII principles and provides differential drive control
 * suitable for a two-wheeled robot.
 */
class MotorController {
public:
    /**
     * @brief Motor identifiers
     */
    enum class Motor : uint8_t {
        LEFT = 0,
        RIGHT = 1
    };

    /**
     * @brief Configuration structure for motor controller
     */
    struct Config {
        uint8_t leftMotorPin1;   ///< Left motor direction pin 1 (AIN1)
        uint8_t leftMotorPin2;   ///< Left motor direction pin 2 (AIN2)
        uint8_t rightMotorPin1;  ///< Right motor direction pin 1 (BIN1)
        uint8_t rightMotorPin2;  ///< Right motor direction pin 2 (BIN2)
        uint32_t pwmFrequency;   ///< PWM frequency in Hz (typically 1000-20000)
    };

    /**
     * @brief Construct a new Motor Controller
     * 
     * @param config Configuration for the motor controller
     */
    explicit MotorController(const Config& config);

    /**
     * @brief Destroy the Motor Controller (RAII cleanup)
     */
    ~MotorController();

    // Disable copy constructor and assignment operator
    MotorController(const MotorController&) = delete;
    MotorController& operator=(const MotorController&) = delete;

    /**
     * @brief Initialize the motor controller hardware
     * 
     * @return true if initialization was successful
     */
    bool initialize();

    /**
     * @brief Set motor speed and direction
     * 
     * @param motor Which motor to control
     * @param speed Speed from -1.0 (full reverse) to 1.0 (full forward)
     */
    void setMotorSpeed(Motor motor, float speed);

    /**
     * @brief Set differential drive motion
     * 
     * @param forward Forward speed from -1.0 to 1.0
     * @param turn Turn rate from -1.0 (left) to 1.0 (right)
     */
    void setDifferentialDrive(float forward, float turn);

    /**
     * @brief Stop all motors immediately
     */
    void stopAllMotors();

    /**
     * @brief Check if the motor controller is initialized
     * 
     * @return true if initialized and ready to use
     */
    bool isInitialized() const { return initialized_; }

private:
    Config config_;
    bool initialized_;
    uint8_t leftPwmSlice_;
    uint8_t rightPwmSlice_;
    uint8_t leftPwmChannelA_;
    uint8_t leftPwmChannelB_;
    uint8_t rightPwmChannelA_;
    uint8_t rightPwmChannelB_;

    /**
     * @brief Configure PWM for a specific pin
     * 
     * @param pin GPIO pin number
     * @return PWM slice number
     */
    uint8_t configurePwmPin(uint8_t pin);

    /**
     * @brief Set PWM duty cycle for a pin
     * 
     * @param pin GPIO pin number
     * @param dutyCycle Duty cycle from 0.0 to 1.0
     */
    void setPwmDutyCycle(uint8_t pin, float dutyCycle);

    /**
     * @brief Constrain a value between min and max
     * 
     * @param value Value to constrain
     * @param min Minimum value
     * @param max Maximum value
     * @return Constrained value
     */
    static float constrain(float value, float min, float max);
};

} // namespace Exterminate
