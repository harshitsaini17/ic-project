#ifndef MODEL_H
#define MODEL_H

#include "cnn.h"

typedef struct{
    ConvResult* conv1;
    ConvResult* conv2;
    LinearResult* linear1;
    LinearResult* linear2;
    LinearResult* linear3;
    double* out;

}ModelResult;

void model(double* pixels, ModelResult* result);
ModelResult* ModelParams();


#endif
