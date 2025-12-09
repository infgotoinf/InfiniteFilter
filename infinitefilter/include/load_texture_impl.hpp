#ifndef LOAD_TEXTURE_FROM_IMPL_
#define LOAD_TEXTURE_FROM_IMPL_

#include <SDL2/SDL_render.h>


bool loadTextureFromMemory(uint8_t* data, size_t data_size, SDL_Renderer* renderer
                         , SDL_Texture** out_texture, int* out_width, int* out_height
                         , uint8_t** out_pixel_data, int* out_channels);

bool loadTextureFromFile(const char* file_name, uint8_t** out_file_data, SDL_Renderer* renderer
                       , SDL_Texture** out_texture, int* out_width, int* out_height, int* out_channels);


#endif // LOAD_TEXTURE_FROM_IMPL_