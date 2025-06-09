#include "../../3rdparty/imgui/imgui.h"
#include <stdio.h>
#include <iostream>
#include <string>


enum Languages {ENG, RUS};

int language = ENG;
bool show_demo_window = true;
bool show_new_window = false;
bool show_config_window = false;
bool show_credits_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);




static void ShowFileMenu()
{
    // New..
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Новый..";
                case ENG:
                default:  return "New..";
            }
        }(), "Ctrl+N"))
        {
            show_new_window = true;
        }
    // Open..
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Открыть..";
                case ENG:
                default:  return "Open..";
            }
        }(), "Ctrl+O")) {}
    // Open Recent..
    if (ImGui::BeginMenu(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Открыть Новый..";
                case ENG:
                default:  return "Open Recent..";
            }
        }()))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        ImGui::EndMenu();
    }
    // Save
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Сохранить";
                case ENG:
                default:  return "Save";
            }
        }(), "Ctrl+S")) {}
    // Save As..
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Сохранить Как..";
                case ENG:
                default:  return "Save As..";
            }
        }(), "Ctrl+Shift+S")) {}

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
    // Cut
    ImGui::Separator();
    if (ImGui::MenuItem(
        [&]() -> const char* {
        switch (language){
            case RUS: return u8"Вырезать";
            case ENG:
            default:  return "Cut";
        }
    }(), "Ctrl+X")) {}
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



static void ShowSettingsWindow()
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