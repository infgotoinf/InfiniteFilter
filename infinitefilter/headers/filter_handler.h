#include <cstdint>
#include <filesystem>

extern "C" {
    #include "../../3rdparty/luajit/lua.h"
    #include "../../3rdparty/luajit/lauxlib.h"
    #include "../../3rdparty/luajit/lualib.h"
}

namespace fs = std::filesystem;

lua_State* L = nullptr;

void lua_init()
{
    L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_dofile(L, "scr/filter_runner.lua");
}

void filter_image(uint8_t* data, int img_width, int img_height, int img_channels, 
                 const char* filter_name, SDL_Renderer* renderer, SDL_Texture** texture)
{
    if (!L) {
        fprintf(stderr, "Lua state not initialized!\n");
        return;
    }
    
    // Push the function and arguments
    lua_getglobal(L, "apply_filter");
    lua_pushlightuserdata(L, data);
    lua_pushinteger(L, img_width);
    lua_pushinteger(L, img_height);
    lua_pushinteger(L, img_channels);
    lua_pushstring(L, filter_name);
    
    if (lua_pcall(L, 5, 0, 0) != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    
    // Destroy old texture and create new one with updated pixels
    if (*texture) {
        SDL_DestroyTexture(*texture);
    }
    *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                SDL_TEXTUREACCESS_STATIC, 
                                img_width, img_height);
    
    // Update texture with modified pixel data
    SDL_UpdateTexture(*texture, NULL, data, img_width * 4);
    SDL_SetTextureBlendMode(*texture, SDL_BLENDMODE_BLEND);
}