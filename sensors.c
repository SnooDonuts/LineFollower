#include "sensors.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

// ------------------ PIN DEFINITIONS ------------------
// You can move these if you want, or keep them in a separate config file.
#define MUXA_S0   2
#define MUXA_S1   3
#define MUXA_S2   4
#define MUXA_S3   5
#define MUXA_ADC  26  // ADC input channel 0 -> GPIO26

#define MUXB_S0   6
#define MUXB_S1   7
#define MUXB_S2   8
#define MUXB_S3   9
#define MUXB_ADC  27  // ADC input channel 1 -> GPIO27

// If you need to use these channel indices:
#define ADC_CHAN_MUXA 0
#define ADC_CHAN_MUXB 1

// ------------------ EXTERNAL GLOBALS DEFINED HERE ------------------
uint16_t sensorMin[NUM_SENSORS];
uint16_t sensorMax[NUM_SENSORS];

// ------------------ STATIC (PRIVATE) VARIABLES/FUNCTIONS ------------------
// If you want to filter sensor readings, define an array for that:
float sensorFiltered[NUM_SENSORS] = {0.0f};

// Exponential smoothing factor for sensor readings
static float alphaSensors = 0.3f;

// Helper function to select channel on the multiplexer
static void select_mux_channel(char mux, uint8_t channel) {
    bool s0 = channel & 0x01;
    bool s1 = channel & 0x02;
    bool s2 = channel & 0x04;
    bool s3 = channel & 0x08;

    if (mux == 'A') {
        gpio_put(MUXA_S0, s0);
        gpio_put(MUXA_S1, s1);
        gpio_put(MUXA_S2, s2);
        gpio_put(MUXA_S3, s3);
    } else {
        gpio_put(MUXB_S0, s0);
        gpio_put(MUXB_S1, s1);
        gpio_put(MUXB_S2, s2);
        gpio_put(MUXB_S3, s3);
    }
}

/**
 * @brief Setup the multiplexer pins as outputs.
 */
void setup_mux_pins(void) {
    // MUX A
    gpio_init(MUXA_S0);
    gpio_init(MUXA_S1);
    gpio_init(MUXA_S2);
    gpio_init(MUXA_S3);
    gpio_set_dir(MUXA_S0, GPIO_OUT);
    gpio_set_dir(MUXA_S1, GPIO_OUT);
    gpio_set_dir(MUXA_S2, GPIO_OUT);
    gpio_set_dir(MUXA_S3, GPIO_OUT);

    // MUX B
    gpio_init(MUXB_S0);
    gpio_init(MUXB_S1);
    gpio_init(MUXB_S2);
    gpio_init(MUXB_S3);
    gpio_set_dir(MUXB_S0, GPIO_OUT);
    gpio_set_dir(MUXB_S1, GPIO_OUT);
    gpio_set_dir(MUXB_S2, GPIO_OUT);
    gpio_set_dir(MUXB_S3, GPIO_OUT);
}

/**
 * @brief Initialize ADC hardware for reading from mux pins.
 */
void setup_adc(void) {
    adc_init();
    adc_gpio_init(MUXA_ADC); // GPIO26 => ADC0
    adc_gpio_init(MUXB_ADC); // GPIO27 => ADC1
}

/**
 * @brief Read raw ADC value from a given MUX channel (0..15).
 * @param mux 'A' or 'B'
 * @param channel 0..15
 * @return 12-bit ADC reading (0..4095)
 */
static uint16_t read_sensor(char mux, uint8_t channel) {
    select_mux_channel(mux, channel);
    if (mux == 'A') {
        adc_select_input(ADC_CHAN_MUXA);
    } else {
        adc_select_input(ADC_CHAN_MUXB);
    }
    return adc_read();
}

/**
 * @brief Read all 16 sensors (8 from each mux).
 */
void read_all_sensors(uint16_t *sensorValues) {
    // MUX A for sensors 0..7
    for (uint8_t ch = 0; ch < 8; ch++) {
        sensorValues[ch] = read_sensor('A', ch);
    }
    // MUX B for sensors 8..15
    for (uint8_t ch = 0; ch < 8; ch++) {
        sensorValues[ch + 8] = read_sensor('B', ch);
    }
}

/**
 * @brief Initialize global min/max arrays so we can store calibration data.
 */
void init_calibration_arrays(void) {
    for(int i = 0; i < NUM_SENSORS; i++) {
        sensorMin[i] = 4095; // largest possible for 12-bit ADC
        sensorMax[i] = 0;
    }
}

/**
 * @brief Run multiple readings to find the min/max values for each sensor.
 */
void calibrate_sensors(int iterations, int delay_ms) {
    for(int n = 0; n < iterations; n++) {
        uint16_t sensorValues[NUM_SENSORS];
        read_all_sensors(sensorValues);

        for(int i = 0; i < NUM_SENSORS; i++) {
            if(sensorValues[i] < sensorMin[i]) {
                sensorMin[i] = sensorValues[i];
            }
            if(sensorValues[i] > sensorMax[i]) {
                sensorMax[i] = sensorValues[i];
            }
        }
        sleep_ms(delay_ms);
    }
}

/**
 * @brief Convert a raw ADC reading into a calibrated range (0..1000).
 */
uint16_t get_calibrated_value(int sensorIndex, uint16_t rawVal) {
    if(rawVal < sensorMin[sensorIndex]) {
        rawVal = sensorMin[sensorIndex];
    }
    if(rawVal > sensorMax[sensorIndex]) {
        rawVal = sensorMax[sensorIndex];
    }
    if(sensorMax[sensorIndex] == sensorMin[sensorIndex]) {
        // avoid divide-by-zero
        return 0;
    }
    uint16_t range = sensorMax[sensorIndex] - sensorMin[sensorIndex];
    return (uint16_t)(((uint32_t)(rawVal - sensorMin[sensorIndex]) * 1000) / range);
}

/**
 * @brief Compute a weighted-average "line position" from sensor array.
 */
float compute_line_position(const uint16_t *sensorValues) {
    unsigned long sum = 0;
    unsigned long total = 0;
    for(int i = 0; i < NUM_SENSORS; i++) {
        unsigned long val = sensorValues[i];
        sum   += (i * val);
        total += val;
    }
    if(total == 0) {
        return -1.0f; // no line
    }
    return (float)sum / (float)total;
}

/**
 * @brief Exponential smoothing of calibrated sensor readings.
 */
void filter_sensors(const uint16_t *calibratedVals) {
    for(int i = 0; i < NUM_SENSORS; i++) {
        // filtered[i] = alpha * new + (1-alpha) * old
        sensorFiltered[i] = alphaSensors * calibratedVals[i] 
                          + (1.0f - alphaSensors) * sensorFiltered[i];
    }
}

// If you want to access the filtered results from other files,
// you could add another function, e.g.:
#if 0
float get_filtered_value(int i) {
    return sensorFiltered[i];
}
#endif
