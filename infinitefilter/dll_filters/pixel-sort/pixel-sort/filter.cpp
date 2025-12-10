#include "pch.h"
#include <cstdint>
#include <utility>
#include "filter.h"

void filter(uint8_t* img, int width, int height, int channels)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (channels >= 3)
            {
                int idx = (x + y * width) * channels;
                uint8_t r = img[idx];
                uint8_t g = img[idx + 1];
                uint8_t b = img[idx + 2];

                bool start_sort = (r + g + b) / 3 < 55 ? true : false;

                if (!start_sort)
                {
                    img[idx] = r;
                    img[idx + 1] = g;
                    img[idx + 2] = b;
                }
                else
                {
                    for (int x2 = x; x2 < width - channels; x2++)
                    {
                        for (int i = x2; i < width - channels; i++)
                        {
                            int idx = (i + y * width) * channels;
                            r = img[idx];
                            g = img[idx + 1];
                            b = img[idx + 2];

                            uint16_t sum_this = r + g + b;


                            uint16_t nr = img[idx + channels];
                            uint16_t ng = img[idx + channels + 1];
                            uint16_t nb = img[idx + channels + 2];

                            uint16_t sum_next = nr + ng + nb;

                            if (sum_this > sum_next)
                            {
                                img[idx] = r;
                                img[idx + 1] = g;
                                img[idx + 2] = b;
                            }
                            else
                            {
                                std::swap(img[idx], img[idx + channels]);
                                std::swap(img[idx + 1], img[idx + channels + 1]);
                                std::swap(img[idx + 2], img[idx + channels + 2]);
                                /*
                                img[idx] = nr;
                                img[idx + 1] = ng;
                                img[idx + 2] = nb;
                                */
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}