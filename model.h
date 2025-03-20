#ifndef MODEL_H
#define MODEL_H

#include "cnn.h"


ModelResult* ModelParams();
void model(double* pixels, ModelResult* result);
void backward(ModelResult* result);


#endif
