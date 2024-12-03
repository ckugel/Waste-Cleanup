#include "uart.h"
#include "imu.h"

// Register definitions for the IMU
#define IMU_OPR_MODE 0x3D          // Operation mode register
#define IMU_EULER_H_LSB 0x1A       // Euler Heading (Yaw) LSB
#define IMU_EULER_H_MSB 0x1B       // Euler Heading (Yaw) MSB
#define IMU_CALIB_STAT 0x35        // Calibration status register

void imu_init() {
    imu_writeRegister(IMU_OPR_MODE, 0x0C);  // Set to NDOF mode
}

/**
 *  Write a value to a specific IMU register.
 * 
 * @param reg The register address to write to.
 * @param value The value to write to the register.
 */
void imu_writeRegister(uint8_t reg, uint8_t value) {
    uart_sendChar(0xAA);  // Start byte
    uart_sendChar(0x00);  // Write command
    uart_sendChar(reg);   // Register address
    uart_sendChar(value); // Data to write
    uart_sendChar((0xAA + 0x00 + reg + value) & 0xFF); // Checksum
}

/**
 Read a value from a specific IMU register.
 * 
 * @param reg The register address to read from.
 * @return uint8_t The value read from the register.
 */
uint8_t imu_readRegister(uint8_t reg) {
    uart_sendChar(0xAA);  // Start byte
    uart_sendChar(0x01);  // Read command
    uart_sendChar(reg);   // Register address
    uart_sendChar((0xAA + 0x01 + reg) & 0xFF);  // Checksum

    // Read response (5 bytes total: start byte, length, address, data, checksum)
    uint8_t response[5];
    for (int i = 0; i < 5; i++) {
        response[i] = uart_receive();
    }

    return response[3];  // Data byte is in response[3]
}

/**
 * Get the heading (yaw) from the IMU.
 * 
 * @return uint16_t The yaw angle in degrees.
 */
uint16_t imu_getHeading() {
    uint8_t lsb = imu_readRegister(IMU_EULER_H_LSB);
    uint8_t msb = imu_readRegister(IMU_EULER_H_MSB);
    return (msb << 8) | lsb;  // Combine MSB and LSB
}

/**
 * Get the calibration status of the IMU.
 * @return uint8_t The calibration status (0-3 for accelerometer, gyroscope, magnetometer, and system).
 */
uint8_t imu_getCalibrationStatus() {
    return imu_readRegister(IMU_CALIB_STAT);
}
