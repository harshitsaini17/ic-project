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

typedef struct {
    int label;
    double pixels[IMAGE_SIZE];
} ImageF;

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
    ModelResult *model_result = ModelParams();
    Tensor loss = {0, 1};
    for(int i = 0; i < BATCH_SIZE; i++) {
        model(imagesF[i].pixels, model_result);
        loss.value += neg_log_likelihood(model_result->out, imagesF[i].label, 10, loss.grad/BATCH_SIZE).value;
    }
    loss.value /= BATCH_SIZE;
    printf("Loss: %f\n", loss.value);

    // backward pass
    backward(model_result);


    // free memory
    free(imagesC);
    free(imagesF);
    free(model_result);

    fclose(file);
    return 0;
}
