#ifndef FILTER_HANDLER_
#define FILTER_HANDLER_

#include <SDL2/SDL_render.h>


void filterImage(uint8_t* data, int img_width, int img_height, int img_channels, 
                 const char* filter_name, SDL_Renderer* renderer, SDL_Texture** texture);

#endif // FILTER_HANDLER_