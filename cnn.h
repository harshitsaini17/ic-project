#ifndef CNN_H
#define CNN_H

typedef struct {
    double* kernel;
    double* bias;
    double* out;
} ConvResult;

ConvResult* conv2d(double* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size);

#endif


