#ifndef LINEFOLLOWER_SENSORS_H
#define LINEFOLLOWER_SENSORS_H

#include <stdint.h>
#include <stdbool.h> // If you need bool

// ------------------ CONSTANTS & MACROS ------------------

// How many reflectance sensors do you have?
#define NUM_SENSORS 16
// "extern" means this array is defined elsewhere
extern float sensorFiltered[NUM_SENSORS];

// ------------------ EXTERNAL GLOBALS ------------------
// These allow other files (like main.c) to see these arrays if needed.
extern uint16_t sensorMin[NUM_SENSORS];
extern uint16_t sensorMax[NUM_SENSORS];

// ------------------ FUNCTION PROTOTYPES ------------------

/**
 * @brief Configure pins for multiplexer select lines (S0..S3).
 */
void setup_mux_pins(void);

/**
 * @brief Initialize the ADC and associate pins (e.g., GPIO26, GPIO27).
 */
void setup_adc(void);

/**
 * @brief Reads all 16 sensors from two multiplexers and stores them.
 * @param sensorValues pointer to an array of size NUM_SENSORS
 */
void read_all_sensors(uint16_t *sensorValues);

/**
 * @brief Initialize global min/max arrays for calibration.
 */
void init_calibration_arrays(void);

/**
 * @brief Collect sensor data multiple times to determine min/max values.
 * @param iterations Number of readings to take
 * @param delay_ms   Delay between each reading (milliseconds)
 */
void calibrate_sensors(int iterations, int delay_ms);

/**
 * @brief Map a raw sensor reading to a calibrated 0..1000 range.
 * @param sensorIndex Which sensor (0..NUM_SENSORS-1)
 * @param rawVal      The raw ADC reading (0..4095)
 * @return            Calibrated value (0..1000)
 */
uint16_t get_calibrated_value(int sensorIndex, uint16_t rawVal);

/**
 * @brief Compute weighted-average line position from array of sensor values.
 * @param sensorValues array of size NUM_SENSORS
 * @return float from 0..(NUM_SENSORS-1), or -1 if no line is detected
 */
float compute_line_position(const uint16_t *sensorValues);

/**
 * @brief Apply exponential smoothing to sensor readings.
 * @param calibratedVals Array of newly calibrated readings (0..1000)
 */
void filter_sensors(const uint16_t *calibratedVals);

#endif // LINEFOLLOWER_SENSORS_H
