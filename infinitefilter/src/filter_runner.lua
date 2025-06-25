package.cpath = package.cpath .. ";../bin/?.dll"

local ffi = require("ffi")
local lfs = require("lfs")


local function init(filter_name)
    return {
        CPP_SOURCE = filter_name .. ".cpp",
        LIB_NAME = filter_name .. (ffi.os == "Windows" and ".dll" or ".so"),
        OPT_FLAGS = "-O3 -fPIC -shared -static -s"
    }
end

local function needs_recompile(config)
    local src_time = lfs.attributes(config.CPP_SOURCE, "modification")
    local lib_time = lfs.attributes(config.LIB_NAME, "modification")
    return not lib_time or (src_time > lib_time)
end


function apply_filter(img_data, width, height, channels, filter_name)
    local config = init(filter_name)
    
    if needs_recompile(config) then
        local compile_cmd = string.format(
            "g++ %s %s -o %s 2>&1",
            config.OPT_FLAGS, config.CPP_SOURCE, config.LIB_NAME
        )
        
        local handle = io.popen(compile_cmd, "r")
        local output = handle:read("*a")
        local success, _, exit_code = handle:close()
        
        if not success or exit_code ~= 0 then
            error("Compilation failed:\n" .. output)
        end
    end

    local filter_lib = ffi.load(config.LIB_NAME)
    ffi.cdef[[void filter(uint8_t* img, int width, int height, int channels);]]
    
    filter_lib.filter(ffi.cast("uint8_t*", img_data), width, height, channels)
    return img_data
end