/*
 * movement.h
 *
 *  Created on: Sep 10, 2024
 *      Author: ckugel05
 */

#ifndef MOVEMENT_H_
#define MOVEMENT_H_

#include "open_interface.h"
#include "uart.h"
#include "Graph.h"
#include "cliff.h"

#define BACKUP_HIT_LENGTH 20

typedef struct {
    bool complete;
    bool hitLeft;
} bumpy;

typedef enum {
    MOVE_FORWARD,
    MOVE_BACKWARDS,
    MOVE_FORWARD_SMART,
    TURN_TO_ANGLE,
    SCAN
} MovementType;

typedef struct {
    double quantity;
    MovementType type;
} Move;

typedef struct {
    Move* moves;
    uint8_t amount;
} Routine;

void send_move_uart(Move move);
void send_routine_uart(Routine* routine);
void deconstruct_routine(Routine* routine);

void move_forward(oi_t *sensor, int centimeters, Pose2D* robotPose);
void turn_clockwise(oi_t *sensor, int degrees);
bumpy smart_move_forward(oi_t* sensor, int centimeters, Pose2D* robotPose);
void move_backwards(oi_t *sensor, int centimeters, Pose2D* robotPose);
void turn_counter_clockwise(oi_t* sensor, int degrees);
void turn_to_angle(oi_t* sensor, double degrees, Pose2D* robotPose);
void multiplyPByTwo();
void dividePByTwo();
double getProportionAngle();
void turn_to_angle_radians(oi_t* sensor, double radians, Pose2D* robotPose);


#endif /* MOVEMENT_H_ */
