#ifndef IMU_H_
#define IMU_H_

#include <stdint.h>

// Function declarations
void imu_init();
void imu_writeRegister(uint8_t reg, uint8_t value);
uint8_t imu_readRegister(uint8_t reg);
uint16_t imu_getHeading();
uint8_t imu_getCalibrationStatus();

#endif /* IMU_H_ */
