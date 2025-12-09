#ifndef MAIN_WINDOW_BAR_
#define MAIN_WINDOW_BAR_

#include <string>
#include <vector>

#include <SDL2/SDL_render.h>

#include "../3rdparty/imgui/imgui.h"


inline bool show_fd_window = false;
#ifdef DEVELOPER_OPTIONS
inline bool show_demo_window = true;
#endif
inline bool show_config_window = false;
inline bool show_credits_window = false;
inline ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
inline std::vector<std::string> filter_stack_vec;

void exportGui(const char* filename, uint8_t* file_data, int* img_width, int* img_height
             , SDL_Texture* texture, SDL_Renderer* renderer);

void importGui(std::string* filename, uint8_t* file_data, int* img_width, int* img_height
             , SDL_Texture** texture, SDL_Renderer* renderer, int display_w, int display_h);


void showFileMenu();

void showEditMenu();

std::vector<std::string> getFilterNameVec();

void showFilterMenu(uint8_t* data, int img_width, int img_height, int img_channels
                  , SDL_Renderer* renderer, SDL_Texture** texture
                  , std::vector<std::string> flterNameVector);

void showSettingsMenu();

void showHelpMenu();


#endif // MAIN_WINDOW_BAR_