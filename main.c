#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "bmp.h"

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define CHANNELS 3
#define IMAGE_SIZE (IMAGE_WIDTH * IMAGE_HEIGHT * CHANNELS)
#define NUM_IMAGES 10000 

typedef struct {
    unsigned char label;
    unsigned char pixels[IMAGE_SIZE];
} ImageC;

typedef struct {
    double label;
    double pixels[IMAGE_SIZE];
} ImageF;

int main() {
    FILE *file;
    ImageC *images;
    ImageF *images_f;
    char filename[100];
    
    images = (ImageC*)malloc(NUM_IMAGES * sizeof(ImageC));
    if (images == NULL) {
        printf("Memory allocation failed\n");
        return 1;
    }
    
    file = fopen("dataset/data_batch_1.bin", "rb");
    if (file == NULL) {
        printf("Error opening file!");
        free(images);
        return 1;
    }

    size_t items_read = fread(images, sizeof(ImageC), NUM_IMAGES, file);
    printf("Read %zu images from file\n", items_read);
    
    fclose(file);

    images_f = (ImageF*)malloc(NUM_IMAGES * sizeof(ImageF));
    for(int i=0; i<NUM_IMAGES; i++){
        images_f[i].label = (double)images[i].label;
        memcpy(images_f[i].pixels, norm_image(images[i].pixels, IMAGE_SIZE), IMAGE_SIZE * sizeof(double));
    }

    


    
    
    free(images);
    return 0;
}
