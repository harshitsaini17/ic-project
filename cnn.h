#ifndef CNN_H
#define CNN_H

typedef struct {
    double* weights;
    double* bias;
    double* out;
} ConvResult;

typedef struct {
    double* weights;
    double* bias;
    double* out;
} LinearResult;

void conv2d(double* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result);
void linear(double* mat, int in_features, int out_features, LinearResult* result);

#endif


