#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "cnn.h"
#include "model.h"

void conv2d(Tensor* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result){

    for(int i=0; i<out_channel; i++){
        Tensor* temp = conv3d_mul(mat, result->weights+i*kernel_size*kernel_size*in_channel, rows, cols, in_channel, kernel_size);
        temp = mat_sum(temp, result->bias[i], rows-kernel_size+1, cols-kernel_size+1);
        for(int j=0; j<(rows-kernel_size+1)*(cols-kernel_size+1); j++){
            result->out[i*(rows-kernel_size+1)*(cols-kernel_size+1)+j] = relu(temp[j]);
        }
        free(temp);
    }

}

void linear(Tensor* mat, int in_features, int out_features, LinearResult* result){
    
    Tensor* temp = mat_mul(mat, result->weights, 1, in_features, out_features);
    
    for(int i=0; i<out_features; i++){
        temp[i].value += result->bias[i].value;
    }

    for(int i=0; i<out_features; i++){
        result->out[i].value = temp[i].value;
    }

    free(temp);
}

