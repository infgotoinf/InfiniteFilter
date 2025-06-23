#include "../../3rdparty/imgui/imgui.h"
#include "../../3rdparty/imgui_fd/ImGuiFileDialog.h"
#include <stdio.h>
#include <iostream>
#include <string>

#include "load_texture_impl.h"



enum Languages {ENG, RUS};

int language = ENG;
bool show_fd_window = false;
bool show_demo_window = true;
bool show_config_window = false;
bool show_credits_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



void drawGui2(const char* filename, int* my_image_width, int* my_image_height, SDL_Texture* my_texture, SDL_Renderer* renderer) { 
    // open Dialog Simple
    IGFD::FileDialogConfig config;config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg,.png,.tga,.bmp,.hdr", config);
  
    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) { // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            //std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            filename = ImGuiFileDialog::Instance()->GetCurrentPath().c_str();
            std::string file_extension = filename;

            size_t i = file_extension.rfind('.', file_extension.length());
            file_extension.substr(i+1, file_extension.length() - i);
            if (file_extension == "png")
            {
                // TODO: figure out with channels's thing
                stbi_write_png(filename, *my_image_width, *my_image_height, 4, file_data, *my_image_width * 4);
            }
            if (file_extension == "jpg")
            {
                stbi_write_jpg(filename, *my_image_width, *my_image_height, 9, file_data, 100);
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


void drawGui(std::string* filename, int* my_image_width, int* my_image_height, SDL_Texture** my_texture, SDL_Renderer* renderer, int display_w, int display_h) { 
    // open Dialog Simple
    ImVec2 maxSize = ImVec2((float)display_w, (float)display_h);
    ImVec2 minSize = ImVec2((float)600, (float)350);
    IGFD::FileDialogConfig config;config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg,.png,.tga,.bmp,.psd,.gif,.hdr,.pic", config);
  

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", 0, minSize, maxSize)) { // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            // Destroy previous texture before loading new one
            if (*my_texture) {
                SDL_DestroyTexture(*my_texture);
                *my_texture = nullptr;
            }
            *filename = ImGuiFileDialog::Instance()->GetFilePathName();

            bool ret = LoadTextureFromFile((*filename).c_str(), renderer, my_texture, my_image_width, my_image_height);
            if (!ret) {
                fprintf(stderr, "Failed to load image: %s\n", filename);
                *my_image_width = 0;
                *my_image_height = 0;
            }
        }

        // close
        ImGuiFileDialog::Instance()->Close();
        show_fd_window = false;
    }
}


//---------------------------------------------------------------------------------
//          FILE MENU
//---------------------------------------------------------------------------------

static void ShowFileMenu()
{
    // Export
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Экспортировать";
                case ENG:
                default:  return "Export";
            }
        }(), "Ctrl+S"))
        {
            show_fd_window = true;
        }
    // Load
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Загрузить";
                case ENG:
                default:  return "Load";
            }
        }(), "Ctrl+D"))
        {
            show_fd_window = true;
        }

    ImGui::Separator();
    // Quit
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Выйти";
                case ENG:
                default:  return "Quit";
            }
        }(), "Alt+F4")) {}
}

//---------------------------------------------------------------------------------
//          EDIT MENU
//---------------------------------------------------------------------------------

static void ShowEditMenu()
{
    // Undo
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Отменить";
            case ENG:
            default:  return "Undo";
        }
    }(), "Ctrl+Z")) {}
    // Redo
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Повторить";
            case ENG:
            default:  return "Redo";
        }
    }(), "Ctrl+Shift+Z", false, false)) {} // Disabled item
    // // Copy
    // if (ImGui::MenuItem(
    //     [&]() -> const char* {
    //     switch (language){
    //         case RUS: return u8"Копировать";
    //         case ENG:
    //         default:  return "Copy";
    //     }
    // }(), "Ctrl+C")) {}
    // // Paste
    // if (ImGui::MenuItem(
    //     [&]() -> const char* {
    //     switch (language){
    //         case RUS: return u8"Вставить";
    //         case ENG:
    //         default:  return "Paste";
    //     }
    // }(), "Ctrl+V")) {}
}

//---------------------------------------------------------------------------------
//          FILTER MENU
//---------------------------------------------------------------------------------

static void ShowFilterMenu()
{
    if (ImGui::MenuItem([&]() -> const char* {
        switch (language){
            case RUS: return u8"Инвертация цвета";
            case ENG:
            default: return "Invert";
        }
    }())) {}
}

//---------------------------------------------------------------------------------
//          SETTINGS MENU
//---------------------------------------------------------------------------------

static void ShowSettingsMenu()
{
    // Configuration
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Настройка";
            case ENG:
            default: return "Configuration";
        }
    }()))
    {
        show_config_window = true;
    }
    // Language
    if (ImGui::BeginMenu(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Язык";
            case ENG:
            default: return "Language";
        }
    }()))
    {
        if (ImGui::MenuItem("English")) {
            language = 0;
        }
        if (ImGui::MenuItem(u8"Русский")){
            language = 1;
        }
        ImGui::EndMenu();
    }
}

//---------------------------------------------------------------------------------
//          HELP MENU
//---------------------------------------------------------------------------------

static void ShowHelpMenu()
{
    // Documentation
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Документация";
            case ENG:
            default: return "Documentation";
        }
    }()))
    {
        std::string url = "https://github.com/infgotoinf/InfiniteFilter/wiki";
        system((std::string("start \"\" \"") + url + std::string("\"")).c_str());
    }
    // Report a bug
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Сообщить об ошибке";
            case ENG:
            default: return "Report a bug";
        }
    }()))
    {
        std::string url = "https://github.com/infgotoinf/InfiniteFilter/issues/new";
        system((std::string("start \"\" \"") + url + std::string("\"")).c_str());
    }
    ImGui::Separator();
    // Credits
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"О программе";
            case ENG:
            default: return "Credits";
        }
    }()))
    {
        show_credits_window = true;
    }
    // Demo Window
    if (ImGui::Checkbox(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Тестировочное окно";
            case ENG:
            default: return "Demo Window";
        }
    }(), &show_demo_window)) {}
    // Demo File Dialog
    if (ImGui::Checkbox(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Тестировочное файловое окно";
            case ENG:
            default: return "Demo File Dialog";
        }
    }(), &show_fd_window)) {}
}