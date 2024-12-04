#ifndef SCAN_H
#define SCAN_H

#include "Graph.h"
#include "filter.h"
#include "LinearInterp.h"
#include "movement.h" 

const uint16_t angles = 180;
#define ANGLE_SIZE 180
const uint8_t STEP = 1;
const uint16_t undefined_dist = 0xFFFF;

Field scan();

#endif
