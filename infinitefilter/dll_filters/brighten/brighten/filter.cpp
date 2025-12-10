#include "pch.h"
#include <cstdint>
#include "filter.h"

void filter(uint8_t* img, int width, int height, int channels) {
    for (size_t i = 0; i < width * height * channels; i += channels) {
        if (channels >= 3) {
            uint8_t r = img[i];
            uint8_t g = img[i + 1];
            uint8_t b = img[i + 2];

            uint8_t bright_r = float(r) * 1.5;
            uint8_t bright_g = float(g) * 1.5;
            uint8_t bright_b = float(b) * 1.5;

            img[i] = bright_r < r ? 255 : bright_r;
            img[i + 1] = bright_g < g ? 255 : bright_g;
            img[i + 2] = bright_b < b ? 255 : bright_b;
        }
    }
}