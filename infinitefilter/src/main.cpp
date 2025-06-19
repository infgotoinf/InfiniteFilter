// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.
#include "../../3rdparty/imgui/backends/imgui_impl_sdl2.h"
#include "../../3rdparty/imgui/backends/imgui_impl_sdlrenderer2.h"
#include "../../3rdparty/nfd/nfd.hpp"
#include "../../assets/fonts/ProggyVector.h"

#include "../headers/main_window_bar.h"


enum Filters {Invert};


typedef struct RGBAPixel_t
{
    uint8_t red   = 0;
    uint8_t green = 0;
    uint8_t blue  = 0;
    uint8_t alpha = 0;

} Pixel;



void renderWithTransparency(SDL_Renderer* renderer, ImGuiIO& io, int transparent_colorref)
{
    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, GetRValue(transparent_colorref), GetGValue(transparent_colorref), GetBValue(transparent_colorref), SDL_ALPHA_OPAQUE);
	SDL_RenderFillRect(renderer, NULL);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
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
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS);
    SDL_Window* window = SDL_CreateWindow("Infinite Filter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1, 1, window_flags);
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
    


    // Windows transparency setup
    static const COLORREF transparent_colorref = RGB(255, 0, 255);
#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
	HWND handle = wmInfo.info.win.window;
	if(!SetWindowLong(handle, GWL_EXSTYLE, GetWindowLong(handle, GWL_EXSTYLE) | WS_EX_LAYERED))
		fprintf(stderr, "SetWindowLong Error\n");
    if(!SetLayeredWindowAttributes(handle, transparent_colorref, 0, 1))
		fprintf(stderr, "SetLayeredWindowAttributes Error\n");
#endif


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

//---------------------------------------------------------------------------------
//          STYLE EDITING
//---------------------------------------------------------------------------------

    ImVec4* colors = ImGui::GetStyle().Colors;
    
    colors[ImGuiCol_TextDisabled]         = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_ChildBg]              = ImVec4(0.04f, 0.04f, 0.04f, 0.30f);
    colors[ImGuiCol_PopupBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border]               = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBg]              = ImVec4(0.22f, 0.22f, 0.22f, 0.50f);
    colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.50f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_FrameBgActive]        = ImVec4(0.64f, 0.64f, 0.64f, 0.60f);
    colors[ImGuiCol_TitleBg]              = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive]        = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]            = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.02f, 0.02f, 0.02f, 0.52f);
    colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.31f, 0.31f, 0.31f, 0.61f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.71f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.51f, 0.51f, 0.51f, 0.81f);
    colors[ImGuiCol_CheckMark]            = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Button]               = ImVec4(0.68f, 0.68f, 0.68f, 0.40f);
    colors[ImGuiCol_ButtonHovered]        = ImVec4(0.82f, 0.82f, 0.82f, 0.50f);
    colors[ImGuiCol_ButtonActive]         = ImVec4(0.96f, 0.96f, 0.96f, 0.60f);
    colors[ImGuiCol_Header]               = ImVec4(0.50f, 0.50f, 0.50f, 0.30f);
    colors[ImGuiCol_HeaderHovered]        = ImVec4(0.64f, 0.64f, 0.64f, 0.60f);
    colors[ImGuiCol_HeaderActive]         = ImVec4(0.96f, 0.96f, 0.96f, 0.70f);
    colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.82f, 0.82f, 0.82f, 0.00f);
    colors[ImGuiCol_SeparatorActive]      = ImVec4(0.96f, 0.96f, 0.96f, 0.00f);
    colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_TabHovered]           = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_Tab]                  = ImVec4(0.32f, 0.32f, 0.32f, 0.40f);
    colors[ImGuiCol_TabSelected]          = ImVec4(0.64f, 0.64f, 0.64f, 0.80f);

    ImGuiStyle& style = ImGui::GetStyle();
    style.GrabRounding   = 3.0f;
    style.FrameRounding  = 3.0f;

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    int width = dm.w;
    int height = dm.h; 
    SDL_SetWindowSize(window, width, height);
    SDL_SetWindowPosition(window, 0, 0);


    // Our state
    const char* filename = u8"../assets/MyImage01.jpg";
    SDL_Texture* my_texture;
    int my_image_width, my_image_height;
    bool ret = LoadTextureFromFile(filename, renderer, &my_texture, &my_image_width, &my_image_height);


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

//---------------------------------------------------------------------------------
//          WINDOW RAINBOW
//---------------------------------------------------------------------------------
        
        static float bright = 0.15f;
        static float old_bright = bright;
        static float step = 0.001f;
        static float R = bright;
        static float G = 0.0f;
        static float B = 0.0f;

        if      (R >= bright &&                B >  step  ) B -= step;
        else if (R >= bright && G <  bright && B <= step  ) G += step;
        else if (R >  step   && G >= bright               ) R -= step;
        else if (R <= step   && G >= bright && B <  bright) B += step;
        else if (               G >  step   && B >= bright) G -= step;
        else if (R <  bright && G <= step   && B >= bright) R += step;

        colors[ImGuiCol_WindowBg] = ImVec4(R, G, B, 1.0f);

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

        static int f = 0;

        ImGuiWindowFlags window_flags = 0;
        // window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        // window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_MenuBar;

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------

        ImGui::Begin("Image Render", nullptr, window_flags);
        if (ImGui::BeginMenuBar())
        {
            // File menu
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Файл";
                        case ENG:
                        default: return "File";
                    }
                }()))
            {
                ShowFileMenu(filename, &my_image_width, &my_image_height, my_texture, renderer);
                ImGui::EndMenu();
            }
            // Edit menu
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Правка";
                        case ENG:
                        default: return "Edit";
                    }
                }()))
            {
                ShowEditMenu();
                ImGui::EndMenu();
            }
            // Filter menu
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Фильтр";
                        case ENG:
                        default: return "Filter";
                    }
                }()))
            {
                ShowFilterMenu();
                ImGui::EndMenu();
            }
            // Settings menu
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Настройки";
                        case ENG:
                        default: return "Settings";
                    }
                }()))
            {
                ShowSettingsMenu();
                ImGui::EndMenu();
            }
            // Help menu
            if (ImGui::BeginMenu(
                [&]() -> const char* {
                    switch (language){
                        case RUS: return u8"Помощь";
                        case ENG:
                        default: return "Help";
                    }
                }()))
            {
                ShowHelpMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();

            if (show_demo_window)
                ImGui::ShowDemoWindow(&show_demo_window);
        }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        if (show_new_window)
        {
            ImGui::Begin("New", &show_new_window);
            ImGui::Text("...");
            ImGui::End();
        }
        if (show_config_window)
        { // Configuratuion window
            ImGui::Begin("Configuration", &show_config_window);
            ImGui::SeparatorText("Theme settings");
            if (ImGui::SliderFloat("Colour brightness", &bright, 0.0f, 0.5f, "%.2f"))
            {
                if (R >= old_bright) R = bright;
                if (G >= old_bright) G = bright;
                if (B >= old_bright) B = bright;
                old_bright = bright;
            };
            if (ImGui::SliderFloat("Changing step", &step, 0.0f, (bright > 0.1f ? 0.1f : bright)))
            {
                if (step > bright) step = bright;
            };
            ImGui::End();
        }
        if (show_credits_window)
        { // Credits window
            ImGui::Begin("Credits", &show_credits_window);
            ImGui::Text("...");
            ImGui::End();
        }
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        // ImGui::Text("pointer = %p", my_texture);
        // ImGui::Text("size = %d x %d", my_image_width, my_image_height);
        ImGui::Image((ImTextureID)(intptr_t)my_texture, ImVec2((float)my_image_width, (float)my_image_height));
        ImGui::SliderInt("Image size", &f, -10, 10);
        ImGui::Text("Image %d x %d", my_image_width, my_image_height);
        ImGui::End();
//*************************************************************************************************************************

        // Rendering
        renderWithTransparency(renderer, io, transparent_colorref);
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
