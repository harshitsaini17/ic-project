#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
typedef struct {
    double* kernel;
    double* bias;
    double* out;
} ConvResult;

ConvResult* conv2d(double* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size){
    ConvResult* result = (ConvResult*)malloc(sizeof(ConvResult));
    result->kernel = random_mat(out_channel*kernel_size*kernel_size*in_channel);
    result->bias = random_mat(out_channel);
    result->out = (double*)malloc(out_channel*(rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(double));

    for(int i=0; i<out_channel; i++){
        double* temp = conv3d_mul(mat, result->kernel+i*kernel_size*kernel_size*in_channel, rows, cols, in_channel, kernel_size);
        temp = mat_sum(temp, result->bias+i, rows-kernel_size+1, cols-kernel_size+1);
        for(int j=0; j<(rows-kernel_size+1)*(cols-kernel_size+1); j++){
            result->out[i*(rows-kernel_size+1)*(cols-kernel_size+1)+j] = relu(temp[j]);
        }
    }

    return result;
}

