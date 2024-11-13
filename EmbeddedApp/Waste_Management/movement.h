/*
 * movement.h
 *
 *  Created on: Sep 11, 2024
 *      Author: udips
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_
#include "open_interface.h"

void move_forward(oi_t *, int); // Moves forward 100 cm

void move_backward(oi_t *, int); // Moves backwards

void turn_clockwise(oi_t *, int); // Turns clockwise

void turn_counter_clockwise(oi_t *, int); // Turns counter clockwise

#endif /* MOVEMENT_H_ */
