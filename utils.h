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

double* random_mat(int rows, int cols);
double* mat_ones(int rows, int cols);

#endif
