#include "motors.h"

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

// ------------------ PIN DEFINITIONS ------------------
#define MOTOR_A_PWM  10
#define MOTOR_A_DIR  11
#define MOTOR_B_PWM  12
#define MOTOR_B_DIR  13

/**
 * @brief Configure motor pins for PWM and direction output.
 */
void setup_motors(void) {
    // Direction pins
    gpio_init(MOTOR_A_DIR);
    gpio_set_dir(MOTOR_A_DIR, GPIO_OUT);

    gpio_init(MOTOR_B_DIR);
    gpio_set_dir(MOTOR_B_DIR, GPIO_OUT);

    // PWM config
    uint sliceA = pwm_gpio_to_slice_num(MOTOR_A_PWM);
    gpio_set_function(MOTOR_A_PWM, GPIO_FUNC_PWM);
    pwm_set_wrap(sliceA, 255);
    pwm_set_chan_level(sliceA, pwm_gpio_to_channel(MOTOR_A_PWM), 0);
    pwm_set_enabled(sliceA, true);

    uint sliceB = pwm_gpio_to_slice_num(MOTOR_B_PWM);
    gpio_set_function(MOTOR_B_PWM, GPIO_FUNC_PWM);
    pwm_set_wrap(sliceB, 255);
    pwm_set_chan_level(sliceB, pwm_gpio_to_channel(MOTOR_B_PWM), 0);
    pwm_set_enabled(sliceB, true);
}

/**
 * @brief Set both motors' speeds, using the assigned PWM slices/channels.
 */
void set_motor_speeds(int speedA, int speedB) {
    // Motor A
    uint sliceA = pwm_gpio_to_slice_num(MOTOR_A_PWM);
    bool dirA   = (speedA >= 0);
    int levelA  = (speedA >= 0) ? speedA : -speedA;
    if (levelA > 255) levelA = 255;

    gpio_put(MOTOR_A_DIR, dirA);
    pwm_set_chan_level(sliceA, pwm_gpio_to_channel(MOTOR_A_PWM), levelA);

    // Motor B
    uint sliceB = pwm_gpio_to_slice_num(MOTOR_B_PWM);
    bool dirB   = (speedB >= 0);
    int levelB  = (speedB >= 0) ? speedB : -speedB;
    if (levelB > 255) levelB = 255;

    gpio_put(MOTOR_B_DIR, dirB);
    pwm_set_chan_level(sliceB, pwm_gpio_to_channel(MOTOR_B_PWM), levelB);
}
