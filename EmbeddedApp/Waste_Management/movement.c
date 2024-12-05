/*
 * movement.c
 *
 *  Created on: Sep 10, 2024
 *      Author: ckugel05
 */

#include "movement.h"
#include "open_interface.h"
#include <math.h>

// minimum speed that the bot can move at
const int staticGain = 60;

//TODO: find Pk
double errorProportionAngle = 0; // experimentally found value

const int angleBounds = 1; // can change for end case for the turn to angle bounds

void deconstruct_routine(Routine* routine) {
    free(routine->moves);
}

// idk what type will be it might be a number or something
void send_move_uart(Move move) {
// format will be m {type} {quantity} m
  uart_sendStr("m ");
  char buffer[13];
  sprintf(buffer, "%d,%.2f", move.type, move.quantity);
  uart_sendStr(buffer);
  uart_sendStr(" m");
}

void send_routine_uart(Routine* routine) {
  // format will be R{moves}R
  // total unested format will then be R{type},{quantity}:{type},{quantity}:{type},{quantity}R
  uart_sendChar('R');
  uint8_t i;
  for (i = 0; i < routine->amount - 1; i++) {
     send_move_uart(routine->moves[i]);
    uart_sendChar(':');
  }
  send_move_uart(routine->moves[routine->amount - 1]);
  uart_sendChar('R');
}

//TODO: Change logic to handle both in same function
void move_forward(oi_t *sensor, int centimeters, Pose2D* robotPose) {
    double sum = 0;
    oi_setWheels(200, 200); //move forward; full speed
    while (sum < centimeters * 10) {
        oi_update(sensor);
        sum += sensor->distance;
    }
    oi_setWheels(0, 0); //stop

  translatePoseByMagnitude(robotPose, ((double) (centimeters)));

}

void move_backwards(oi_t *sensor, int centimeters, Pose2D* robotPose) {
    double sum = 0;
    centimeters = -centimeters;
    oi_setWheels(-200, -200); //move forward; full speed
    while (sum > centimeters * 10) {
        oi_update(sensor);
        sum += sensor->distance;
    }
    oi_setWheels(0, 0); //stop
  translatePoseByMagnitude(robotPose, -((double) centimeters));

}

/*Pillar*/ bumpy smart_move_forward(oi_t* sensor, int centimeters, Pose2D* robotPose) {
    double sum = 0;
    oi_setWheels(200, 200); //move forward; full speed
    while (sum / 10 < centimeters) {
        oi_update(sensor);
        sum += sensor->distance;
        uint8_t hole = getHoleTouching(sensor);
        uint8_t edge = getEdgeTouching(sensor);
        uint8_t target = getTargetTouching(sensor);
        if (target || edge || hole || sensor->bumpRight || sensor->bumpLeft) {
            translatePoseByMagnitude(robotPose, (sum / 10));
            // find the pillar position
            Pose2D hitPose = copy_of_pose(*robotPose);
            // the new magnitude should be (sum / 10) + SMALL_ITEM_RADIUS
            double oldMagnitude = sqrt(pow(hitPose.xy.x,2) + pow(hitPose.xy.y, 2));

            translatePoseByMagnitude(&hitPose, ((sum / 10) + SMALL_ITEM_RADIUS) / oldMagnitude);
            Pillar hit = {.position = hitPose, .radius = SMALL_ITEM_RADIUS};

            // backup
            // move_backwards(sensor, BACKUP_HIT_LENGTH, robotPose);
            return (bumpy) {.complete = false, .hitLeft = sensor->bumpLeft};
        }
    }
    oi_setWheels(0, 0); //stop
    // basically null
    translatePoseByMagnitude(robotPose, (sum / 10));
    //return (Pillar) {.radius = -1};
    return (bumpy) {.complete =  true};
}

void turn_clockwise(oi_t *sensor,int degrees) {
    double sum = 0;
    oi_setWheels(-20, 20); //moves wheels opposite direction to mimic turning
    while (sum < degrees) {
        oi_update(sensor);
        sum += -1 * sensor->angle;     //updating the angle bot is facing
    }
    oi_setWheels(0,0); //stop
}

// TODO: Change to turn to angle and handle both in same method
void turn_counter_clockwise(oi_t* sensor, int degrees) {
    double sum = 0;
    oi_setWheels(20, -20); //moves wheels opposite direction to mimic turning
    while (sum < degrees) {
        oi_update(sensor);
        sum += sensor->angle;     //updating the angle bot is facing
    }
    oi_setWheels(0,0); //stop

}

/**
 * Turn to an angle relative to the current angle.
 * Using a P-f controller.
 * Angle is in degrees, positive values are counter clockwise, negative values are clockwise
 */
void turn_to_angle(oi_t* sensor, double relAngle, Pose2D* robotPose) {
    // basically our odometry
    double sum = 0;

    double ffMulti = 1;

    // used for calculating speed to guarantee that we are not standing still
    // double lastMeasurement = -1;
    // double measurement = 0;

    if (relAngle < 0) {
        ffMulti = -1;
    }

    unsigned int startTime = timer_getMillis();


    // repeat the controller until we are within angle bounds of the target
    while (fabs(ffMulti * relAngle - sum) > angleBounds && timer_getMillis() - startTime < 15000 /*&& lastMeasurement != measurement*/) {
        // TODO: make static gain negative for counter clockwise
        int motorOutput = (errorProportionAngle * (relAngle - sum)) + (ffMulti * staticGain); // P-f controller. Our feedforward is a dumb static gain
        // Sign Convention
        // right wheel is standard for sign, if we want to turn clockwise we will have a negative
        // angle and a negative output on the right wheel will lead us to turn right
        oi_setWheels(motorOutput, -motorOutput);

        // wait 10 ms to make sure that we do not slam the v controller with changing setpoints
        // lastMeasurement = sensor->angle;
        timer_waitMillis(5);

        oi_update(sensor);
        sum += (ffMulti) * sensor->angle;
        // measurement = sensor->angle;
    }
    // at set point
    oi_setWheels(0, 0);

    robotPose->heading += relAngle;
}

void turn_to_angle_radians(oi_t* sensor, double radians, Pose2D* robotPose) {
  turn_to_angle(sensor, degrees_to_radians(radians), robotPose);
}

void multiplyPByTwo() {
    errorProportionAngle *= 2;
}

void dividePByTwo() {
    errorProportionAngle /= 2;
}

double getProportionAngle() {
    return errorProportionAngle;
}
