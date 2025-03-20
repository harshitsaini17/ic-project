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

void relu_backward(Tensor* mat, int elements){
    for(int i=0; i<elements; i++){
        if(mat[i].value<=0){
            mat[i].grad = 0;
        }
    }
}

void mat_softmax(Tensor* mat, int elements, Tensor* out){
    double max_val = -INFINITY;
    for(int i=0; i<elements; i++){
        if(mat[i].value>max_val){
            max_val = mat[i].value;
        }
    }
    double sum = 0;
    for(int i=0; i<elements; i++){
        out[i].value = exp(mat[i].value-max_val);
        sum += out[i].value;
    }
    for(int i=0; i<elements; i++){
        out[i].value /= sum;
    }
}

void softmax_backward(Tensor* mat, int elements, Tensor* out){
    // softmax formula
    // s_i = e^(x_i-x_max)/sum(e^(x_i-x_max))
    /*
        s_i = chain[i].value
        x_k = mat[k].value
        ∂s_i/∂x_k = {
                        s_i(1-s_i)          if i=k
                        -s_i×s_k            if i≠k
                    }
    simplified:
        ∂L/∂x_j = s_j(∂L/∂s_j - Σ_i (∂L/∂s_i)s_i)
    */
   double sum = 0;
   for(int i=0; i<elements; i++){
        sum += out[i].value * out[i].grad;
   }
   
   for(int j=0; j<elements; j++){
        mat[j].prev_grad = mat[j].grad;
        mat[j].grad += out[j].value * (out[j].grad - sum);
   }
    
}

Tensor* random_mat(int elements){
    Tensor* result = (Tensor*)malloc(elements*sizeof(Tensor));
    for(int i=0; i<elements; i++){
        result[i].value = (double)rand()/RAND_MAX;
        result[i].grad = 0;
        result[i].prev_grad = 0;
    }
    return result;
}

Tensor neg_log_likelihood(Tensor* mat, int label, int elements, double chain_grad){
    Tensor loss = mat[label];
    loss.value = -log(loss.value);

    // mat grad
    for(int i=0; i<elements; i++){
        mat[i].prev_grad = mat[i].grad;
        if(i==label){
            mat[i].grad += -1/(mat[i].value+0.001)*chain_grad;
        }else{
            mat[i].grad += 0;
        }
    }

    return loss;
}

// gaussian normalisation
Tensor* mat_norm(Tensor* mat, int elements){
    Tensor* result = (Tensor*)malloc(elements*sizeof(Tensor));
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
        result[i].value = (mat[i].value-mean)/std;
    }
    return result;
}

void norm_backward(Tensor* mat, int elements, Tensor* out){
    /*
    formula:
        n_i = (mat[i].value - mean)/std;
        mean = Σmat[i].value/elements
        std = sqrt(Σ(mat[i].value-mean)^2/elements)
    gradient:
        ∂n_i/∂mat[i].value = ((N-1)-n_i^2)/(N*std)
    */
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
        mat[i].prev_grad = mat[i].grad;
        mat[i].grad += (elements-1-out[i].value*out[i].value)/(elements*std)*(out[i].grad-out[i].prev_grad);
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

void maxpool_backward(Tensor* input, Tensor* output_grad, int rows, int cols, int channels, int pool_size, int stride) {
    int out_rows = (rows - pool_size) / stride + 1;
    int out_cols = (cols - pool_size) / stride + 1;
    for (int c = 0; c < channels; c++) {
        for (int i = 0; i < out_rows; i++) {
            for (int j = 0; j < out_cols; j++) {
                int out_idx = i * out_cols + j;
                double max_val = -INFINITY;
                int max_i = 0, max_j = 0;
                
                for (int k = 0; k < pool_size; k++) {
                    for (int l = 0; l < pool_size; l++) {
                        int r = i * stride + k;
                        int col = j * stride + l;
                        
                        if (r < rows && col < cols) {
                            int in_idx = r * cols + col;
                            if (input[c * rows * cols + in_idx].value > max_val) {
                                max_val = input[c * rows * cols + in_idx].value;
                                max_i = r;
                                max_j = col;
                            }
                        }
                    }
                }
                int max_idx = max_i * cols + max_j;
                input[c * rows * cols + max_idx].prev_grad = input[c * rows * cols + max_idx].grad;
                input[c * rows * cols + max_idx].grad += (output_grad[c * out_rows * out_cols + out_idx].grad - output_grad[c * out_rows * out_cols + out_idx].prev_grad);
            }
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


