#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "sensors.h"
#include "motors.h"

int main() {
    stdio_init_all();
    cyw43_arch_init();

    // Blink the LED to show we started
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    sleep_ms(1000);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);

    // Setup hardware
    setup_mux_pins();
    setup_adc();
    setup_motors();

    init_calibration_arrays();

    // Calibrating white
    for (int i; i < 6; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(200);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(200);
    }
    calibrate_sensors(100, 10);

    //Calibrating black
    for (int i; i < 6; i++) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
        sleep_ms(200);
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
        sleep_ms(200);
    }
    calibrate_sensors(100, 10);

    while (true) {
        // Read raw sensors
        uint16_t raw[NUM_SENSORS];
        read_all_sensors(raw);

        // Convert to calibrated
        uint16_t cal[NUM_SENSORS];
        for(int i = 0; i < NUM_SENSORS; i++) {
            cal[i] = get_calibrated_value(i, raw[i]);
        }

        // Filter them
        filter_sensors(cal);

        // Convert filtered floats back to integer if needed
        // (or write a float-based compute_line_position)
        uint16_t filtered[NUM_SENSORS];
        // You'd need a getter or make sensorFiltered[] external
        // but let's assume we do that:
        for(int i = 0; i < NUM_SENSORS; i++){
            extern float sensorFiltered[NUM_SENSORS]; // or use a function
            filtered[i] = (uint16_t)(sensorFiltered[i] + 0.5f);
        }

        // Get line position
        float pos = compute_line_position(filtered);

        // ... do your PID and set_motor_speeds() here ...

        sleep_ms(10);
    }
    return 0;
}

