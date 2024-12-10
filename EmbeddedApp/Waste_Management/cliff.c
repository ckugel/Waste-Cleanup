/*
 * cliff.c
 *
 *  Created on: Nov 12, 2024
 *      Author: ckugel05
 */


#include "cliff.h"

/**
 * Checks if the given sensor value indicates a hole.
 *
 * A hole is detected when the sensor value is below the maximum threshold for holes.
 * 
 * @param value The signal value from the cliff sensor.
 * @return true if the value indicates a hole, otherwise false.
 */
bool readingHole(uint16_t value) {
    return value < HOLE_THRESHOLD_MAX;
}

/**
 * Checks if the given sensor value indicates an edge.
 *
 * An edge is detected when the signal value falls within a specific range.
 * @return true if the value indicates an edge, otherwise false.
 */

bool readingEdge(uint16_t value) {
    return value > EDGE_THRESHOLD_MIN && value < EDGE_THRESHOLD_MAX;
}

/**
 * Checks if the given sensor value indicates a target (e.g., a marker).
 *
 * A target is detected when the sensor value exceeds the minimum threshold for targets.
 * @return true if the value indicates a target, false otherwise.
 */

bool readingTarget(uint16_t value) {
    return value > TARGET_THRESHOLD_MIN;
}

/**
 * Determine which cliff sensors detect a hole.
 *
 * Each cliff sensor is checked to see if it indicates a hole. The result is a
 * bitwise OR of all detected holes:
 *     Bit 0: Left cliff sensor
 *     Bit 1: Front-left cliff sensor
 *     Bit 2: Front-right cliff sensor
 *     Bit 3: Right cliff sensor
 * 
 * @param sensor Pointer to the CyBot's sensor data structure.
 * @return A bitfield representing which sensors detect a hole.
 */

uint8_t getHoleTouching(oi_t* sensor) {
    return readingHole(sensor->cliffLeftSignal) | (readingHole(sensor->cliffFrontLeftSignal) << 1) | (readingHole(sensor->cliffFrontRightSignal) << 2) | (readingHole(sensor->cliffRightSignal) << 3);
}

/**
 * Determine which cliff sensors detect an edge.
 * @return A bitfield representing which sensors detect an edge.
 */

uint8_t getEdgeTouching(oi_t* sensor) {
    return readingEdge(sensor->cliffLeftSignal) | (readingEdge(sensor->cliffFrontLeftSignal) << 1) | (readingEdge(sensor->cliffFrontRightSignal) << 2) | (readingEdge(sensor->cliffRightSignal) << 3);
}

/**
 * Determine which cliff sensors detect a target.
 * @return A bitfield representing which sensors detect a target.
 */


uint8_t getTargetTouching(oi_t* sensor){
    return readingTarget(sensor->cliffLeftSignal) | (readingTarget(sensor->cliffFrontLeftSignal) << 1) | (readingTarget(sensor->cliffFrontRightSignal) << 2) | (readingTarget(sensor->cliffRightSignal) << 3);
}
