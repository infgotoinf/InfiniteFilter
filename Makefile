#
# Cross Platform Makefile?
#
# To run project:
#   Linux:
#   Mac OS X:
#   UCRT64: (C:\msys64\ucrt64.exe)
#     cd your\\project\\folder
#
# You'll need freetype https://github.com/ocornut/imgui/tree/master/misc/freetype
# To make this programm for windows you need libstdc++, libgcc and lwinpthread (use UCRT64 to download them)
# make build=debug
#        OR
# make build=release (after building a release I compress it via UPX https://github.com/upx/upx)
# 
# Use 'make clean' to clean build folder

CXX  = g++
#CXX = clang++

RELEASE = RELEASE_Infinite_Filter
DEBUG   = DEBUG_Infinite_Filter

IMGUI_DIR = ./imgui
SDL2_DIR  = ./SDL2
BUILD_DIR = ./build
FREETYPE_DIR = ./imgui/misc/freetype

FREETYPE_INCLUDE_DIR = C:/msys64/ucrt64/include/freetype2
FREETYPE_LIB_DIR = C:/msys64/ucrt64/lib

SOURCES := main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp \
           $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp
SOURCES += $(FREETYPE_DIR)/imgui_freetype.cpp
SOURCES := $(basename $(notdir $(SOURCES)))
OBJS    := $(SOURCES:%=$(BUILD_DIR)/$(build)_%.o)
EXISTING_EXE := $(basename $(notdir $(shell find $(BUILD_DIR) -name '*.exe')))

CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(SDL2_DIR)/include \
           -I$(FREETYPE_DIR) -I$(FREETYPE_INCLUDE_DIR)
RELEASE_CXXFLAGS = -g0 -O3 -DNDEBUG -flto -fno-rtti -fno-exceptions -ffunction-sections \
                   -fdata-sections -Wl,--gc-sections -fvisibility=hidden -fomit-frame-pointer \
                   -funroll-loops -fstrict-aliasing -fipa-pta -ftree-vectorize \
                   -fno-semantic-interposition -Wl,-O3 -Wl,--relax -Wl,--strip-all -mfpmath=both \
                   -mbranch-cost=2 -fno-stack-protector -fno-unwind-tables # There are hell-a-lot-of stuff
DEBUG_CXXFLAGS = -g3 -O0 -Wall -Wextra -Wpedantic -Wshadow -Wformat=2 -Wundef \
                 -fno-omit-frame-pointer -fno-inline -fno-common \
                 -Wcast-qual -Wduplicated-cond -Wlogical-op
LDFLAGS = -L$(SDL2_DIR)/lib -L$(FREETYPE_LIB_DIR) \
          -lmingw32 -lSDL2main -lSDL2 -lfreetype \
          -Wl,--dynamicbase -Wl,--nxcompat \
		      -static-libstdc++ -static-libgcc -static -lwinpthread \
          -lsetupapi -lhid -lwinmm -limm32 -lole32 -loleaut32 -lversion

##---------------------------------------------------------------------
## DEBUG AND RELEASE REALISATION
##---------------------------------------------------------------------

ifeq ($(build), debug)
	CXXFLAGS += $(DEBUG_CXXFLAGS)
	EXE = $(DEBUG)
endif
ifeq ($(build), release)
	CXXFLAGS += $(RELEASE_CXXFLAGS)
	EXE = $(RELEASE)
endif

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

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

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(BUILD_DIR)/$(build)_%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(build)_%.o:$(FREETYPE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: mkdir $(BUILD_DIR)/$(EXE)
	@echo $(build) build complete

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


mkdir:
	mkdir -p build

clean:
	rm -rf $(BUILD_DIR)