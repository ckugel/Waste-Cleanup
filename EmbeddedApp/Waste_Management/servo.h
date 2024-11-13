/*
 * servo.h
 *
 *  Created on: Nov 5, 2024
 *      Author: dwitty7
 */

#ifndef SERVO_H_
#define SERVO_H_

#include "Timer.h"
#include <inc/tm4c123gh6pm.h>

void servo_init(void);

void servo_mode(float);

void servo_calibrate();
#endif /* SERVO_H_ */
