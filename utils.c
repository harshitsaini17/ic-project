#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
double relu(double num){
    return fmax(0,num);
}

double relu_gr(double num){
    return num>0?1:0;
}


double* mat_sum(double* mat1, double* mat2, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = mat1[i*cols+j] + mat2[i*cols+j];
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

double* random_mat(int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = (double)rand()/(double)(RAND_MAX);
        }
    }
    return result;
}

double* mat_relu(double* mat, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = relu(mat[i*cols+j]);
        }
    }
    return result;
}

double* mat_relu_gr(double* mat, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = relu_gr(mat[i*cols+j]);
        }
    }
    return result;
}

// gaussian normalisation
double* mat_norm(double* mat, int rows, int cols){
    double* result = (double*)malloc(rows*cols*sizeof(double));
    double mean = 0;
    double std = 0;
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            mean += mat[i*cols+j];
        }
    }
    mean = mean/(rows*cols);
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            std += pow(mat[i*cols+j]-mean,2);
        }
    }
    std = sqrt(std/(rows*cols));
    for(int i=0; i<rows; i++){
        for(int j=0; j<cols; j++){
            result[i*cols+j] = (mat[i*cols+j]-mean)/std;
        }
    }
    return result;
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


double* conv2d_mul(double* mat, double* kernel, int rows, int cols, int kernel_size){
    double* result = (double*)malloc((rows-kernel_size+1)*(cols-kernel_size+1)*sizeof(double));
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


