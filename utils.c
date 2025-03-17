#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

double* mat_sum(double* mat1, double bias, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = mat1[i*cols+j] + bias;
        }
    }
    return result;
}

double* mat_mul(double* mat1, double* mat2, int rows1, int cols1, int cols2){
    double* result = (double*)malloc(rows1*cols2*sizeof(double));
    for(int i=0; i<rows1; i++){
        for(int j=0; j<cols2; j++){
            result[i*cols2+j] = 0;
            for(int k=0; k<cols1; k++){
                result[i*cols2+j] += mat1[i*cols1+k]*mat2[k*cols2+j];
            }
        }
    }
    return result;
}

double relu(double num){
    return fmax(0,num);
}

double relu_gr(double num){
    return num>0?1:0;
}

void relu_mat(double* mat, int elements){
    for(int i=0; i<elements; i++){
        mat[i] = relu(mat[i]);
    }
}

void mat_softmax(double* mat, int elements){
    double max_val = -INFINITY;
    for(int i=0; i<elements; i++){
        if(mat[i]>max_val){
            max_val = mat[i];
        }
    }
    double sum = 0;
    for(int i=0; i<elements; i++){
        mat[i] = exp(mat[i]-max_val);
        sum += mat[i];
    }
    for(int i=0; i<elements; i++){
        mat[i] /= sum;
    }
}


double* mat_transpose(double* mat, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[j*rows+i] = mat[i*cols+j];
        }
    }
    return result;
}

double* mat_scalar_mul(double* mat, double scalar, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = mat[i*cols+j]*scalar;
        }
    }
    return result;
}

double* random_mat(int elements){
    double* result = (double*)malloc(elements*sizeof(double));
    for(int i=0; i<elements; i++){
        result[i] = (double)rand()/RAND_MAX;
    }
    return result;
}

// gaussian normalisation
void mat_norm(double* mat, int elements){
    double mean = 0;
    double std = 0;
    for(int i=0; i<elements; i++){
        mean += mat[i];
    }
    mean /= elements;
    for(int i=0; i<elements; i++){
        std += (mat[i]-mean)*(mat[i]-mean);
    }
    std = sqrt(std/elements);
    for(int i=0; i<elements; i++){
        mat[i] = (mat[i]-mean)/std;
    }
}

double* mat_ones(int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = 1;
        }
    }
    return result;
}

// Fixed conv2d_mul with correct indexing
double* conv2d_mul(double* mat, double* kernel, int rows, int cols, int kernel_size){
    double* result = (double*)malloc((rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(double));
    for(int i=0; i<rows-kernel_size+1; i++){
        for(int j=0; j<cols-kernel_size+1; j++){
            result[i*(cols-kernel_size+1)+j] = 0;
            for(int k=0; k<kernel_size; k++){
                for(int l=0; l<kernel_size; l++){
                    result[i*(cols-kernel_size+1)+j] += mat[(i+k)*cols+(j+l)]*kernel[k*kernel_size+l];
                }
            }
        }
    }
    return result;
}

// Fixed conv3d_mul with correct indexing for multi-channel data
double* conv3d_mul(double* mat, double* kernel, int rows, int cols, int channels, int kernel_size){
    double* result = (double*)malloc((rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(double));
    for(int i=0; i<rows-kernel_size+1; i++){
        for(int j=0; j<cols-kernel_size+1; j++){
            result[i*(cols-kernel_size+1)+j] = 0;
            for(int c=0; c<channels; c++){
                for(int k=0; k<kernel_size; k++){
                    for(int l=0; l<kernel_size; l++){
                        result[i*(cols-kernel_size+1)+j] += 
                            mat[c*rows*cols + (i+k)*cols + (j+l)] * 
                            kernel[c*kernel_size*kernel_size + k*kernel_size + l];
                    }
                }
            }
        }
    }
    return result;
}

int* conv2d_mul_int(int* mat, int* kernel, int rows, int cols, int kernel_size){
    int* result = (int*)malloc((rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(int));
    for(int i=0; i<rows-kernel_size+1; i++){
        for(int j=0; j<cols-kernel_size+1; j++){
            result[i*(cols-kernel_size+1)+j] = 0;
            for(int k=0; k<kernel_size; k++){
                for(int l=0; l<kernel_size; l++){
                    result[i*(cols-kernel_size+1)+j] += mat[(i+k)*cols+j+l]*kernel[k*kernel_size+l];
                }
            }
        }
    }
    return result;
}
void maxpool2d(double* mat, int rows, int cols, int pool_size, int stride, double* result) {
    // Validate input parameters
    if (mat == NULL || result == NULL || rows <= 0 || cols <= 0 || 
        pool_size <= 0 || stride <= 0 || pool_size > rows || pool_size > cols) {
        return;  // Early return on invalid input
    }
    
    int new_rows = (rows - pool_size) / stride + 1;
    int new_cols = (cols - pool_size) / stride + 1;
    
    // Ensure that we have at least one output element
    if (new_rows <= 0 || new_cols <= 0) {
        return;
    }
    
    for (int i = 0; i < new_rows; i++) {
        for (int j = 0; j < new_cols; j++) {
            double max_val = -INFINITY;  // Initialize to negative infinity
            for (int k = 0; k < pool_size; k++) {
                for (int l = 0; l < pool_size; l++) {
                    double val = mat[(i * stride + k) * cols + j * stride + l];
                    if (val > max_val) {
                        max_val = val;
                    }
                }
            }
            result[i * new_cols + j] = max_val;
        }
    }
}



double* norm_image(unsigned char* mat, int length){
    double* result = (double*)malloc(length*sizeof(double));
    for(int i=0; i<length; i++){
        result[i] = (double)mat[i]/255;
    }
    return result;
}

unsigned char* denorm_image(double* mat, int length){
    unsigned char* result = (unsigned char*)malloc(length*sizeof(unsigned char));
    for(int i=0; i<length; i++){
        result[i] = (unsigned char)(mat[i]*255);
    }
    return result;
}
