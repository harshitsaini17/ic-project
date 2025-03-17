#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double relu(double num);
double relu_gr(double num);
void relu_mat(double* mat, int elements);
void mat_softmax(double* mat, int elements);

double* mat_sum(double* mat1, double bias, int rows, int cols);
double* mat_mul(double* mat1, double* mat2, int rows1, int cols1, int cols2);
double* mat_transpose(double* mat, int rows, int cols);
double* mat_scalar_mul(double* mat, double scalar, int rows, int cols);
void mat_norm(double* mat, int elements);

double* random_mat(int elements);
double* mat_ones(int rows, int cols);

double* conv2d_mul(double* mat, double* kernel, int rows, int cols, int kernel_size);
int* conv2d_mul_int(int* mat, int* kernel, int rows, int cols, int kernel_size);
void maxpool2d(double* mat, int rows, int cols, int pool_size, int stride, double* result);

double* conv3d_mul(double* mat, double* kernel, int rows, int cols, int channel, int kernel_size);

double* norm_image(unsigned char* mat, int length);
unsigned char* denorm_image(double* mat, int length);

#endif
