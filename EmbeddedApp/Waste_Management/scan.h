#ifndef SCAN_H
#define SCAN_H

#include "Graph.h"
#include "filter.h"
#include "LinearInterp.h"
#include "movement.h" 
#include "servo.h"
#include "adc.h"
#include "ping.h"


#define ANGLE_SIZE 180
uint16_t angles;
uint8_t STEP;
uint16_t undefined_dist;

Field scan();

#endif
