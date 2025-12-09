#include "pch.h"
#include <cstdint>
#include "filter.h"

void filter(uint8_t* img, int width, int height, int channels) {
    for (size_t i = 0; i < width * height * channels; i += channels) {
        if (channels >= 3) {
            uint8_t r = img[i];
            uint8_t g = img[i + 1];
            uint8_t b = img[i + 2];
            uint8_t gray = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);

            img[i] = gray;
            img[i + 1] = gray;
            img[i + 2] = gray;
        }
    }
}