#include "pch.h"
#include <cstdint>
#include "filter.h"

void filter(uint8_t* img, int width, int height, int channels) {
    for (size_t i = 0; i < width * height * channels; i += channels) {
        if (channels >= 3) {
            uint8_t r = img[i];
            uint8_t g = img[i + 1];
            uint8_t b = img[i + 2];

            img[i] = r > 122 ? r = 255 : r = 0;
            img[i + 1] = g > 122 ? g = 255 : g = 0;
            img[i + 2] = b > 122 ? b = 255 : b = 0;
        }
    }
}