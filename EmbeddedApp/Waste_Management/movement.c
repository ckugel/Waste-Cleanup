/*
 * movement.c
 *
 *  Created on: Sep 11, 2024
 *      Author: udips
 */
#include "open_interface.h"

void move_forward(oi_t *sensor_data, int centimeters) {
    oi_init(sensor_data);
     double sum = 0;
     oi_setWheels(100, 100);
     while (sum < centimeters * 10) {
         oi_update(sensor_data);
         sum += sensor_data->distance;
     }
     oi_setWheels(0, 0);
}

void move_backward(oi_t *sensor_data, int centimeters) {
    oi_init(sensor_data);
    double sum = 0;
    oi_setWheels(-100, -100);
    while (sum > centimeters * -10) {
       oi_update(sensor_data);
       sum += sensor_data->distance;
    }
    oi_setWheels(0, 0);
}

void turn_clockwise(oi_t *sensor_data, int degrees) {
    oi_init(sensor_data);
    int degrees_turned = 0;

    oi_setWheels(-100, 100);

    while(degrees_turned > degrees * -1) {
        oi_update(sensor_data);
        degrees_turned += sensor_data -> angle;
    }
    oi_setWheels(0, 0);
}
void turn_counter_clockwise(oi_t *sensor_data, int degrees) {
    oi_init(sensor_data);
    int degrees_turned = 0;

    oi_setWheels(100, -100);

    while(degrees_turned < degrees) {
        oi_update(sensor_data);
        degrees_turned += sensor_data -> angle;
    }
    oi_setWheels(0, 0);
}
