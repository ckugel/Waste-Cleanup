/*
* Filter.c
*
* Created on: Sep 17 2024
* 	Author: Caleb Kugel
*/


#include "filter.h"
#include "uart.h"


/**
 * @brief Creates a median filter structure.
 *
 * Initializes a circular array for data storage and a sorted array for median calculations.
 *
 * @param size The maximum number of elements the filter can hold.
 * @return MedianFilter The initialized median filter.
 */
MedianFilter create_filter(uint8_t size) {
  MedianFilter f;
  f.data = create_circular_array(size);  // Initialize the circular array for data storage.
  f.size = size;                          // Set the filter size.
  T* arr = calloc(sizeof(T), size);      // Allocate memory for the sorted array.
  f.sorted_array = arr;                  // Assign the allocated memory to the filter's sorted array.

  return f;      // Return the initialized filter.
}

// Returns the smaller of two float values.
float min(float one, float two) {
    if (one < two) {
        return one;
    }
    return two;
}

/**
 * Adds a new data point to the median filter and returns the median value.
 *
 * Updates the circular array and sorted array to include the new data,
 * ensuring that the sorted array remains sorted.
 *
 * @param self Pointer to the median filter.
 * @param newData The new data point to add.
 * @return T The median value after adding the new data.
 */
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

/**
 * Adds raw data (angle and distance) to the median filter.
 *
 * Constructs a `Vector` from the raw data and passes it to `add_new_data_to_filter`.
 * @param angle The angle value of the new data.
 * @param distance The distance value of the new data.
 * @return T The median value after adding the new data.
 */
T add_new_data_to_filter_raw(MedianFilter* self, uint16_t angle, float distance) {
    Vector v;
    v.distance = distance; // Set the distance in the vector.
    v.angle = angle;      // Set the angle in the vector.
    return add_new_data_to_filter(self, v);    // Add the vector to the filter.
}

/**
 *  Retrieves the median value from the median filter.
 *
 * The median is the middle element of the sorted array.
 *
 * @param self Pointer to the median filter.
 * @return T The median value.
 */
T getMedian(MedianFilter* self) {
    // Retrieve the median from the middle of the sorted array.
    return self->sorted_array[get_corrected_size(&self->data) / 2];
}

// Deconstructs a median filter and frees its allocated memory.
void deconstruct_filter(MedianFilter* self) {
    deconstruct_circular_array(&(self->data));  // Free the sorted array.
    free(self->sorted_array);      // Free the sorted array.
    // free(self->sorted_array);
}


/**
 * Swaps two elements in an array.
 *
 * Swaps the elements at the specified indices.
 *
 * @param array The array containing the elements to swap.
 * @param index1 Index of the first element.
 * @param index2 Index of the second element.
 */
void swap_elements(T* array, char index1, char index2) {
    T x = array[index1];     // Temporarily store the value at index1.
    array[index1] = array[index2];    // Move the value at index2 to index1.
    array[index2] = x;      // Move the temporarily stored value to index2.
}
