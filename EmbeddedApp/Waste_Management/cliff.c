/*
 * cliff.c
 *
 *  Created on: Nov 12, 2024
 *      Author: ckugel05
 */


#include "cliff.h"

bool hole_Fleft(oi_t* sensor_data) {
    return (sensor_data->cliffFrontLeftSignal HOLE_THRESHOLD);
}

bool hole_Fright(oi_t* sensor_data) {
    return (sensor_data->cliffFrontRightSignal HOLE_THRESHOLD);
}

bool hole_left(oi_t* sensor_data) {
    return (sensor_data->cliffLeftSignal HOLE_THRESHOLD);
}

bool hole_right(oi_t* sensor_data) {
    return (sensor_data->cliffRightSignal HOLE_THRESHOLD);
}

bool edge_of_field_FLeft(oi_t* sensor_data) {
    return (sensor_data->cliffFrontLeftSignal EDGE_THRESHOLD);
}

bool edge_of_field_Left(oi_t* sensor_data) {
    return (sensor_data->cliffLeftSignal EDGE_THRESHOLD);
}

bool edge_of_field_FRight(oi_t* sensor_data) {
    return (sensor_data->cliffFrontRightSignal EDGE_THRESHOLD);
}

bool edge_of_field_Right(oi_t* sensor_data) {
    return (sensor_data->cliffRightSignal EDGE_THRESHOLD);
}
