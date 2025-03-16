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
} Image;

int main() {
    FILE *file;
    Image *images;
    char filename[100];
    
    images = (Image*)malloc(NUM_IMAGES * sizeof(Image));
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

    size_t items_read = fread(images, sizeof(Image), NUM_IMAGES, file);
    printf("Read %zu images from file\n", items_read);
    
    fclose(file);

    
   
    
    
    free(images);
    return 0;
}
