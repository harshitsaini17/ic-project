#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "utils.h"
#include "cnn.h"
#include "model.h"




void conv2d(Tensor* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result){

    for(int i=0; i<out_channel; i++){
        Tensor* temp = conv3d_mul(mat, result->weights+i*kernel_size*kernel_size*in_channel, rows, cols, in_channel, kernel_size);
        temp = mat_sum(temp, result->bias[i], rows-kernel_size+1, cols-kernel_size+1);
        for(int j=0; j<(rows-kernel_size+1)*(cols-kernel_size+1); j++){
            result->out[i*(rows-kernel_size+1)*(cols-kernel_size+1)+j] = relu(temp[j]);
        }
        free(temp);
    }

}

void linear(Tensor* mat, int in_features, int out_features, LinearResult* result){
    
    Tensor* temp = mat_mul(mat, result->weights, 1, in_features, out_features);
    
    for(int i=0; i<out_features; i++){
        temp[i].value += result->bias[i].value;
    }

    for(int i=0; i<out_features; i++){
        result->out[i].value = temp[i].value;
    }

    free(temp);
}

void linear_backward(Tensor* mat, int in_features, int out_features, LinearResult* out){
    
    // out weights grad
    for(int i=0; i<out_features; i++){
        for(int j=0; j<in_features; j++){
            out->weights[j*out_features+i].prev_grad = out->weights[j*out_features+i].grad;
            out->weights[j*out_features+i].grad += mat[j].value * (out->out[i].grad-out->bias[i].grad);
        }
    }
    
    
    // mat out grad
    for(int i=0; i<in_features; i++){
        for(int j=0; j<out_features; j++){
            mat[i].prev_grad = mat[i].grad;
            mat[i].grad += out->weights[i*out_features+j].value * (out->out[j].grad-out->bias[j].grad);
        }
    }

    // out bias grad
    for(int i=0; i<out_features; i++){
        out->bias[i].prev_grad = out->bias[i].grad;
        out->bias[i].grad = out->out[i].grad;
    }

}


void conv2d_backward(Tensor* input, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result) {
    // Calculate output dimensions
    int out_rows = rows - kernel_size + 1;
    int out_cols = cols - kernel_size + 1;
    int out_size = out_rows * out_cols;
    
    // Compute ReLU gradient and apply it to the output gradients
    for (int oc = 0; oc < out_channel; oc++) {
        for (int i = 0; i < out_size; i++) {
            int idx = oc * out_size + i;
            // ReLU gradient is 1 if value > 0, otherwise 0
            double relu_grad = result->out[idx].value > 0 ? 1.0 : 0.0;
            result->out[idx].prev_grad = result->out[idx].grad;
            result->out[idx].grad *= relu_grad;
        }
    }
    
    // Compute gradient with respect to bias
    for (int oc = 0; oc < out_channel; oc++) {
        double bias_grad = 0.0;
        for (int i = 0; i < out_size; i++) {
            int idx = oc * out_size + i;
            bias_grad += result->out[idx].grad;
        }
        // Store previous gradient for momentum
        result->bias[oc].prev_grad = result->bias[oc].grad;
        result->bias[oc].grad = bias_grad;
    }
    
    // Compute gradient with respect to weights
    for (int oc = 0; oc < out_channel; oc++) {
        for (int ic = 0; ic < in_channel; ic++) {
            for (int kh = 0; kh < kernel_size; kh++) {
                for (int kw = 0; kw < kernel_size; kw++) {
                    double weight_grad = 0.0;
                    
                    for (int h = 0; h < out_rows; h++) {
                        for (int w = 0; w < out_cols; w++) {
                            int out_idx = oc * out_size + h * out_cols + w;
                            int in_idx = ic * rows * cols + (h + kh) * cols + (w + kw);
                            weight_grad += input[in_idx].value * (result->out[out_idx].grad-result->out[out_idx].prev_grad);
                        }
                    }
                    
                    int weight_idx = oc * in_channel * kernel_size * kernel_size + 
                                    ic * kernel_size * kernel_size + 
                                    kh * kernel_size + kw;
                    result->weights[weight_idx].prev_grad = result->weights[weight_idx].grad;
                    result->weights[weight_idx].grad += weight_grad;
                }
            }
        }
    }
    
    // Compute gradient with respect to input
    for (int i = 0; i < in_channel * rows * cols; i++) {
        input[i].grad = 0.0;  // Reset input gradients
    }
    
    for (int ic = 0; ic < in_channel; ic++) {
        for (int h = 0; h < rows; h++) {
            for (int w = 0; w < cols; w++) {
                double input_grad = 0.0;
                
                for (int oc = 0; oc < out_channel; oc++) {
                    for (int kh = 0; kh < kernel_size; kh++) {
                        for (int kw = 0; kw < kernel_size; kw++) {
                            // Check if this input position contributes to a valid output
                            int out_h = h - kh;
                            int out_w = w - kw;
                            
                            if (out_h >= 0 && out_h < out_rows && 
                                out_w >= 0 && out_w < out_cols) {
                                int out_idx = oc * out_size + out_h * out_cols + out_w;
                                int weight_idx = oc * in_channel * kernel_size * kernel_size + 
                                               ic * kernel_size * kernel_size + 
                                               kh * kernel_size + kw;
                                input_grad += result->weights[weight_idx].value * 
                                             (result->out[out_idx].grad-result->out[out_idx].prev_grad);
                            }
                        }
                    }
                }
                
                int in_idx = ic * rows * cols + h * cols + w;
                input[in_idx].prev_grad = input[in_idx].grad;
                input[in_idx].grad += input_grad;
            }
        }
    }
}

void update_params(ModelResult* result, float learning_rate) {
    // Update conv1 parameters
    int conv1_weights_size = 5*5*3*6;
    for(int i=0; i<conv1_weights_size; i++) {
        result->conv1->weights[i].value -= learning_rate * result->conv1->weights[i].grad;
        // Reset gradient for next batch
        result->conv1->weights[i].grad = 0;
    }
    
    for(int i=0; i<6; i++) {
        result->conv1->bias[i].value -= learning_rate * result->conv1->bias[i].grad;
        result->conv1->bias[i].grad = 0;
    }
    
    // Update conv2 parameters
    int conv2_weights_size = 5*5*6*16;
    for(int i=0; i<conv2_weights_size; i++) {
        result->conv2->weights[i].value -= learning_rate * result->conv2->weights[i].grad;
        result->conv2->weights[i].grad = 0;
    }
    
    for(int i=0; i<16; i++) {
        result->conv2->bias[i].value -= learning_rate * result->conv2->bias[i].grad;
        result->conv2->bias[i].grad = 0;
    }
    
    // Update linear1 parameters
    int linear1_weights_size = 120*16*((IMAGE_HEIGHT-4)/2-4)/2*((IMAGE_WIDTH-4)/2-4)/2;
    for(int i=0; i<linear1_weights_size; i++) {
        result->linear1->weights[i].value -= learning_rate * result->linear1->weights[i].grad;
        result->linear1->weights[i].grad = 0;
    }
    
    for(int i=0; i<120; i++) {
        result->linear1->bias[i].value -= learning_rate * result->linear1->bias[i].grad;
        result->linear1->bias[i].grad = 0;
    }
    
    // Update linear2 parameters
    int linear2_weights_size = 84*120;
    for(int i=0; i<linear2_weights_size; i++) {
        result->linear2->weights[i].value -= learning_rate * result->linear2->weights[i].grad;
        result->linear2->weights[i].grad = 0;
    }
    
    for(int i=0; i<84; i++) {
        result->linear2->bias[i].value -= learning_rate * result->linear2->bias[i].grad;
        result->linear2->bias[i].grad = 0;
    }
    
    // Update linear3 parameters
    int linear3_weights_size = 10*84;
    for(int i=0; i<linear3_weights_size; i++) {
        result->linear3->weights[i].value -= learning_rate * result->linear3->weights[i].grad;
        result->linear3->weights[i].grad = 0;
    }
    
    for(int i=0; i<10; i++) {
        result->linear3->bias[i].value -= learning_rate * result->linear3->bias[i].grad;
        result->linear3->bias[i].grad = 0;
    }
}

void train_model(ImageF* imagesF, int num_epochs, int num_images, int batch_size, float learning_rate) {
    ModelResult* model_result = ModelParams();
    
    for (int epoch = 0; epoch < num_epochs; epoch++) {
        float total_loss = 0.0;
        int num_batches = num_images / batch_size;
        
        for (int batch = 0; batch < num_batches; batch++) {
            Tensor loss;
            loss.value = 0.0;
            loss.grad = 1.0;
            
            for (int i = 0; i < batch_size; i++) {
                int img_index = batch * batch_size + i;
                model(imagesF[img_index].pixels, model_result);
                loss.value += neg_log_likelihood(model_result->out, imagesF[img_index].label, 10, loss.grad/batch_size).value;
                backward(model_result);
            }
            
            update_params(model_result, learning_rate);
            
            total_loss += loss.value;
        }
        
        float avg_loss = total_loss / num_batches;
        printf("Epoch %d, Average Loss: %f\n", epoch + 1, avg_loss);
    }
    
    // Free allocated memory
    free(model_result);
}

