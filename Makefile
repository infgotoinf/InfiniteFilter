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
## Making a build takes time, so launch.json may throw an error at the first run!
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

IMGUI_DIR     = ./include/imgui
BACKENDS_DIR  = ./include/backends/imgui
STB_DIR       = ./include/stb
BUILD_DIR     = ./build
SDL2_DIR      = C:/msys64/ucrt64/include/SDL2
FREETYPE_DIR  = C:/msys64/ucrt64/include/freetype2
FREETYPE_DIR2 = ./include/imgui/misc/freetype


SOURCES := main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp \
           $(IMGUI_DIR)/imgui_demo.cpp \
           $(IMGUI_DIR)/imgui_draw.cpp \
           $(IMGUI_DIR)/imgui_tables.cpp \
           $(IMGUI_DIR)/imgui_widgets.cpp \
           $(BACKENDS_DIR)/imgui_impl_sdl2.cpp \
           $(BACKENDS_DIR)/imgui_impl_sdlrenderer2.cpp \
           $(FREETYPE_DIR2)/imgui_freetype.cpp

SOURCES := $(basename $(notdir $(SOURCES)))
OBJS    := $(SOURCES:%=$(BUILD_DIR)/$(build)_%.o)


CXXFLAGS = -std=c++11 \
           -I$(IMGUI_DIR) \
           -I$(BACKENDS_DIR) \
           -I$(STB_DIR) \
           -I$(SDL2_DIR) \
           -I$(FREETYPE_DIR) \
           -I$(FREETYPE_DIR2)

RELEASE_CXXFLAGS = -g0 -O3 -w -DNDEBUG -flto -fno-rtti -fno-exceptions \
                   -ffunction-sections -fdata-sections -Wl,--gc-sections \
                   -fvisibility=hidden -fomit-frame-pointer -funroll-loops \
                   -fstrict-aliasing -fipa-pta -ftree-vectorize \
                   -fno-semantic-interposition -Wl,-O3 -Wl,--relax \
                   -Wl,--strip-all -mfpmath=both -mbranch-cost=2 \
                   -fno-stack-protector -fno-unwind-tables
                   # There are hell-a-lot-of stuff

DEBUG_CXXFLAGS = -g -g3 -O0

LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lfreetype -lpng -lharfbuzz -lgraphite2 \
          -ldwrite -lbrotlidec -lbrotlicommon -lbz2 -lz -lusp10 -lrpcrt4 \
          -Wl,--dynamicbase -Wl,--nxcompat \
          -static-libstdc++ -static-libgcc -static -lwinpthread \
          -lsetupapi -lhid -lwinmm -limm32 -lole32 -loleaut32 -lversion

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

$(BUILD_DIR)/$(build)_%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/misc/freetype/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<


all: mkdir $(BUILD_DIR)/$(EXE)
	@echo $(build) build complete

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


mkdir:
	mkdir -p build

clean:
	rm -rf $(BUILD_DIR)