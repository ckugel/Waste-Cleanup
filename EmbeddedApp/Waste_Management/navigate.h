/*
 * navigate.h
 *
 *  Created on: Oct 18, 2024
 *      Author: ckugel05
 */

#ifndef NAVIGATE_H_
#define NAVIGATE_H_

#include "movement.h"
#include "Graph.h"
#include "open_interface.h"

#define ASIDE_LENGTH 30
#define FORWARD_LENGTH 45

// plans a route that avoids the pillars on the field
// resulting Routine object needs to be deconstructed
Routine plan_route_after_collision(Pose2D* botLocation, bool hitLeft);

bumpy execute_move(oi_t* sensor, Move move, Pose2D* robotPose);
bumpy execute_routine(oi_t* sensor, Routine* routine, Pose2D* botPose);


#endif /* NAVIGATE_H_ */
