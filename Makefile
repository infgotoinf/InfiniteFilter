#
# Cross Platform Makefile?
#
# To run project:
#   Linux:
#   Mac OS X:
#   UCRT64: (C:\msys64\ucrt64.exe)
#     cd your\\project\\folder
#
# To make this programm you need libstdc++ libgcc and lwinpthread
# make build=debug
#        OR
# make build=release
# 
# Use 'make clean' to clean build folder

CXX  = g++
#CXX = clang++

RELEASE = RELEASE_Infinite_Filter
DEBUG   = DEBUG_Infinite_Filter

IMGUI_DIR = ./imgui
SDL2_DIR  = ./SDL2
BUILD_DIR = ./build

SOURCES := main.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp $(IMGUI_DIR)/backends/imgui_impl_sdlrenderer2.cpp
SOURCES := $(basename $(notdir $(SOURCES)))
OBJS    := $(SOURCES:%=$(BUILD_DIR)/$(build)_%.o)
EXISTING_EXE := $(basename $(notdir $(shell find $(BUILD_DIR) -name '*.exe')))

CXXFLAGS         = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends -I$(SDL2_DIR)/include
RELEASE_CXXFLAGS = -Ofast -fno-rtti -flto -ffunction-sections -fdata-sections -Wl,--gc-sections
DEBUG_CXXFLAGS   = -g -Wall -W -Wshadow -Wformat
LDFLAGS = -L$(SDL2_DIR)/lib \
          -lmingw32 -lSDL2main -lSDL2 \
          -Wl,--dynamicbase -Wl,--nxcompat \
		      -static-libstdc++ -static-libgcc -static -lwinpthread \
          -lsetupapi -lhid -lwinmm -limm32 -lole32 -loleaut32 -lversion \
          -luuid -lgdi32 -lopengl32 -lbcrypt

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

all: mkdir $(BUILD_DIR)/$(EXE)
	@echo $(build) build complete

$(BUILD_DIR)/$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)


mkdir:
	mkdir -p build

clean:
	rm -rf $(BUILD_DIR)