#ifndef LINEFOLLOWER_MOTORS_H
#define LINEFOLLOWER_MOTORS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Initialize the motor driver pins as PWM + direction.
 */
void setup_motors(void);

/**
 * @brief Set motor speeds, range -255..+255.
 * @param speedA Speed for motor A
 * @param speedB Speed for motor B
 */
void set_motor_speeds(int speedA, int speedB);

#endif // LINEFOLLOWER_MOTORS_H
