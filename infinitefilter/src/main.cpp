// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.
#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION
#define STBI_WINDOWS_UTF8
#ifdef _WIN32
#include <windows.h>
#endif
#include "../../3rdparty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../3rdparty/stb/stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../3rdparty/stb/stb_image_resize2.h"

#include "../../3rdparty/imgui/imgui.h"
#include "../../3rdparty/imgui/backends/imgui_impl_sdl2.h"
#include "../../3rdparty/imgui/backends/imgui_impl_sdlrenderer2.h"
#include "../../3rdparty/nfd/nfd.hpp"
#include <stdio.h>
#include <SDL.h>
#include "../../assets/fonts/ProggyVector.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

enum Languages {ENG, RUS};

int language = ENG;








bool LoadTextureFromMemory(const void* data, size_t data_size, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height)
{
    int image_width = 0;
    int image_height = 0;
    int channels = 4;
    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &image_width, &image_height, NULL, 4);
    if (image_data == nullptr)
    {
        fprintf(stderr, "Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }

    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom((void*)image_data, image_width, image_height, channels * 8, channels * image_width, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
    if (surface == nullptr)
    {
        fprintf(stderr, "Failed to create SDL surface: %s\n", SDL_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr)
        fprintf(stderr, "Failed to create SDL texture: %s\n", SDL_GetError());

    *out_texture = texture;
    *out_width = image_width;
    *out_height = image_height;

    SDL_FreeSurface(surface);
    stbi_image_free(image_data);

    return true;
}

// Open and read a file, then forward to LoadTextureFromMemory()
bool LoadTextureFromFile(const char* file_name, SDL_Renderer* renderer, SDL_Texture** out_texture, int* out_width, int* out_height)
{
#ifdef _WIN32
    // Convert UTF-8 to UTF-16 for Windows
    int wlen = MultiByteToWideChar(CP_UTF8, 0, file_name, -1, NULL, 0);
    wchar_t* wfilename = new wchar_t[wlen];
    MultiByteToWideChar(CP_UTF8, 0, file_name, -1, wfilename, wlen);
    
    FILE* f = _wfopen(wfilename, L"rb");
    delete[] wfilename;
#else
    FILE* f = fopen(file_name, "rb");
#endif
    
    if (f == nullptr) {
        fprintf(stderr, "Failed to open file: %s\n", file_name);
        return false;
    }
    
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    if (file_size == -1) {
        fclose(f);
        return false;
    }
    
    fseek(f, 0, SEEK_SET);
    unsigned char* file_data = new unsigned char[file_size];
    size_t read_size = fread(file_data, 1, file_size, f);
    fclose(f);
    
    if (read_size != static_cast<size_t>(file_size)) {
        delete[] file_data;
        return false;
    }
    
    bool ret = LoadTextureFromMemory(file_data, file_size, renderer, out_texture, out_width, out_height);
    delete[] file_data;
    return ret;
}









static void ShowFileMenu()
{
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Новый..";
                case ENG:
                default: return "New..";
            }
        }(), "Ctrl+N")) {}
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Открыть..";
                case ENG:
                default: return "Open..";
            }
        }(), "Ctrl+O")) {}
    if (ImGui::BeginMenu(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Открыть Новый..";
                case ENG:
                default: return "Open Recent..";
            }
        }()))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Сохранить";
                case ENG:
                default: return "Save";
            }
        }(), "Ctrl+S")) {}
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Сохранить Как..";
                case ENG:
                default: return "Save As..";
            }
        }(), "Ctrl+Shift+S")) {}

    ImGui::Separator();
    if (ImGui::MenuItem(
        [&]() -> const char* {
            switch (language){
                case RUS: return u8"Выйти";
                case ENG:
                default: return "Quit";
            }
        }(), "Alt+F4")) {}
}





// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return -1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return -1;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFontConfig config;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.BuildRanges(&ranges);
    // io.Fonts->AddFontFromFileTTF("..\\imgui\\misc\\fonts\\ProggyVector.ttf", 16.0f, nullptr, ranges.Data);
    io.Fonts->AddFontFromMemoryCompressedTTF(ProggyVector_compressed_data, ProggyVector_compressed_size, 16.0f, nullptr, ranges.Data);



    NFD::Guard nfdGuard;


    // Our state
    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    const char* filename = u8"../assets/MyImage01.jpg";
    SDL_Texture* my_texture;
    int my_image_width, my_image_height;
    bool ret = LoadTextureFromFile(filename, renderer, &my_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);




    // show the dialog
    NFD::UniquePathU8 outPath;
    nfdfilteritem_t filter[9] = {{"Image files", "jpg,png,tga,bmp,psd,gif,hdr,pic"},
                                {"JPG", "jpg"}, {"PNG", "png"}, {"TGA", "tga"}, {"BMP", "bmp"},
                                {"PSD", "psd"}, {"GIF", "gif"}, {"HDR", "hdr"}, {"PIC", "pic"}};




    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

//*************************************************************************************************************************

            if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Файл";
                        case ENG:
                        default: return "File";
                    }
                }()))
            {
                ShowFileMenu();
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Правка";
                        case ENG:
                        default: return "Edit";
                    }
                }()))
            {
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Отменить";
                        case ENG:
                        default: return "Undo";
                    }
                }(), "Ctrl+Z")) {}
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Повторить";
                        case ENG:
                        default: return "Redo";
                    }
                }(), "Ctrl+Shift+Z", false, false)) {} // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Вырезать";
                        case ENG:
                        default: return "Cut";
                    }
                }(), "Ctrl+X")) {}
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Копировать";
                        case ENG:
                        default: return "Copy";
                    }
                }(), "Ctrl+C")) {}
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Вставить";
                        case ENG:
                        default: return "Paste";
                    }
                }(), "Ctrl+V")) {}
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Настройки";
                        case ENG:
                        default: return "Settings";
                    }
                }()))
            {
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Настройка";
                        case ENG:
                        default: return "Configuration";
                    }
                }())) {}
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
                ImGui::EndMenu();
            }

            
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Помощь";
                        case ENG:
                        default: return "Help";
                    }
                }()))
            {
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Документация";
                        case ENG:
                        default: return "Documentation";
                    }
                }())) {}
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Сообщить об ошибке";
                        case ENG:
                        default: return "Report a bug";
                    }
                }())) {}
                ImGui::Separator();
                if (ImGui::MenuItem(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"О программе";
                        case ENG:
                        default: return "Credits";
                    }
                }())) {}
                if (ImGui::Checkbox(
                    [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Тестировочное окно";
                        case ENG:
                        default: return "Demo Window";
                    }
                }(), &show_demo_window)) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();

            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);
        }


        static int f = 0;

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_MenuBar;

        ImGui::Begin("Image Render", nullptr, window_flags);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Load")) {
                nfdresult_t result = NFD::OpenDialog(outPath, filter, 9);
                if (result == NFD_OKAY)
                {
                    // Destroy previous texture before loading new one
                    if (my_texture) {
                        SDL_DestroyTexture(my_texture);
                        my_texture = nullptr;
                    }
                
                    filename = outPath.get();
                    bool ret = LoadTextureFromFile(filename, renderer, &my_texture, &my_image_width, &my_image_height);
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
                bool ret = LoadTextureFromFile(filename, renderer, &my_texture, &my_image_width, &my_image_height);
                IM_ASSERT(ret);
            };
            if (ImGui::MenuItem("Export")) {

            };
            ImGui::EndMenuBar();
        }
        // ImGui::Text("pointer = %p", my_texture);
        // ImGui::Text("size = %d x %d", my_image_width, my_image_height);
        ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
        ImGui::SliderInt("Image size", &f, -10, 10);
        ImGui::Text("Image %d x %d", my_image_width, my_image_height);
        ImGui::End();
//*************************************************************************************************************************

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    if (my_texture) {
        SDL_DestroyTexture(my_texture);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
