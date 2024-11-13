/*
 * cliff.c
 *
 *  Created on: Nov 12, 2024
 *      Author: ckugel05
 */


#include "cliff.h"

bool readingHole(uint16_t value) {
    return value < HOLE_THRESHOLD_MAX;
}

bool readingEdge(uint16_t value) {
    return value > EDGE_THRESHOLD_MIN;
}

uint8_t getHoleTouching(oi_t* sensor) {
    return readingHole(sensor->cliffLeftSignal) | (readingHole(sensor->cliffFrontLeftSignal) << 1) | (readingHole(sensor->cliffFrontRight) << 2) | (readingHole(sensor->cliffRightSignal) << 3);
}

uint8_t getEdgeTouching(oi_t* sensor) {
    return readingEdge(sensor->cliffLeftSignal) | (readingEdge(sensor->cliffFrontLeftSignal) << 1) | (readingEdge(sensor->cliffFrontRight) << 2) | (readingEdge(sensor->cliffRightSignal) << 3);
}

