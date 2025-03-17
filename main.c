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
#define BATCH_SIZE 50

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
    char filename[100];
    
    file = fopen("dataset/data_batch_1.bin", "rb");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 1;
    }
    
    ImageF *imagesF = (ImageF*)malloc(NUM_IMAGES * sizeof(ImageF));
    ImageC *imagesC = (ImageC*)malloc(BATCH_SIZE * sizeof(ImageC));

    for (int i = 0; i < NUM_IMAGES; i++) {
        fread(&imagesC, sizeof(ImageC), BATCH_SIZE, file);
        imagesF[i].label = (int)imagesC[0].label;
        for (int j = 0; j < IMAGE_SIZE; j++) {
            imagesF[i].pixels[j] = (double)imagesC[0].pixels[j] / 255.0;
        }
    }
    
    ModelResult *model_result = ModelParams();
    
    for(int i=0; i<NUM_IMAGES; i++){
        model(imagesF[i].pixels, model_result);
        
    }


    free(imagesC);
    free(imagesF);
    free(model_result);

    fclose(file);
    return 0;
}
