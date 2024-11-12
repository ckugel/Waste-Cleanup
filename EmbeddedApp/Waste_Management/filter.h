/*
* Filter.h
*
* Created on: Sep 17 2024
* 	Author: ckugel05
*/

#ifndef FILTER_H
#define FILTER_H


#include "caleb_array.h"

typedef struct {
  CArray data;
  T* sorted_array;
  uint8_t size;
} MedianFilter;

// create a new median filter
MedianFilter create_filter(uint8_t size);

// log new data
// return the median of the past calculations
T add_new_data_to_filter_raw(MedianFilter* self, uint16_t angle, float distance);

// put a new vector in
T add_new_data_to_filter(MedianFilter* self, T newData);

// Return the median of the past calculations
T getMedian(MedianFilter* self);

void deconstruct_filter(MedianFilter* self);

void swap_elements(T* array, char index1, char index2);

float min(float one, float two);

#endif /* FILTER_H */
