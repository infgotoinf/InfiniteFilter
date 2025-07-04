#include "../../3rdparty/imgui/imgui.h"
#include "../../3rdparty/imgui_fd/ImGuiFileDialog.h"
#include <stdio.h>
#include <string>

#include "load_texture_impl.h"
#include "filter_handler.h"



enum Languages {ENG, RUS};

int language = ENG;
bool show_fd_window = false;
#ifdef DEVELOPER_OPTIONS
bool show_demo_window = true;
#endif
bool show_config_window = false;
bool show_credits_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



void drawGui2(const char* filename, int* img_width, int* img_height, SDL_Texture* texture, SDL_Renderer* renderer) { 
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


void drawGui(std::string* filename, int* img_width, int* img_height, SDL_Texture** texture, SDL_Renderer* renderer, int display_w, int display_h) { 
    // open Dialog Simple
    ImVec2 maxSize = ImVec2((float)display_w, (float)display_h);
    ImVec2 minSize = ImVec2((float)600, (float)350);
    IGFD::FileDialogConfig config;config.path = ".";
    ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".jpg,.png,.tga,.bmp,.psd,.gif,.hdr,.pic", config);
  

    // display
    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", 0, minSize, maxSize)) { // => will show a dialog
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            // Destroy previous texture before loading new one
            if (*texture) {
                SDL_DestroyTexture(*texture);
                *texture = nullptr;
            }
            *filename = ImGuiFileDialog::Instance()->GetFilePathName();

            bool ret = LoadTextureFromFile((*filename).c_str(), renderer, texture, img_width, img_height);
            if (!ret) {
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

// static void ShowEditMenu()
// {
    // // Undo
    // if (ImGui::MenuItem(
    //     [&]() -> const char* {
    //     switch (language){
    //         case RUS: return u8"Отменить";
    //         case ENG:
    //         default:  return "Undo";
    //     }
    // }(), "Ctrl+Z")) {}
    // // Redo
    // if (ImGui::MenuItem(
    //     [&]() -> const char* {
    //     switch (language){
    //         case RUS: return u8"Повторить";
    //         case ENG:
    //         default:  return "Redo";
    //     }
    // }(), "Ctrl+Shift+Z", false, false)) {} // Disabled item
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
// }

//---------------------------------------------------------------------------------
//          FILTER MENU
//---------------------------------------------------------------------------------

static void ShowFilterMenu(SDL_Renderer* renderer, SDL_Texture** texture)
{
    for (const auto& entry : fs::directory_iterator("./filters")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (ImGui::MenuItem(filename.c_str())) {
                filter_image(file_data, img_width, img_height, channels, filename.c_str(), renderer, texture);
            }
        }
    }
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
    // User Manual
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Руководство Пользователя";
            case ENG:
            default: return "User Manual";
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
#ifdef DEVELOPER_OPTIONS
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
#endif
}