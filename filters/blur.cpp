#include <cstdint>
#include <memory>

extern "C" void filter(uint8_t* img, int width, int height, int channels) {
    uint8_t* temp = new uint8_t[width * height * channels];
    memcpy(temp, img, width * height * channels);
    int spread = (width + height) / 200;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            size_t center_idx = (y * width + x) * channels;

            if (channels >= 3)
            {
                int r_sum = 0, g_sum = 0, b_sum = 0;
                int count = 0;

                for (int ky = -spread; ky <= spread; ky++)
                {
                    for (int kx = -spread; kx <= spread; kx++)
                    {
                        int nx = x + kx;
                        int ny = y + ky;

                        if (nx >= 0 && nx < width && ny >= 0 && ny < height)
                        {
                            size_t neighbor_idx = (ny * width + nx) * channels;
                            r_sum += temp[neighbor_idx];
                            g_sum += temp[neighbor_idx + 1];
                            b_sum += temp[neighbor_idx + 2];
                            count++;
                        }
                    }
                }

                img[center_idx] = r_sum / count;
                img[center_idx + 1] = g_sum / count;
                img[center_idx + 2] = b_sum / count;
            }
        }
    }

    delete[] temp;
}