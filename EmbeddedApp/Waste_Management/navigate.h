

#ifndef NAVIGATE_H_
#define NAVIGATE_H_


#include <stdio.h>
#include <math.h>
#include "filter.h"
#include "open_interface.h"
#include "movement.h"
#include "uart.h"
#include "Graph.h"
#include "cliff.h"
#include "scan.h"
#include <ctype.h>


Pose2D cybot_pose;
int north_found;

void find_east();
bumpy receive_and_execute(oi_t*);
int manage_not_complete(oi_t*, Coordinate);
void send_edge(char);
void find_north();
void set_cybot_coords(float, float);
void set_cybot_heading(float);
Coordinate get_interim_coordinate(Coordinate target_coord, Coordinate bot_pos);
void send_interim_coordinate(Coordinate);
void send_bot_pos();
void send_hole_point(oi_t*);
void send_field(Field);
bumpy execute_move(oi_t*, Move, Pose2D*);
bumpy execute_routine(oi_t*, Routine*, Pose2D*);

#endif // NAVIGATE_H_
