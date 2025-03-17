#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include "cnn.h"

Tensor* mat_sum(Tensor* mat1, Tensor bias, int rows, int cols){
    Tensor* result = (Tensor*)malloc(rows*cols*sizeof(Tensor));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j].value = mat1[i*cols+j].value + bias.value;
        }
    }
    return result;
}

Tensor* mat_mul(Tensor* mat1, Tensor* mat2, int rows1, int cols1, int cols2){
    Tensor* result = (Tensor*)malloc(rows1*cols2*sizeof(Tensor));
    for(int i=0; i<rows1; i++){
        for(int j=0; j<cols2; j++){
            result[i*cols2+j].value = 0;
            for(int k=0; k<cols1; k++){
                result[i*cols2+j].value += mat1[i*cols1+k].value*mat2[k*cols2+j].value;
            }
        }
    }
    return result;
}

Tensor relu(Tensor num){
    double r = fmax(0,num.value);
    return (Tensor){r, num.grad};
}

void relu_mat(Tensor* mat, int elements){
    for(int i=0; i<elements; i++){
        mat[i] = relu(mat[i]);
    }
}

void mat_softmax(Tensor* mat, int elements){
    double max_val = -INFINITY;
    for(int i=0; i<elements; i++){
        if(mat[i].value>max_val){
            max_val = mat[i].value;
        }
    }
    double sum = 0;
    for(int i=0; i<elements; i++){
        mat[i].value = exp(mat[i].value-max_val);
        sum += mat[i].value;
    }
    for(int i=0; i<elements; i++){
        mat[i].value /= sum;
    }
}


Tensor* random_mat(int elements){
    Tensor* result = (Tensor*)malloc(elements*sizeof(Tensor));
    for(int i=0; i<elements; i++){
        result[i].value = (double)rand()/RAND_MAX;
    }
    return result;
}

Tensor neg_log_likelihood(Tensor* mat, int label, int elements, double chain_grad){
    Tensor loss = mat[label];
    loss.value = -log(loss.value);

    // mat grad
    for(int i=0; i<elements; i++){
        if(i==label){
            mat[i].grad += -1/(mat[i].value+0.001)*chain_grad;
        }else{
            mat[i].grad += 0;
        }
    }

    return loss;
}

// gaussian normalisation
void mat_norm(Tensor* mat, int elements){
    double mean = 0;
    double std = 0;
    for(int i=0; i<elements; i++){
        mean += mat[i].value;
    }
    mean /= elements;
    for(int i=0; i<elements; i++){
        std += (mat[i].value-mean)*(mat[i].value-mean);
    }
    std = sqrt(std/elements);
    for(int i=0; i<elements; i++){
        mat[i].value = (mat[i].value-mean)/std;
    }
}

// Fixed conv3d_mul with correct indexing for multi-channel data
Tensor* conv3d_mul(Tensor* mat, Tensor* kernel, int rows, int cols, int channels, int kernel_size){
    Tensor* result = (Tensor*)malloc((rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(Tensor));
    for(int i=0; i<rows-kernel_size+1; i++){
        for(int j=0; j<cols-kernel_size+1; j++){
            result[i*(cols-kernel_size+1)+j].value = 0;
            for(int c=0; c<channels; c++){
                for(int k=0; k<kernel_size; k++){
                    for(int l=0; l<kernel_size; l++){
                        result[i*(cols-kernel_size+1)+j].value += 
                        mat[c*rows*cols + (i+k)*cols + (j+l)].value *
                        kernel[c*kernel_size*kernel_size + k*kernel_size + l].value;
                    }
                }
            }
        }
    }
    return result;
}


void maxpool2d(Tensor* mat, int rows, int cols, int pool_size, int stride, Tensor* result) {
    
    int new_rows = (rows - pool_size) / stride + 1;
    int new_cols = (cols - pool_size) / stride + 1;
    for (int i = 0; i < new_rows; i++) {
        for (int j = 0; j < new_cols; j++) {
            double max_val = -INFINITY;
            for (int k = 0; k < pool_size; k++) {
                for (int l = 0; l < pool_size; l++) {
                    double val = mat[(i * stride + k) * cols + j * stride + l].value;
                    if (val > max_val) {
                        max_val = val;
                    }
                }
            }
            result[i * new_cols + j].value = max_val;
        }
    }
}



Tensor* norm_image(unsigned char* mat, int length){
    Tensor* result = (Tensor*)malloc(length*sizeof(Tensor));
    for(int i=0; i<length; i++){
        result[i].value = mat[i]/255;
    }
    return result;
}

unsigned char* denorm_image(Tensor* mat, int length){
    unsigned char* result = (unsigned char*)malloc(length*sizeof(unsigned char));
    for(int i=0; i<length; i++){
        result[i] = (unsigned char)(mat[i].value*255);
    }
    return result;
}
