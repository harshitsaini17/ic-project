#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

double relu(double num);
void relu_mat(double* mat, int elements);
void mat_softmax(double* mat, int elements);

double* mat_sum(double* mat1, double bias, int rows, int cols);
double* mat_mul(double* mat1, double* mat2, int rows1, int cols1, int cols2);
void mat_norm(double* mat, int elements);

double* random_mat(int elements);

void maxpool2d(double* mat, int rows, int cols, int pool_size, int stride, double* result);

double* conv3d_mul(double* mat, double* kernel, int rows, int cols, int channel, int kernel_size);

double* norm_image(unsigned char* mat, int length);
unsigned char* denorm_image(double* mat, int length);

#endif
