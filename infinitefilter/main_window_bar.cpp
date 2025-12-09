#include "include/main_window_bar.hpp"

#include "3rdparty/imgui/imgui.h"
#include "3rdparty/imgui_fd/ImGuiFileDialog.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBIW_WINDOWS_UTF8
#include "3rdparty/stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "3rdparty/stb/stb_image_resize2.h"

#include <filesystem>

#include "include/filter_handler.hpp"
#include "include/load_texture_impl.hpp"


namespace fs = std::filesystem;

// TODO: figure out with file import and export
void exportGui(const char* filename, uint8_t* file_data, int* img_width, int* img_height
             , SDL_Texture* texture, SDL_Renderer* renderer)
{ 
    // open Dialog Simple
    IGFD::FileDialogConfig config;config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File"
                                          , ".jpg,.png,.tga,.bmp,.hdr", config);
  
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) // => will show a dialog
    {
        if (ImGuiFileDialog::Instance()->IsOk()) // action if OK
        {
            //std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            filename = ImGuiFileDialog::Instance()->GetCurrentPath().c_str();
            std::string file_extension = filename;

            size_t i = file_extension.rfind('.', file_extension.length());
            file_extension.substr(i+1, file_extension.length() - i);
            if (file_extension == "png")
            {
                // TODO: figure out with channels's thing
                stbi_write_png(filename, *img_width, *img_height, 4, file_data, *img_width * 4);
            }
            if (file_extension == "jpg")
            {
                stbi_write_jpg(filename, *img_width, *img_height, 9, file_data, 100);
            }
            else {
                printf("Error: %s\n", file_extension);
            }
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        show_fd_window = false;
    }
}


void importGui(std::string* filename, uint8_t* file_data, int* img_width, int* img_height
             , SDL_Texture** texture, SDL_Renderer* renderer, int display_w, int display_h)
{ 
    // open Dialog Simple
    ImVec2 maxSize = ImVec2((float)display_w, (float)display_h);
    ImVec2 minSize = ImVec2((float)600, (float)350);
    IGFD::FileDialogConfig config;config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File"
                                          , ".jpg,.png,.tga,.bmp,.psd,.gif,.hdr,.pic", config);
  

    // display
    // => will show a dialog
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", 0, minSize, maxSize))
    {
        if (ImGuiFileDialog::Instance()->IsOk()) // action if OK
        {
            // Destroy previous texture before loading new one
            if (*texture)
            {
                SDL_DestroyTexture(*texture);
                *texture = nullptr;
            }
            *filename = ImGuiFileDialog::Instance()->GetFilePathName();
            int img_channels = 3;

            bool ret = loadTextureFromFile((*filename).c_str(), &file_data, renderer, texture
                                         , img_width, img_height, &img_channels);
            if (!ret)
            {
                fprintf(stderr, "Failed to load image: %s\n", filename);
                *img_width = 0;
                *img_height = 0;
            }
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        show_fd_window = false;
    }
}


//-------------------------------------------------------------------------------------------------
//          FILE MENU
//-------------------------------------------------------------------------------------------------

void showFileMenu()
{
    // Export
    if (ImGui::MenuItem("Export", "Ctrl+S"))
        {
            show_fd_window = true;
        }
    // Load
    if (ImGui::MenuItem("Load", "Ctrl+D"))
        {
            show_fd_window = true;
        }

    ImGui::Separator();
    // Quit
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

//-------------------------------------------------------------------------------------------------
//          EDIT MENU
//-------------------------------------------------------------------------------------------------

void showEditMenu()
{
 // Undo
 if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
 // Redo
 if (ImGui::MenuItem("Redo", "Ctrl+Shift+Z", false, false)) {} // Disabled item
 // Copy
 if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
 // Paste
 if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
}

//-------------------------------------------------------------------------------------------------
//          FILTER MENU
//-------------------------------------------------------------------------------------------------

std::vector<std::string> getFilterNameVec()
{
    std::vector<std::string> flter_name_vec;
    for (const auto& entry : fs::directory_iterator("./build/filters"))
    {
        flter_name_vec.push_back(entry.path().filename().string());
    }
    return flter_name_vec;
}

void showFilterMenu(uint8_t* data, int img_width, int img_height, int img_channels
                  , SDL_Renderer* renderer, SDL_Texture** texture
                  , std::vector<std::string> flter_name_vec)
{
    for (const auto& filter : flter_name_vec)
    {
        if (ImGui::MenuItem(filter.c_str()))
        {
            filterImage(data, img_width, img_height, img_channels, filter.c_str()
                      , renderer, texture);
            static int id = 0;
            std::string filter_name_with_id = filter + "##" + std::to_string(id);
            id++;
            filter_stack_vec.push_back(filter_name_with_id.c_str());
        }
    }
}

//-------------------------------------------------------------------------------------------------
//          SETTINGS MENU
//-------------------------------------------------------------------------------------------------

void showSettingsMenu()
{
    // Configuration
    if (ImGui::MenuItem("Configuration"))
    {
        show_config_window = true;
    }
}

//-------------------------------------------------------------------------------------------------
//          HELP MENU
//-------------------------------------------------------------------------------------------------

void showHelpMenu()
{
    // User Manual
    if (ImGui::MenuItem("User Manual"))
    {
        std::string url = "https://github.com/infgotoinf/InfiniteFilter/wiki";
        system((std::string("start \"\" \"") + url + std::string("\"")).c_str());
    }
    // Report a bug
    if (ImGui::MenuItem("Report a bug"))
    {
        std::string url = "https://github.com/infgotoinf/InfiniteFilter/issues/new";
        system((std::string("start \"\" \"") + url + std::string("\"")).c_str());
    }
    ImGui::Separator();
    // Credits
    if (ImGui::MenuItem("Credits"))
    {
        show_credits_window = true;
    }
#ifdef DEVELOPER_OPTIONS
    // Demo Window
    if (ImGui::Checkbox("Demo Window", &show_demo_window)) {}
    // Demo File Dialog
    if (ImGui::Checkbox("Demo File Dialog", &show_fd_window)) {}
#endif
}