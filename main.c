#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "bmp.h"
#include "cnn.h"
#include "model.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define CHANNELS 3
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS)
#define NUM_IMAGES 10000
#define BATCH_SIZE 100

typedef struct {
    unsigned char label;
    unsigned char pixels[IMAGE_SIZE];
} ImageC;


int main() {
    FILE *file;
    
    file = fopen("dataset/data_batch_1.bin", "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    
    // Dataset Preprocessing
    ImageF *imagesF = (ImageF*)malloc(NUM_IMAGES * sizeof(ImageF));
    ImageC *imagesC = (ImageC*)malloc(BATCH_SIZE * sizeof(ImageC));

    for (int i = 0; i < NUM_IMAGES; i += BATCH_SIZE) {
        int current_batch_size = (i + BATCH_SIZE <= NUM_IMAGES) ? BATCH_SIZE : (NUM_IMAGES - i);
        
        for (int j = 0; j < current_batch_size; j++) {
            fread(&imagesC[j].label, sizeof(unsigned char), 1, file);
            fread(imagesC[j].pixels, sizeof(unsigned char), IMAGE_SIZE, file);
            
            imagesF[i + j].label = (int)imagesC[j].label;
            for (int k = 0; k < IMAGE_SIZE; k++) {
                imagesF[i + j].pixels[k] = (double)imagesC[j].pixels[k] / 255.0;
            }
        }
    }
    

    // forward pass
    // ModelResult *model_result = ModelParams();
    // Tensor loss = {0, 1};



    // for(int i = 0; i < BATCH_SIZE; i++) {
    //     model(imagesF[i].pixels, model_result);
    //     loss.value += neg_log_likelihood(model_result->out, imagesF[i].label, 10, loss.grad/BATCH_SIZE).value;
    //     backward(model_result);
    //     update_params(model_result, 0.1);
    //     // printf("Linear3 Out grad: ");
    //     // for(int i = 0; i < 10; i++){
    //     //     printf("%f ", model_result->out[i].grad);
    //     // }
    //     // printf("\n");
    //     // printf("Linear3 Weight grad: ");
    //     // for(int i = 0; i < 10; i++) {
    //     //     printf("%f ", model_result->linear3->weights[i].grad);
    //     // }
    //     // printf("\n");
    //     // printf("Linear3 Bias grad: ");
    //     // for(int i = 0; i < 10; i++) {
    //     //     printf("%f ", model_result->linear3->bias[i].grad);
    //     // }
    //     // printf("\n");
    //     // printf("Linear2 Out grad: ");
    //     // for(int i = 0; i < 10; i++) {
    //     //     printf("%f ", model_result->linear2->out[i].value);
    //     // }
    //     // printf("\n");
    //     // printf("Linear2 Out grad: ");
    //     // for(int i = 0; i < 10; i++) {
    //     //     printf("%f ", model_result->linear2->out[i].grad);
    //     // }
    //     // printf("\nNorm Out Grad: ");
    //     // for(int i=0; i<10; i++){
    //     //     printf("%f ", model_result->conv2norm[i].grad);
    //     // }
    //     // printf("\n");
    //     // printf("\n");
    // }
    // loss.value /= BATCH_SIZE;
    // printf("Loss: %f\n", loss.value);

    train_model(imagesF, 10, IMAGE_SIZE, BATCH_SIZE, 0.1);


    // free memory
    free(imagesC);
    free(imagesF);

    fclose(file);
    return 0;
}
