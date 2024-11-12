//
// Created by calebk on 10/2/24.
//



#include <stdlib.h>
#include "LinearInterp.h"

Lerp create_lerp(float* measuredValues, float* realValues, unsigned char size) {
    /*float* measured = malloc(size * sizeof(float));
    float* real = malloc(size * sizeof(float));

    memcpy(measured, measuredValues, size * sizeof(float));
    memcpy(real, realValues, size * sizeof(float));
    */

    return (Lerp) {measuredValues, realValues, size};
}

void deconstruct_lerp(Lerp* self) {
    /*
    free(self->measured);
    free(self->real);

*/}

// measured value on IR sensor should be 3 - rawData
//TODO: filter out measurements < 0.5
float getReal(Lerp* self, float measured) {
    // unsigned char head = 0;

    unsigned char lower = find_index_range_binsearch(self, measured);
    if (lower == self->size - 1) {
        //ISSUE
        return calculateSlope(self, lower - 1, lower) * measured;
    }
    else if (lower == 255) {
        // ISSUE
        return calculateSlope(self, 0, 1) * measured;
    }
    else {
        // normal case
        // slope calculation is (y2 - y1) / (x2 - x1)
        // slope * measured should give us an approximation for real

        //return self->real[lower] + ((measured - self->measured[lower]) * (self->real[lower + 1] - self->real[lower]) / (self->measured[lower + 1] - self->measured[lower]));
           return (self->real[lower]) + ((calculateSlope(self, lower, lower + 1)) * (measured - self->measured[lower]));
        // return calculateSlope(self, lower, lower + 1) * measured;
    }
}

float calculateSlope(Lerp* self, unsigned char low, unsigned char high) {
    return (self->real[high] - self->real[low]) / (self->measured[high] - self->measured[low]);
}


unsigned char find_index_range_binsearch(Lerp* self, float measurement) {
    unsigned char low = 0;
    unsigned char high = self->size - 1;
    unsigned char mid;

    if (measurement < self->measured[0]) {
        return 255;
    }

    while (low <= high) {
        mid = (low + high) / 2;

        // always truncates so there should be a value greater than mid in the memory at all times (old high value)
        if (measurement >= self->measured[mid] && measurement < self->measured[mid + 1]) {
            // HIT
            return mid;
        }
        else if (measurement < self->measured[mid]) {
            high = mid - 1;
        }
        else {
            low = mid + 1;
        }
    }

    // is the low is the last element in the array return that as no suitable element was found
    // otherwise if you hit this point in execution and low is not the last element then something
    // went wrong and you need to return 255
    if (low == self->size - 1) {
        return low;
    }
    else {
        return 255;
    }
}


