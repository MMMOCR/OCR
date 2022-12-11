#include "erosion_dilation.h"

void
dilate(Uint32* pixels_in,Uint32* pixels_out , int size,long int w, long int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // Check the neighboring pixels
            int max = 0;
            int max_value = 0;
            for (int x = -size; x <= size; x++) {
                for (int y = -size; y <= size; y++) {
                    if (i + y >= 0 && i + y < h && j + x >= 0 && j + x < w) {
                        int val = (pixels_in[(i+y)*w + (j+x)]);
                        if ((val >> 16&0xff) > max) {
                        // if ((val >> 16&0xff) == 255 && max == 0) {
                            max = (val >> 16&0xff);
                            max_value = val;
                        }
                    }
                }
            }
            pixels_out[i*w + j] = max_value;
        }
    }
}

void
erode(Uint32* pixels_in,Uint32* pixels_out , int size,long int w, long int h) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            // Check the neighboring pixels
            int min = 255;
            int min_value = 255;
            for (int x = -size; x <= size; x++) {
                for (int y = -size; y <= size; y++) {
                    if (i + y >= 0 && i + y < h && j + x >= 0 && j + x < w) {
                        int val = (pixels_in[(i+y)*w + (j+x)]);
                        if ((val >> 16&0xff) < min) {
                        // if ((val >> 16&0xff) == 0 && min == 255) {
                            min = (val >> 16&0xff);
                            min_value = val;
                        }
                    }
                }
            }
            pixels_out[i*w + j] = min_value;
        }
    }
}
