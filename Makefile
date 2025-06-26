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
##                !THIS FROJECT IS CONFIGURED TO RUN IN VS CODE!
##
## Making a build takes time, so launch.json may throw an error at the first run
##      or you will have to rerun build to see your updated application!
## - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
##
## Go to "Run and Debug" (Ctrl+Shift+D) and choose "Debug Build" or "Release Build"
## To clean "build" folder press (Ctrl+Shift+P) -> "Tasks: Run Task" -> make-clean
##
##---------------------------------------------------------------------------------
#                           Requirements to build
#
# To build this programm on windows you will need: g++, libstdc++, lwinpthread
# Install them via UCRT64 (C:\msys64\ucrt64.exe):
# pacman -Syu
# pacman -S mingw-w64-ucrt-x86_64-toolchain
#
# You will also need freetype, libpng, harfbuzz, brotli, bzip2, graphite2, SDL2
# pacman -S mingw-w64-ucrt-x86_64-LIBNAME
# 
##---------------------------------------------------------------------------------
##           If you prefer to use console or don't want to use VS Code
##
## in UCRT64 cd to the project folder and run:
## make build=debug
##        OR
## make build=release
## (after building a release I compress it via UPX https://github.com/upx/upx)
## 
## Use 'make clean' to clean build folder
##
##---------------------------------------------------------------------------------

CXX  = g++

RELEASE = RELEASE_Infinite_Filter
DEBUG   = DEBUG_Infinite_Filter

SRC_DIR       = ./infinitefilter/src
BUILD_DIR     = ./build
LUAJIT_DIR    = ./3rdparty/luajit
IMGUI_DIR     = ./3rdparty/imgui
IMGUI_FD_DIR  = ./3rdparty/imgui_fd
BACKENDS_DIR  = $(IMGUI_DIR)/backends
FREETYPE_DIR2 = $(IMGUI_DIR)/freetype
STB_DIR       = ./3rdparty/stb
SDL2_DIR      = C:/msys64/ucrt64/include/SDL2
FREETYPE_DIR  = C:/msys64/ucrt64/include/freetype2

FILTER_FILES := $(wildcard filters/*.cpp)


SOURCES := $(shell find $(SRC_DIR) -name '*.cpp') \
           $(shell find $(IMGUI_DIR) -name '*.cpp')\
           $(shell find $(IMGUI_FD_DIR) -name '*.cpp')

SOURCES := $(basename $(notdir $(SOURCES)))
OBJS    := $(SOURCES:%=$(BUILD_DIR)/$(build)_%.o)


CXXFLAGS = -std=c++17 \
           -I$(IMGUI_DIR) \
           -I$(IMGUI_FD_DIR) \
           -I$(BACKENDS_DIR) \
           -I$(FREETYPE_DIR2) \
           -I$(STB_DIR) \
           -I$(SDL2_DIR) \
           -I$(FREETYPE_DIR) \
           -I$(LUAJIT_DIR)

RELEASE_CXXFLAGS = -g0 -O3 -w -DNDEBUG -flto -fno-rtti -fno-exceptions \
                   -ffunction-sections -fdata-sections -Wl,--gc-sections \
                   -fvisibility=hidden -fomit-frame-pointer -funroll-loops \
                   -fstrict-aliasing -fipa-pta -ftree-vectorize \
                   -fno-semantic-interposition -Wl,-O3 -Wl,--relax \
                   -Wl,--strip-all -mfpmath=both -mbranch-cost=2 \
                   -fno-stack-protector -fno-unwind-tables
                   # There are hell-a-lot-of stuff

DEBUG_CXXFLAGS = -g -g3 -O0 -Wall -Wextra -pedantic

LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lfreetype -lpng -lharfbuzz -lgraphite2 \
          -ldwrite -lbrotlidec -lbrotlicommon -lbz2 -lz -lusp10 -lrpcrt4 \
          -Wl,--dynamicbase -Wl,--nxcompat -L$(LUAJIT_DIR) -l:liblua51.dll.a\
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
    PLATFORM = windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        PLATFORM = linux
    endif
    ifeq ($(UNAME_S),Darwin)
        PLATFORM = macos
    endif
endif

ifeq ($(PLATFORM),windows)
  LDFLAGS += -mwindows  # Hide console window
  EXE_EXT = .exe
else ifeq ($(PLATFORM),macos)
  LDFLAGS += -framework Cocoa -framework CoreVideo
  EXE_EXT =
else ifeq ($(PLATFORM),linux)
  LDFLAGS += -lm -ldl -lpthread -lrt
  EXE_EXT =
endif

##---------------------------------------------------------------------------------
##                                 BUILD RULES
##---------------------------------------------------------------------------------

$(BUILD_DIR)/$(build)_%.o:$(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_FD_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(BACKENDS_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(FREETYPE_DIR2)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


all: mkdir mkdir_bin mkdir_src mkdir_filters $(BUILD_DIR)/$(EXE)
	@echo $(build) build complete

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)


mkdir:
	mkdir -p $(BUILD_DIR)

mkdir_bin: mkdir
	mkdir -p $(BUILD_DIR)/bin
	cp $(LUAJIT_DIR)/lfs.dll $(BUILD_DIR)/bin/
	cp $(LUAJIT_DIR)/lua51.dll $(BUILD_DIR)/bin/
	cp $(LUAJIT_DIR)/liblua51.dll.a $(BUILD_DIR)/bin/

mkdir_src: mkdir
	mkdir -p $(BUILD_DIR)/src
	cp infinitefilter/src/filter_runner.lua $(BUILD_DIR)/src/

mkdir_filters: mkdir
	mkdir -p $(BUILD_DIR)/filters
	cp $(FILTER_FILES) $(BUILD_DIR)/filters/

clean:
	rm -rf $(BUILD_DIR)