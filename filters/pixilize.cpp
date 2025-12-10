#include <cstdint>
#include <memory>

extern "C" void filter(uint8_t* img, int width, int height, int channels) {
    uint8_t* temp = new uint8_t[width * height * channels];
    memcpy(temp, img, width * height * channels);

    int pixel_size = (width + height) / 100;
    if (pixel_size < 2) pixel_size = 2;

    for (int block_y = 0; block_y < height; block_y += pixel_size)
    {
        for (int block_x = 0; block_x < width; block_x += pixel_size)
        {
            if (channels >= 3)
            {
                int r_sum = 0, g_sum = 0, b_sum = 0;
                int count = 0;

                int block_end_y = block_y + pixel_size;
                int block_end_x = block_x + pixel_size;
                if (block_end_y > height) block_end_y = height;
                if (block_end_x > width) block_end_x = width;

                for (int y = block_y; y < block_end_y; y++)
                {
                    for (int x = block_x; x < block_end_x; x++)
                    {
                        size_t idx = (y * width + x) * channels;
                        r_sum += temp[idx];
                        g_sum += temp[idx + 1];
                        b_sum += temp[idx + 2];
                        count++;
                    }
                }

                if (count > 0)
                {
                    uint8_t avg_r = static_cast<uint8_t>(r_sum / count);
                    uint8_t avg_g = static_cast<uint8_t>(g_sum / count);
                    uint8_t avg_b = static_cast<uint8_t>(b_sum / count);

                    for (int y = block_y; y < block_end_y; y++)
                    {
                        for (int x = block_x; x < block_end_x; x++)
                        {
                            size_t idx = (y * width + x) * channels;
                            img[idx] = avg_r;
                            img[idx + 1] = avg_g;
                            img[idx + 2] = avg_b;
                        }
                    }
                }
            }
        }
    }

    delete[] temp;
}