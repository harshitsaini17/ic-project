#ifndef CNN_H

#define CNN_H

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define CHANNELS 3
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS)
#define NUM_IMAGES 10000
#define BATCH_SIZE 100


typedef struct{
    double value;
    double grad;
    double prev_grad;
} Tensor;

typedef struct {
    Tensor* weights;
    Tensor* bias;
    Tensor* out;
} ConvResult;

typedef struct {
    Tensor* weights;
    Tensor* bias;
    Tensor* out;
} LinearResult;

typedef struct {
    int label;
    double pixels[IMAGE_SIZE];
} ImageF;

typedef struct{
    Tensor* pixel;
    ConvResult* conv1;
    Tensor* conv1norm;
    Tensor* pool1;
    ConvResult* conv2;
    Tensor* conv2norm;
    Tensor* pool2;
    LinearResult* linear1;
    Tensor* l1norm;
    LinearResult* linear2;
    Tensor* l2norm;
    LinearResult* linear3;
    Tensor* out;

}ModelResult;

void conv2d(Tensor* mat, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result);
void linear(Tensor* mat, int in_features, int out_features, LinearResult* result);
void linear_backward(Tensor* mat, int in_features, int out_features, LinearResult* out);
void conv2d_backward(Tensor* input, int rows, int cols, int in_channel, int out_channel, int kernel_size, ConvResult* result);
void update_params(ModelResult* result, float learning_rate);
void train_model(ImageF* imagesF, int num_epochs, int num_images, int batch_size, float learning_rate);
#endif


