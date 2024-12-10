/*
* Filter.c
*
* Created on: Sep 17 2024
* 	Author: Caleb Kugel
*/


#include "filter.h"
#include "uart.h"

MedianFilter create_filter(uint8_t size) {
  MedianFilter f;
  f.data = create_circular_array(size);
  f.size = size;
  T* arr = calloc(sizeof(T), size);
  f.sorted_array = arr;

  return f;
}

float min(float one, float two) {
    if (one < two) {
        return one;
    }
    return two;
}

T add_new_data_to_filter(MedianFilter* self, T newData) {
    // the index of the data that we want to remove in the sorted array
    short indexOfRemoveSorted;

    // if we are still adding data to the array (it's not full yet)
    if (self->data.size < self->data.capacity) {
        // get the next value we are to add data at in the list

        indexOfRemoveSorted = self->data.size;
    }
    else {
        // should not use get_corrected_size here because get should handle that case automatically
      T remove = get_last(&self->data);
      // should always return a value in the list as the key should always be in it
      // search for the data we want to remove in the sorted array. 
      // the sorted array's size should be the max capacity of the circular array
      indexOfRemoveSorted = bst(self->sorted_array, remove, self->data.capacity);

      // We are no longer overwriting data yayyyy (FROM THIS)
      if (indexOfRemoveSorted < 0 || indexOfRemoveSorted > self->data.capacity) {
          uart_sendChar('!');
          uart_sendChar('a' + indexOfRemoveSorted);
          uart_sendChar('\r');
          uart_sendChar('\n');
       }
    }

    // add new data to circular array
    append(&self->data, newData);
    // replace the data in the sorted array with new data
    self->sorted_array[indexOfRemoveSorted] = newData;

  // swap to the left until the data on the left is > or equal to the new data
  while (indexOfRemoveSorted > 0 &&
          self->sorted_array[indexOfRemoveSorted].distance < self->sorted_array[indexOfRemoveSorted - 1].distance) {
      swap_elements(self->sorted_array, indexOfRemoveSorted, indexOfRemoveSorted - 1);
      indexOfRemoveSorted--;
  }

  // moving window needs to sort data based on if the size or the capacity is less.
  // This is to prevent swapping data into the sorted array in a place we don't
  // expect there to be data yet.
  // Data size will be one higher than expected because we just added some data
  while (indexOfRemoveSorted < min(self->data.capacity - 1, self->data.size - 1) &&
          self->sorted_array[indexOfRemoveSorted].distance > self->sorted_array[indexOfRemoveSorted + 1].distance) {
      swap_elements(self->sorted_array, indexOfRemoveSorted, indexOfRemoveSorted + 1);
      indexOfRemoveSorted++;
  }

  // return the median value in the new data.
  return getMedian(self);
}

T add_new_data_to_filter_raw(MedianFilter* self, uint16_t angle, float distance) {
    Vector v;
    v.distance = distance;
    v.angle = angle;
    return add_new_data_to_filter(self, v);
}

T getMedian(MedianFilter* self) {
    // bruh no get call
    return self->sorted_array[get_corrected_size(&self->data) / 2];
}

void deconstruct_filter(MedianFilter* self) {
    deconstruct_circular_array(&(self->data));
    free(self->sorted_array);
    // free(self->sorted_array);
}

void swap_elements(T* array, char index1, char index2) {
    T x = array[index1];
    array[index1] = array[index2];
    array[index2] = x;
}
