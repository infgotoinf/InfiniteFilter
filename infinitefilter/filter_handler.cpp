#include "include/filter_handler.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include <iostream>


typedef void (*filter_func)(uint8_t* img, int width, int height, int channels); 

void filterImage(uint8_t* data, int img_width, int img_height, int img_channels
               , const char* filter_name, SDL_Renderer* renderer, SDL_Texture** texture)
{
    std::string full_path = "./build/filters/";
    full_path += filter_name;
    
    void* handle = nullptr;
    filter_func function = nullptr;

#ifdef _WIN32
    handle = LoadLibraryA(full_path.c_str());
    if (handle == nullptr)
    {
        std::cerr << "Error: Could not load library. Error code: " << GetLastError() << std::endl;
        return;
    }

    function = (filter_func)GetProcAddress((HMODULE)handle, "filter");
#else
    // RTLD_LAZY means resolve symbols as needed
    handle = dlopen(full_path.c_str(), RTLD_LAZY);
    if (handle == nullptr)
    {
        std::cerr << "Error: " << dlerror() << std::endl;
        return;
    }

    function = (filter_func)dlsym(handle, "filter");
#endif

    if (function == nullptr)
    {
        std::cerr << "Error: Could not find the 'filter' function." << std::endl;
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }

    function(data, img_width, img_height, img_channels);
    
    if (*texture != nullptr) {
        SDL_DestroyTexture(*texture);
    }
    
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
          (void*)data, img_width, img_height
        , img_channels * 8, img_channels * img_width
        , 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (surface == nullptr)
    {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }
    
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (*texture == nullptr)
    {
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());
#ifdef _WIN32
        FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
        return;
    }
    
    SDL_FreeSurface(surface);

#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose(handle);
#endif
}