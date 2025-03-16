#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double relu(double num);
double relu_gr(double num);

double* mat_sum(double* mat1, double* mat2, int rows, int cols);
double* mat_mul(double* mat1, double* mat2, int rows1, int cols1, int cols2);
double* mat_transpose(double* mat, int rows, int cols);
double* mat_scalar_mul(double* mat, double scalar, int rows, int cols);
double* mat_relu(double* mat, int rows, int cols);
double* mat_relu_gr(double* mat, int rows, int cols);
double* mat_norm(double* mat, int rows, int cols);

double* random_mat(int elements);
double* mat_ones(int rows, int cols);

double* conv2d_mul(double* mat, double* kernel, int rows, int cols, int kernel_size);
int* conv2d_mul_int(int* mat, int* kernel, int rows, int cols, int kernel_size);
double* maxpool2d(double* mat, int rows, int cols, int pool_size, int stride);

double* conv3d_mul(double* mat, double* kernel, int rows, int cols, int channel, int kernel_size);

double* norm_image(unsigned char* mat, int length);
unsigned char* denorm_image(double* mat, int length);

#endif
