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
    result->pixel = random_mat(IMAGE_SIZE);
    result->conv1 = (ConvResult*)malloc(sizeof(ConvResult));
    result->conv1->weights = random_mat(5*5*3*6);
    result->conv1->bias = random_mat(6);
    result->conv1->out = random_mat(6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4));
    result->conv1norm = random_mat(6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4));
    result->pool1 = random_mat(6*(IMAGE_HEIGHT-4)/2*(IMAGE_WIDTH-4)/2);


    result->conv2 = (ConvResult*)malloc(sizeof(ConvResult));
    result->conv2->weights = random_mat(5*5*6*16);
    result->conv2->bias = random_mat(16);
    result->conv2->out = random_mat(16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4));
    result->conv2norm = random_mat(16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4));
    result->pool2 = random_mat(16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2);


    result->linear1 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear1->weights = random_mat(120*16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2);
    result->linear1->bias = random_mat(120);
    result->linear1->out = random_mat(120);
    result->l1norm = random_mat(120);

    result->linear2 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear2->weights = random_mat(84*120);
    result->linear2->bias = random_mat(84);
    result->linear2->out = random_mat(84);
    result->l2norm = random_mat(84);

    result->linear3 = (LinearResult*)malloc(sizeof(LinearResult));
    result->linear3->weights = random_mat(10*84);
    result->linear3->bias = random_mat(10);
    result->linear3->out = random_mat(10);

    result->out = random_mat(10);
    return result;
}

void model(double* pixels, ModelResult* result){
    for(int i=0; i<IMAGE_SIZE; i++){
        result->pixel[i].value = pixels[i];
    }
    conv2d(result->pixel, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS, 6, 5, result->conv1);
    result->conv1norm = mat_norm(result->conv1->out, 6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4));
    for(int i=0; i<6; i++){
        maxpool2d(result->conv1norm+i*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4), IMAGE_HEIGHT-4, IMAGE_WIDTH-4, 2, 2, result->pool1+i*((IMAGE_HEIGHT-4)/2)*((IMAGE_WIDTH-4)/2));
    }
    
    conv2d(result->pool1, (IMAGE_HEIGHT-4)/2, (IMAGE_WIDTH-4)/2, 6, 16, 5, result->conv2);
    result->conv2norm = mat_norm(result->conv2->out, 16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4));
    // printf conv2norm matrix
    // for(int i=0; i<16; i++){
    //     printf("conv2norm[%d]: ", i);
    //     for(int j=0; j<((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4); j++){
    //         printf("%f ", result->conv2norm[i*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4)+j].value);
    //     }
    //     printf("\n");
    // }
    
    for(int i=0; i<16; i++){
        maxpool2d(result->conv2norm+i*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4),
                  (IMAGE_HEIGHT-4)/2-4, (IMAGE_WIDTH-4)/2-4, 2, 2,
                  result->pool2+i*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2);
    }
    
    
    int flat_size = 16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2;
    
    linear(result->pool2, flat_size, 120, result->linear1);
    relu_mat(result->linear1->out, 120);
    result->l1norm = mat_norm(result->linear1->out, 120);
    
    linear(result->l1norm, 120, 84, result->linear2);
    relu_mat(result->linear2->out, 84);
    result->l2norm = mat_norm(result->linear2->out, 84);
    

    linear(result->l2norm, 84, 10, result->linear3);
    mat_softmax(result->linear3->out, 10, result->out);

}

void backward(ModelResult* result){
    softmax_backward(result->linear3->out, 10, result->out);

    linear_backward(result->l2norm, 84, 10, result->linear3);
    norm_backward(result->linear2->out, 84, result->l2norm);
    relu_backward(result->linear2->out, 84);

    linear_backward(result->l1norm, 120, 84, result->linear2);
    norm_backward(result->linear1->out, 120, result->l1norm);
    relu_backward(result->linear1->out, 120);

    int flat_size = 16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2;

    linear_backward(result->pool2, flat_size, 120, result->linear1);
    maxpool_backward(result->conv2norm, result->pool2, (IMAGE_HEIGHT-4)/2-4, (IMAGE_WIDTH-4)/2-4, 16, 2, 2);
    norm_backward(result->conv2->out, 16*((IMAGE_HEIGHT-4)/2-4)*((IMAGE_WIDTH-4)/2-4), result->conv2norm);

    conv2d_backward(result->pool1, (IMAGE_HEIGHT-4)/2, (IMAGE_WIDTH-4)/2, 6, 16, 5, result->conv2);
    maxpool_backward(result->conv1norm, result->pool1, IMAGE_HEIGHT-4, IMAGE_WIDTH-4,6, 2, 2);
    norm_backward(result->conv1norm, 6*(IMAGE_HEIGHT-4)*(IMAGE_WIDTH-4), result->pool1);

    conv2d_backward(result->pixel, IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS, 6, 5, result->conv1);

}

