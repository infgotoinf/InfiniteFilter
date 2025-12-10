#include <cstdint>

extern "C" void filter(uint8_t* img, int width, int height, int channels) {
    for (size_t i = 0; i < width * height * channels; i += channels) {
        if (channels >= 3) {
            uint8_t r = img[i];
            uint8_t g = img[i + 1];
            uint8_t b = img[i + 2];

            uint8_t total = (r + g + b) / 3 > 122 ? 255 : 0;

            img[i]     = total;
            img[i + 1] = total;
            img[i + 2] = total;
        }
    }
}