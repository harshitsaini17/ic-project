#ifndef MODEL_H
#define MODEL_H

#include "cnn.h"

typedef struct{
    ConvResult* conv1;
    ConvResult* conv2;
    LinearResult* linear1;
    LinearResult* linear2;
    LinearResult* linear3;
    Tensor* out;

}ModelResult;

ModelResult* ModelParams();
void model(double* pixels, ModelResult* result);
void backward(ModelResult* result);


#endif
