#include "uart.h"
#include "imu.h"

// Register definitions
#define BNO055_OPR_MODE 0x3D
#define BNO055_EULER_H_LSB 0x1A
#define BNO055_EULER_H_MSB 0x1B
#define BNO055_CALIB_STAT 0x35

// Initialize the IMU
void imu_init() {
    imu_writeRegister(BNO055_OPR_MODE, 0x0C);  // Set to NDOF mode
}

// Write to a BNO055 register
void imu_writeRegister(uint8_t reg, uint8_t value) {
    uart_sendChar(0xAA);  // Start byte
    uart_sendChar(0x00);  // Write command
    uart_sendChar(reg);   // Register address
    uart_sendChar(value); // Value to write
    uart_sendChar((0xAA + 0x00 + reg + value) & 0xFF); // Checksum
}

// Read from a BNO055 register
uint8_t imu_readRegister(uint8_t reg) {
    uart_sendChar(0xAA);  // Start byte
    uart_sendChar(0x01);  // Read command
    uart_sendChar(reg);   // Register address
    uart_sendChar((0xAA + 0x01 + reg) & 0xFF);  // Checksum

    // Read the response (5 bytes total)
    uint8_t response[5];
    for (int i = 0; i < 5; i++) {
        response[i] = uart_receive();
    }

    return response[3];  // Data byte
}

// Get Euler heading (Yaw)
uint16_t imu_getHeading() {
    uint8_t lsb = imu_readRegister(BNO055_EULER_H_LSB);
    uint8_t msb = imu_readRegister(BNO055_EULER_H_MSB);
    return (msb << 8) | lsb;  // Combine MSB and LSB
}

// Get calibration status
uint8_t imu_getCalibrationStatus() {
    return imu_readRegister(BNO055_CALIB_STAT);
}
