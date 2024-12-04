

#ifndef NAVIGATE_H_
#define NAGIVATE_H_


#include <stdio.h>
#include <math.h>
#include "filter.h"
#include "open_interface.h"
#include "uart.h"
#include "Graph.h"
#include "cliff.h"
#include "scan.h"
#include <ctype.h>

Coordinate target_coords[16];
target_coords[0].x = 5;
target_coords[0].y = 5;
target_coords[1].x = 240;
target_coords[1].y = 422;
target_coords[2].x = 5;
target_coords[2].y = 422;
target_coords[3].x = 240;
target_coords[3].y = 5;
target_coords[4].x = 5;
target_coords[4].y = 141;
target_coords[5].x = 240;
target_coords[5].y = 282;
target_coords[6].x = 5;
target_coords[6].y = 422;
target_coords[7].x = 240;
target_coords[7].y = 422;
target_coords[8].x = 5;
target_coords[8].y = 282;
target_coords[9].x = 240;
target_coords[9].y = 141;
target_coords[10].x = 5;
target_coords[10].y = 5;
target_coords[11].x = 5;
target_coords[11].y = 422;
target_coords[12].x = 120;
target_coords[12].y = 5;
target_coords[13].x = 240;
target_coords[13].y = 422;
target_coords[14].x = 240;
target_coords[14].y = 5;
target_coords[15].x = 5;
target_coords[15].y = 5;
Pose2D cybot_pose;
int north_found;

void find_east();
bumpy receive_and_execute(oi_t*);
void manage_not_complete(oi_t*);
void send_edge(char);
void find_north();
void set_cybot_coords();
void set_cybot_heading();
Coordinate get_interim_coordinate(Coordinate target_coord, Coordinate bot_pos);
void send_interim_coordinate(Coordinate);
void send_bot_pos();
void send_hole_point(oi_t*);
void send_field(Field);
bumpy execute_move(oi_t*, Move, Pose2D*);
bumpy execute_routine(oi_t*, Routine*, Pose2D*);