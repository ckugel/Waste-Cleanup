/*
 * cliff.h
 *
 *  Created on: Nov 12, 2024
 *      Author: ckugel05
 */

#ifndef CLIFF_H_
#define CLIFF_H_

#include "open_interface.h"

#define HOLE_THRESHOLD_MAX 100 //reads 0 on black
#define EDGE_THRESHOLD_MIN 2500 // edge value reads between 2500 and 2700
#define EDGE_THRESHOLD_MAX 2800 // edge value reads between 2500 and 2700
#define TARGET_THRESHOLD_MIN 2800 // reading on glossy green folder 2800
#define FLOOR_THRESHOLD_MIN 2000 // reading on floor tile between 1000 and 1700
#define FLOOR_THRESHOLD_MAX 2450 // reading on floor tile between 1000 and 1700

/**
 * Predcate that returns whether a cliff sensor's signal is reading a hole
 * @param value a cliff sensor's signal
 * @return whether the sensor value means we are reading a hole
 */ 
bool readingHole(uint16_t value);

/**
 * Predicate that returns whether a cliff sensor's signal is reading an edge
 * @param value the value read by the cliff sensor's signal
 * @return whether the sensor calue means we are reading an edge
 */ 
bool readingEdge(uint16_t value);

bool readingTarget(uint16_t value);

/**
 * Determines which cliff sensor is reading a hole.
 * Assumes that the sensor has been updated already.
 * @param sensor OI_T sensor from cyBot
 * @return which sensor is reading the hole. Methodology -> {
 *  0 for none
 *  left has a value of 1
 *  front left has a value of 2
 *  front right has a value of 4
 *  right has a value of 8
 *  These values get combined into a single value with addition.
 *  For instance if front left and left both see a hole we return 1 + 2 = 3
 * }
 */ 
uint8_t getHoleTouching(oi_t* sensor);

/**
 * Determines which cliff sensor is reading an edge
 * Assumes that the sensor has been updated already
 * @param sensor OI_T sensor from cyBot
 * @return which sensor is reading the edge.Methodology -> {
 *  0 for none
 *  left has a value of 1
 *  front left has a value of 2
 *  front right has a value of 4
 *  right has a value of 8
 *  These values get combined into a single value with addition.
 *  For instance if front left and left both see a hole we return 1 + 2 = 3
 * } 
 */ 
uint8_t getEdgeTouching(oi_t* sensor);

uint8_t getTargetTouching(oi_t* sensor);

#endif /* CLIFF_H_ */
