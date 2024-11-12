/*
 * cliff.h
 *
 *  Created on: Nov 12, 2024
 *      Author: ckugel05
 */

#ifndef CLIFF_H_
#define CLIFF_H_

#include "open_interface.h"

#define HOLE_THRESHOLD < 1000
#define EDGE_THRESHOLD > 2000

// assume sensor data has been updated for all of these
bool hole_Fleft(oi_t* sensor_data);

bool hole_Fright(oi_t* sensor_data);

bool hole_left(oi_t* sensor_data);

bool hole_right(oi_t* sensor_data);

bool target_left(oi_t* sensor_data);

bool target_right(oi_t* sensor_data);

bool edge_of_field_FLeft(oi_t* sensor_data);
bool edge_of_field_Left(oi_t* sensor_data);
bool edge_of_field_FRight(oi_t* sensor_data);
bool edge_of_field_Right(oi_t* sensor_data);


#endif /* CLIFF_H_ */
