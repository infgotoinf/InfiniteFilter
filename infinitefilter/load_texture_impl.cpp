#include "include/load_texture_impl.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL.h>
#include <SDL_syswm.h>
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb/stb_image.h"

bool loadTextureFromMemory(uint8_t* data, size_t data_size, SDL_Renderer* renderer
                         , SDL_Texture** out_texture, int* out_width, int* out_height
                         , uint8_t** out_pixel_data, int* out_channels)
{
    int image_width = 0;
    int image_height = 0;
    int image_channels = 0;
    
    uint8_t* image_data = stbi_load_from_memory((const uint8_t*)data, (int)data_size
                                              , &image_width, &image_height, &image_channels, 0);
    if (image_data == nullptr)
    {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }
    
    int target_channels = 4;
    uint8_t* rgba_data = new uint8_t[image_width * image_height * target_channels];
    
    if (image_channels == 3) {
        for (int i = 0, j = 0; i < image_width * image_height * 3; i += 3, j += 4) {
            rgba_data[j]   = image_data[i];
            rgba_data[j+1] = image_data[i+1];
            rgba_data[j+2] = image_data[i+2];
            rgba_data[j+3] = 255;
        }
    } else if (image_channels == 4) {
        for (int i = 0, j = 0; i < image_width * image_height * 4; i += 4, j += 4) {
            rgba_data[j]   = image_data[i];
            rgba_data[j+1] = image_data[i+1];
            rgba_data[j+2] = image_data[i+2];
            rgba_data[j+3] = image_data[i+3];
        }
    }
    
    stbi_image_free(image_data);
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
          (void*)rgba_data, image_width, image_height
        , target_channels * 8, target_channels * image_width
        , 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    
    if (surface == nullptr)
    {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        delete[] rgba_data;
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

    *out_texture = texture;
    *out_width = image_width;
    *out_height = image_height;
    *out_pixel_data = rgba_data;
    *out_channels = target_channels;

    SDL_FreeSurface(surface);

    return true;
}

bool loadTextureFromFile(const char* file_name, uint8_t** out_file_data, SDL_Renderer* renderer
                       , SDL_Texture** out_texture, int* out_width, int* out_height, int* out_channels)
{
#ifdef _WIN32
    int wlen = MultiByteToWideChar(CP_UTF8, 0, file_name, -1, NULL, 0);
    wchar_t* wfilename = new wchar_t[wlen];
    MultiByteToWideChar(CP_UTF8, 0, file_name, -1, wfilename, wlen);
    
    FILE* f = _wfopen(wfilename, L"rb");
    delete[] wfilename;
#else
    FILE* f = fopen(file_name, "rb");
#endif
    
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file: %s\n", file_name);
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    uint8_t* compressed_data = new uint8_t[file_size];
    size_t read_size = fread(compressed_data, 1, file_size, f);
    fclose(f);
    
    if (read_size != static_cast<size_t>(file_size)) {
        delete[] compressed_data;
        return false;
    }
    
    uint8_t* pixel_data = nullptr;
    int channels = 0;
    bool ret = loadTextureFromMemory(compressed_data, file_size, renderer, 
                                   out_texture, out_width, out_height,
                                   &pixel_data, &channels);
    
    delete[] compressed_data;

    if (ret) {
        *out_file_data = pixel_data;
        *out_channels = channels;
    } else {
        *out_file_data = nullptr;
    }
    
    return ret;
}