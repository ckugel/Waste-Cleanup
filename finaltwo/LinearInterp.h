//
// Created by calebk on 10/2/24.
//

#ifndef LINEARINTERP_H
#define LINEARINTERP_H


typedef struct {
    float *measured;
    float *real;
    unsigned char size;
} Lerp;

Lerp create_lerp(float* measuredValues, float* realValues, unsigned char size);

void deconstruct_lerp(Lerp* self);

// take in a measured value and return an approximation of the real
float getReal(Lerp* self, float measured);

unsigned char find_index_range_binsearch(Lerp* self, float measurement);

float calculateSlope(Lerp* self, unsigned char low, unsigned char high);

#endif //LINEARINTERP_H
