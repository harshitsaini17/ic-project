#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 32
#define CHANNELS 3

void save_as_bmp(unsigned char *pixels, const char *filename) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;
    
    // BMP file header
    unsigned char file_header[14] = {
        'B', 'M',           // signature
        0, 0, 0, 0,         // file size (filled later)
        0, 0, 0, 0,         // reserved
        54, 0, 0, 0         // pixel data offset
    };
    
    // BMP info header
    unsigned char info_header[40] = {
        40, 0, 0, 0,        // info header size
        0, 0, 0, 0,         // width (filled later)
        0, 0, 0, 0,         // height (filled later)
        1, 0,               // number of color planes
        24, 0,              // bits per pixel
        0, 0, 0, 0,         // compression
        0, 0, 0, 0,         // image size (filled later)
        0, 0, 0, 0,         // horizontal resolution
        0, 0, 0, 0,         // vertical resolution
        0, 0, 0, 0,         // colors in color table
        0, 0, 0, 0          // important color count
    };
    
    // Set width and height in info header
    *(int*)&info_header[4] = IMAGE_WIDTH;
    *(int*)&info_header[8] = IMAGE_HEIGHT;
    
    // Row size must be divisible by 4
    int row_size = ((IMAGE_WIDTH * 3 + 3) / 4) * 4;
    int image_size = row_size * IMAGE_HEIGHT;
    
    // Set file size in file header
    *(int*)&file_header[2] = 54 + image_size;
    
    // Set image size in info header
    *(int*)&info_header[20] = image_size;
    
    // Write headers
    fwrite(file_header, 1, 14, f);
    fwrite(info_header, 1, 40, f);
    
    // BMP stores images upside down
    for (int y = IMAGE_HEIGHT-1; y >= 0; y--) {
        for (int x = 0; x < IMAGE_WIDTH; x++) {
            // BMP uses BGR format
            fputc(pixels[2048 + y * IMAGE_WIDTH + x], f); // B
            fputc(pixels[1024 + y * IMAGE_WIDTH + x], f); // G
            fputc(pixels[y * IMAGE_WIDTH + x], f);        // R
        }
        // Pad row to be divisible by 4
        for (int i = 0; i < row_size - IMAGE_WIDTH * 3; i++) {
            fputc(0, f);
        }
    }
    
    fclose(f);
}
