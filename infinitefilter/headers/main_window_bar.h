#include "../../3rdparty/imgui/imgui.h"
#include "../../3rdparty/imgui_fd/ImGuiFileDialog.h"
#include <stdio.h>
#include <iostream>
#include <string>

#include "load_texture_impl.h"



enum Languages {ENG, RUS};

int language = ENG;
bool show_demo_window = true;
bool show_new_window = false;
bool show_config_window = false;
bool show_credits_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


NFD::UniquePathU8 outPath;
nfdfilteritem_t import_filter[9] = {{"Image files", "jpg,png,tga,bmp,psd,gif,hdr,pic"},
                                    {"JPG", "jpg"}, {"PNG", "png"}, {"TGA", "tga"}, {"BMP", "bmp"},
                                    {"PSD", "psd"}, {"GIF", "gif"}, {"HDR", "hdr"}, {"PIC", "pic"}};
nfdfilteritem_t export_filter[5] = {{"JPG", "jpg"}, {"PNG", "png"}, {"TGA", "tga"},
                                    {"BMP", "bmp"}, {"HDR", "hdr"}};



static void ShowFileMenu(const char* filename, int* my_image_width, int* my_image_height, SDL_Texture* my_texture, SDL_Renderer* renderer)
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
            nfdresult_t result = NFD::SaveDialog(outPath, export_filter, 5);
            if (result == NFD_OKAY)
            {
                filename = outPath.get();
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
            else if (result == NFD_CANCEL)
            {
                puts("User pressed cancel.");
            }
            else 
            {
                printf("Error: %s\n", NFD::GetError());
            }
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
           nfdresult_t result = NFD::OpenDialog(outPath, import_filter, 9);
            if (result == NFD_OKAY)
            {
                // Destroy previous texture before loading new one
                if (my_texture) {
                    SDL_DestroyTexture(my_texture);
                    my_texture = nullptr;
                }
            
                filename = outPath.get();
                bool ret = LoadTextureFromFile(filename, renderer, &my_texture, my_image_width, my_image_height);
                if (!ret) {
                    fprintf(stderr, "Failed to load image: %s\n", filename);
                }
            }
            else if (result == NFD_CANCEL)
            {
                puts("User pressed cancel.");
            }
            else 
            {
                printf("Error: %s\n", NFD::GetError());
            }
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
    // Copy
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Копировать";
            case ENG:
            default:  return "Copy";
        }
    }(), "Ctrl+C")) {}
    // Paste
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Вставить";
            case ENG:
            default:  return "Paste";
        }
    }(), "Ctrl+V")) {}
}


static void ShowFilterMenu()
{
    if (ImGui::BeginMenu(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Настройка";
            case ENG:
            default: return "Configuration";
        }
    }()))
    {
        if (ImGui::MenuItem("Invert")) {}
        ImGui::EndMenu();
    }
}


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
}