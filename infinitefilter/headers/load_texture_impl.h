#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#ifdef _WIN32
#include <windows.h>
#endif

#include "../../3rdparty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include "../../3rdparty/stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../3rdparty/stb/stb_image_resize2.h"

#include <SDL.h>
#include <SDL_syswm.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif


unsigned char* file_data;

bool LoadTextureFromMemory(const void* data, size_t data_size, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height)
{
    int image_width = 0;
    int image_height = 0;
    int channels = 4;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == nullptr)
    {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)image_data, image_width, image_height, channels * 8, channels * image_width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (surface == nullptr)
    {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

    *out_texture = texture;
    *out_width = image_width;
    *out_height = image_height;

    SDL_FreeSurface(surface);
    stbi_image_free(image_data);

    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height)
{
#ifdef _WIN32
    // Convert UTF-8 to UTF-16 for Windows
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
    if (file_size == -1) {
        fclose(f);
        return false;
    }
    
    fseek(f, 0, SEEK_SET);
    file_data = new unsigned char[file_size];
    size_t read_size = fread(file_data, 1, file_size, f);
    fclose(f);
    
    if (read_size != static_cast<size_t>(file_size)) {
        delete[] file_data;
        return false;
    }
    
    bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
    delete[] file_data;
    return ret;
}