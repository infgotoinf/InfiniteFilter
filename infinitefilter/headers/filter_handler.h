#include <cstdint>
#include <fstream>
#include <filesystem>

extern "C" {
    #include "../../3rdparty/luajit/lua.h"
    #include "../../3rdparty/luajit/lauxlib.h"
    #include "../../3rdparty/luajit/lualib.h"
}

namespace fs = std::filesystem;

lua_State* L;

void lua_init()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    
    luaL_dofile(L, "filter_runner.lua");
    lua_getglobal(L, "apply_filter");
}

void filter_image(uint8_t* data, int img_width, int img_height, int img_channels, const char* filter_name)
{
    lua_pushlightuserdata(L, data);
    lua_pushinteger(L, img_width);
    lua_pushinteger(L, img_height);
    lua_pushinteger(L, img_channels);
    lua_pushstring(L, filter_name);
    
    if (lua_pcall(L, 5, 0, 0) != LUA_OK) {
        fprintf(stderr, "Lua error: %s\n", lua_tostring(L, -1));
    }
}