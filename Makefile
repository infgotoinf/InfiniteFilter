##---------------------------------------------------------------------------------
#
#                      ██████╗ ██╗   ██╗██╗██╗     ██████╗
#                      ██╔══██╗██║   ██║██║██║     ██╔══██╗
#                      ██████╔╝██║   ██║██║██║     ██║  ██║
#                      ██╔══██╗██║   ██║██║██║     ██║  ██║
#                      ██████╔╝╚██████╔╝██║███████╗██████╔╝
#                      ╚═════╝  ╚═════╝ ╚═╝╚══════╝╚═════╝
#
##---------------------------------------------------------------------------------
#                           Requirements to build
#
# To build this programm on windows you will need: make, g++, libstdc++, lwinpthread
# You need to download MSYS2 (https://www.msys2.org/)
# And install them via UCRT64 (C:\msys64\ucrt64.exe):
# pacman -Syu
# pacman -S make
# pacman -S mingw-w64-ucrt-x86_64-toolchain
#
# You will also need freetype, libpng, harfbuzz, brotli, bzip2, graphite2, SDL2
# pacman -S mingw-w64-ucrt-x86_64-LIBNAME
# 
#----------------------------------------------------------------------------------
##
## in UCRT64 cd to the project folder and run:
## make build=debug
##        OR
## make build=release
## 
## Use 'make clean' to clean build folder
##
##---------------------------------------------------------------------------------
#                              For developers
#
# After building a release I compress it via UPX https://github.com/upx/upx
#
##---------------------------------------------------------------------------------

RELEASE = RELEASE_Infinite_Filter
DEBUG   = DEBUG_Infinite_Filter

PROJECT_DIR  = ./infinitefilter
3RDPARTY_DIR = ./infinitefilter/3rdparty
INCLUDE_DIR  = ./infinitefilter/include
BUILD_DIR    = ./build
FILTER_DIR   = ./filters

IMGUI_DIR          = $(3RDPARTY_DIR)/imgui
IMGUI_FREETYPE_DIR = $(3RDPARTY_DIR)/imgui/freetype
IMGUI_FD_DIR       = $(3RDPARTY_DIR)/imgui_fd
STB_DIR            = $(3RDPARTY_DIR)/stb

WIN_SDL2_DIR     = C:/msys64/ucrt64/include/SDL2
WIN_FREETYPE_DIR = C:/msys64/ucrt64/include/freetype2


SOURCES := $(shell find $(PROJECT_DIR) -maxdepth 1 -name '*.cpp') \
           $(shell find $(3RDPARTY_DIR) -name '*.cpp')

SOURCES := $(basename $(notdir $(SOURCES)))
OBJS    := $(SOURCES:%=$(BUILD_DIR)/$(build)_%.o)


FILTERS  := $(shell find $(FILTER_DIR) -name '*.cpp')
FILTERS  := $(basename $(notdir $(FILTERS)))


DLL_FILTER_DIR = $(PROJECT_DIR)/dll_filters
DLL_FILTERS   := $(shell ls $(DLL_FILTER_DIR))

DLL_BUILD_FILTER_DIR = $(BUILD_DIR)/filters
DLL_BUILD_FILTER_FILES = \
    $(foreach filter,$(DLL_FILTERS), \
    $(DLL_BUILD_FILTER_DIR)/$(filter).dll)


CXXFLAGS = -std=c++17 \
           -I$(INCLUDE_DIR) \
           -I$(IMGUI_DIR) \
           -I$(IMGUI_FREETYPE_DIR) \
           -I$(IMGUI_FD_DIR) \
           -I$(STB_DIR) \
           -I$(WIN_SDL2_DIR) \
           -I$(WIN_FREETYPE_DIR)

RELEASE_CXXFLAGS = -g0 -O3 -Wall -Wextra -pedantic -flto
                   # -w -DNDEBUG -fno-rtti -fno-exceptions \
                   # -ffunction-sections -fdata-sections -Wl,--gc-sections \
                   # -fvisibility=hidden -fomit-frame-pointer -funroll-loops \
                   # -fstrict-aliasing -fipa-pta -ftree-vectorize \
                   # -fno-semantic-interposition -Wl,-O3 -Wl,--relax \
                   # -Wl,--strip-all -mfpmath=both -mbranch-cost=2 \
                   # -fno-stack-protector -fno-unwind-tables
                   # There are hell-a-lot-of stuff

DEBUG_CXXFLAGS = -g -g3 -Og -Wall -Wextra -pedantic

LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lfreetype -lpng -lharfbuzz -lgraphite2 \
          -ldwrite -lbrotlidec -lbrotlicommon -lbz2 -lz -lusp10 -lrpcrt4 \
          -Wl,--dynamicbase -Wl,--nxcompat \
          -static-libstdc++ -static-libgcc -static -lwinpthread -lsetupapi -lhid \
          -lwinmm -limm32 -lshell32 -lole32 -loleaut32 -luuid -lversion -msse2

##---------------------------------------------------------------------------------
##                         DEBUG AND RELEASE REALISATION
##---------------------------------------------------------------------------------

ifeq ($(build), debug)
	CXXFLAGS += $(DEBUG_CXXFLAGS)
	EXE = $(DEBUG)
endif
ifeq ($(build), release)
	CXXFLAGS += $(RELEASE_CXXFLAGS)
	EXE = $(RELEASE)
endif

##---------------------------------------------------------------------------------
##           BUILD FLAGS PER PLATFORM (tested only on windows for now)
##---------------------------------------------------------------------------------

ifeq ($(OS),Windows_NT)
    CXX      = x86_64-w64-mingw32-g++
    PLATFORM = windows
    FILTER_FLAGS  = -static-libgcc -static-libstdc++
    LIB_EXTENSION = .dll
    LIB_OBJS := $(FILTERS:%=$(BUILD_DIR)/filters/%.dll)
else
    FILTER_FLAGS  = -shared -fPIC
    LIB_EXTENSION = .so
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        CXX = g++
        PLATFORM = linux
    endif
    ifeq ($(UNAME_S),Darwin)
        CXX = clang++
        PLATFORM = macos
    endif
endif

ifeq ($(PLATFORM),windows)
  LDFLAGS += -mwindows  # Hide console window
else ifeq ($(PLATFORM),macos)
  LDFLAGS += -framework Cocoa -framework CoreVideo
else ifeq ($(PLATFORM),linux)
  LDFLAGS += -lm -ldl -lpthread -lrt
endif

##---------------------------------------------------------------------------------
##                                 BUILD RULES
##---------------------------------------------------------------------------------

$(BUILD_DIR)/$(build)_%.o:$(PROJECT_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_FD_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_FREETYPE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


#$(BUILD_DIR)/filters/%$(LIB_EXTENSION):$(FILTER_DIR)/%.cpp
#	$(CXX) $(FILTER_FLAGS) -c -o $@ $<


all: mkdir mkdir_filters $(DLL_BUILD_FILTER_FILES) $(BUILD_DIR)/$(EXE) run
	@echo $(build) build complete

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)


mkdir:
	mkdir -p $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(EXE)

mkdir_filters: mkdir
	mkdir -p $(BUILD_DIR)/filters



$(DLL_BUILD_FILTER_FILES):
	$(foreach filter,$(DLL_FILTERS), \
		cp "$(DLL_FILTER_DIR)/$(filter)/x64/Release/$(filter).dll" \
       "$(DLL_BUILD_FILTER_DIR)/$(filter).dll";)


clean:
	rm -rf $(BUILD_DIR)