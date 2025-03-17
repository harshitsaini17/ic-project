#ifndef CNN_H
#define CNN_H

typedef struct{
    double value;
    double grad;
} Tensor;

typedef struct {
    Tensor* weights;
    Tensor* bias;
    Tensor* out;
} ConvResult;

typedef struct {
    Tensor* weights;
    Tensor* bias;
    Tensor* out;
} LinearResult;

void conv2d(Tensor* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result);
void linear(Tensor* mat, int in_features, int out_features, LinearResult* result);

#endif


