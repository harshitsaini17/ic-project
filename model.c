#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "model.h"
#include "cnn.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define CHANNELS 3
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS)


ModelResult* ModelParams(){
    ModelResult* result = (ModelResult*)malloc(sizeof(ModelResult));
    result->conv1 = (ConvResult*)malloc(sizeof(ConvResult));
    result->conv1->weights = random_mat(5*5*3*6);
    result->conv1->bias = random_mat(6);
    result->conv1->out = (Tensor*)malloc(6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4)*sizeof(Tensor));
    for(int i=0; i<6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4); i++){
        result->conv1->out[i].grad = 0;
    }

    result->conv2 = (ConvResult*)malloc(sizeof(ConvResult));
    result->conv2->weights = random_mat(5*5*6*16);
    result->conv2->bias = random_mat(16);
    result->conv2->out = (Tensor*)malloc(16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4)*sizeof(Tensor));
    for(int i=0; i<16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4); i++){
        result->conv2->out[i].grad = 0;
    }


    result->linear1 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear1->weights = random_mat(120*16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2);
    result->linear1->bias = random_mat(120);
    result->linear1->out = (Tensor*)malloc(120*sizeof(Tensor));
    for(int i=0; i<120; i++){
        result->linear1->out[i].grad = 0;
    }

    result->linear2 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear2->weights = random_mat(84*120);
    result->linear2->bias = random_mat(84);
    result->linear2->out = (Tensor*)malloc(84*sizeof(Tensor));
    for(int i=0; i<84; i++){
        result->linear2->out[i].grad = 0;
    }

    result->linear3 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear3->weights = random_mat(10*84);
    result->linear3->bias = random_mat(10);

    result->linear3->out = (Tensor*)malloc(10*sizeof(Tensor));
    for(int i=0; i<10; i++){
        result->linear3->out[i].grad = 0;
    }
    result->out = (Tensor*)malloc(10*sizeof(Tensor));
    for(int i=0; i<10; i++){
        result->out[i].grad = 0;
    }
    return result;
}

void model(double* pixels, ModelResult* result){
    Tensor* pixels_tensor = (Tensor*)malloc(IMAGE_SIZE*sizeof(Tensor));
    for(int i=0; i<IMAGE_SIZE; i++){
        pixels_tensor[i].value = pixels[i];
    }
    conv2d(pixels_tensor, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS, 6, 5, result->conv1);
    mat_norm(result->conv1->out, 6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4));
    Tensor* pool1 = (Tensor*)malloc(6*((IMAGE_HEIGHT-4)/2)*((IMAGE_WIDTH-4)/2)*sizeof(Tensor));
    for(int i=0; i<6; i++){
        maxpool2d(result->conv1->out+i*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4), IMAGE_HEIGHT-4, IMAGE_WIDTH-4, 2, 2, pool1+i*((IMAGE_HEIGHT-4)/2)*((IMAGE_WIDTH-4)/2));
    }
    
    conv2d(pool1, (IMAGE_HEIGHT-4)/2, (IMAGE_WIDTH-4)/2, 6, 16, 5, result->conv2);
    mat_norm(result->conv2->out, 16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4));
    Tensor* pool2 = (Tensor*)malloc(16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2*sizeof(Tensor));
    for(int i=0; i<16; i++){
        maxpool2d(result->conv2->out+i*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2), (IMAGE_HEIGHT-4)/2-4, (IMAGE_WIDTH-4)/2-4, 2, 2, pool2+i*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2);
    }
    
    int flat_size = 16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2;
    
    linear(pool2, flat_size, 120, result->linear1);
    relu_mat(result->linear1->out, 120);
    mat_norm(result->linear1->out, 120);
    
    linear(result->linear1->out, 120, 84, result->linear2);
    relu_mat(result->linear2->out, 84);
    mat_norm(result->linear2->out, 84);
    

    linear(result->linear2->out, 84, 10, result->linear3);
    mat_softmax(result->linear3->out, 10, result->out);

    free(pool1);
    free(pool2);
    free(pixels_tensor);
}

void backward(ModelResult* result){

    softmax_backward(result->linear3->out, 10, result->out);
    linear_backward(result->linear2->out, 84, 10, result->linear3);
}

