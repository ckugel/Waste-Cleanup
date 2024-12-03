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
    return value > EDGE_THRESHOLD_MIN && value < EDGE_THRESHOLD_MAX;
}

bool readingTarget(uint16_t value) {
    return value > TARGET_THRESHOLD_MIN;
}

uint8_t getHoleTouching(oi_t* sensor) {
    return readingHole(sensor->cliffLeftSignal) | (readingHole(sensor->cliffFrontLeftSignal) << 1) | (readingHole(sensor->cliffFrontRightSignal) << 2) | (readingHole(sensor->cliffRightSignal) << 3);
}

uint8_t getEdgeTouching(oi_t* sensor) {
    return readingEdge(sensor->cliffLeftSignal) | (readingEdge(sensor->cliffFrontLeftSignal) << 1) | (readingEdge(sensor->cliffFrontRightSignal) << 2) | (readingEdge(sensor->cliffRightSignal) << 3);
}


uint8_t getTargetTouching(oi_t* sensor){
    return readingTarget(sensor->cliffLeftSignal) | (readingTarget(sensor->cliffFrontLeftSignal) << 1) | (readingTarget(sensor->cliffFrontRightSignal) << 2) | (readingTarget(sensor->cliffRightSignal) << 3);
}


