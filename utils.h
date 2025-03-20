#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cnn.h"

Tensor relu(Tensor num);
void relu_mat(Tensor* mat, int elements);
void mat_softmax(Tensor* mat, int elements, Tensor* out);
void softmax_backward(Tensor* mat, int elements, Tensor* out);
void relu_backward(Tensor* mat, int elements);

Tensor* mat_sum(Tensor* mat1, Tensor bias, int rows, int cols);
Tensor* mat_mul(Tensor* mat1, Tensor* mat2, int rows1, int cols1, int cols2);
Tensor* mat_norm(Tensor* mat, int elements);
void norm_backward(Tensor* mat, int elements, Tensor* out);

Tensor neg_log_likelihood(Tensor* mat, int label, int elements, double chain_grad);

Tensor* random_mat(int elements);

void maxpool2d(Tensor* mat, int rows, int cols, int pool_size, int stride, Tensor* result);
void maxpool_backward(Tensor* input, Tensor* output_grad, int rows, int cols, int channels, int pool_size, int stride);

Tensor* conv3d_mul(Tensor* mat, Tensor* kernel, int rows, int cols, int channel, int kernel_size);

Tensor* norm_image(unsigned char* mat, int length);
unsigned char* denorm_image(Tensor* mat, int length);

#endif
