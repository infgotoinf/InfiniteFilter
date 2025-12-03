// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "../../3rdparty/imgui/backends/imgui_impl_sdl2.h"
#include "../../3rdparty/imgui/backends/imgui_impl_sdlrenderer2.h"
#include "../../assets/fonts/ProggyVector.h"

#define DEVELOPER_OPTIONS // Disable this for a release
#include "../headers/main_window_bar.hpp"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif


//=================================================================================================
//      START OF THE MAIN CODE
//=================================================================================================

int main(int, char**)
{

//-------------------------------------------------------------------------------------------------
//          SETUP
//-------------------------------------------------------------------------------------------------

    // Setup SDL
#ifdef _WIN32
    ::SetProcessDPIAware();
#endif
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return 1;
    }

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    float main_scale = ImGui_ImplSDL2_GetContentScaleForDisplay(0);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE
                                                   | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example"
                                        , SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED
                                        , (int)(1280 * main_scale), (int)(800 * main_scale)
                                        , window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC
                                                          | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    

    // Load Fonts
    ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    builder.BuildRanges(&ranges);
    // io.Fonts->AddFontFromFileTTF("..\\imgui\\misc\\fonts\\ProggyVector.ttf", 16.0f
    //                            , nullptr, ranges.Data);
    io.Fonts->AddFontFromMemoryCompressedTTF(ProggyVector_compressed_data
                                           , ProggyVector_compressed_size, 16.0f
                                           , nullptr, ranges.Data);

//-------------------------------------------------------------------------------------------------
//          STYLE EDITING
//-------------------------------------------------------------------------------------------------

    ImVec4* colors = ImGui::GetStyle().Colors;
    
    colors[ImGuiCol_TextDisabled]           = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.04f, 0.04f, 0.04f, 0.30f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.50f, 0.50f, 0.50f, 0.30f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.64f, 0.64f, 0.64f, 0.50f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.78f, 0.78f, 0.78f, 0.70f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.52f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 0.61f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 0.71f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 0.81f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.78f, 0.78f, 0.78f, 0.70f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.75f, 0.75f, 0.75f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.68f, 0.68f, 0.68f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.82f, 0.82f, 0.82f, 0.50f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.96f, 0.96f, 0.96f, 0.60f);
    colors[ImGuiCol_Header]                 = ImVec4(0.50f, 0.50f, 0.50f, 0.30f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.64f, 0.64f, 0.64f, 0.50f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.78f, 0.78f, 0.78f, 0.70f);
    colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.78f, 0.78f, 0.78f, 0.80f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.50f, 0.50f, 0.50f, 0.40f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
    colors[ImGuiCol_TabSelected]            = ImVec4(0.78f, 0.78f, 0.78f, 0.80f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);





    style.GrabRounding   = 3.0f;
    style.FrameRounding  = 3.0f;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

    // Making app to take entire screen
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    int display_width = dm.w;
    int display_height = dm.h; 
    SDL_SetWindowSize(window, display_width/2, display_height/2);
    SDL_SetWindowPosition(window, display_width/2, display_height/2);


    // Our state
    std::string filename = u8"assets/MyImage01.jpg";
    SDL_Texture* texture;
    int img_width, img_height, channels;
    bool ret = loadTextureFromFile(filename.c_str(), renderer, &texture, &img_width, &img_height);
    if (ret)
    {
        printf("Image loadead successfully!\n");
    }
    else
    {
        fprintf(stderr, "Error loading image!\n");
    }

//=================================================================================================
//      START OF THE MAIN LOOP
//=================================================================================================

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
            if (event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_CLOSE
                && event.window.windowID == SDL_GetWindowID(window))
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

//-------------------------------------------------------------------------------------------------
//          MAIN WINDOW
//-------------------------------------------------------------------------------------------------

        // static int f = 0;

        ImGuiWindowFlags main_window_flags = 0;
        main_window_flags = ImGuiWindowFlags_NoScrollbar  // Disable scrollbar
                          | ImGuiWindowFlags_NoDecoration
                          | ImGuiWindowFlags_NoMove
                          | ImGuiWindowFlags_NoSavedSettings
                          | ImGuiWindowFlags_NoBringToFrontOnFocus;


        static bool use_work_area = true;

        // We demonstrate using the full viewport area or the work area
        // (without menu-bars, task-bars etc.)
        // Based on your use case you may want one or the other.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

        ImGui::Begin("Image Render", nullptr, main_window_flags);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//          MENU BAR
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (ImGui::BeginMainMenuBar())
        {
            // File menu
            if (ImGui::BeginMenu("File"))
            {
                showFileMenu();
                ImGui::EndMenu();
            }
            // Edit menu
            if (ImGui::BeginMenu("Edit"))
            {
                showEditMenu();
                ImGui::EndMenu();
            }
            // Filter menu
            if (ImGui::BeginMenu("Filter"))
            {
                showFilterMenu(renderer, &texture);
                ImGui::EndMenu();
            }
            // Settings menu
            if (ImGui::BeginMenu("Settings"))
            {
                showSettingsMenu();
                ImGui::EndMenu();
            }
            // Help menu
            if (ImGui::BeginMenu("Help"))
            {
                showHelpMenu();
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//          MAIN WINDOW CONTENT
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Left
        ImGui::BeginGroup();
        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Description"))
            {
                //Left
                static int selected = 0;
                {
                    ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
                    static const char* item_names[] = { "Item One", "Item Two", "Item Three", "Item Four", "Item Five" };
                    for (int n = 0; n < IM_ARRAYSIZE(item_names); n++)
                    {
                        const char* item = item_names[n];
                        //if (ImGui::Selectable(item))
                        if (ImGui::TreeNode(item))
                        {
                            selected = n;
                            ImGui::Text("lol");
                            ImGui::TreePop();
                        }
                    
                        if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                        {
                            int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                            if (n_next >= 0 && n_next < IM_ARRAYSIZE(item_names))
                            {
                                item_names[n] = item_names[n_next];
                                item_names[n_next] = item;
                                ImGui::ResetMouseDragDelta();
                            }
                        }
                    }
                    // int num_of_elements = 100;
                    // for (int i = 0; i < num_of_elements; i++)
                    // {
                    //     char label[128];
                    //     sprintf(label, "MyObject %d", i);
                    //     if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                    //         selected = i;
                    
                    //     if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
                    //     {
                    //         int n_next = i + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
                    //         if (n_next >= 0 && n_next < num_of_elements)
                    //         {
                    //             // item_names[i] = item_names[n_next];
                    //             // item_names[n_next] = item;
                    //             ImGui::ResetMouseDragDelta();
                    //         }
                    //     }
                    // }
                    ImGui::EndChild();

                    ImGuiIO& io = ImGui::GetIO();
                    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
                                                  | ImGuiWindowFlags_AlwaysAutoResize
                                                  | ImGuiWindowFlags_NoSavedSettings
                                                  | ImGuiWindowFlags_NoFocusOnAppearing
                                                  | ImGuiWindowFlags_NoNav
                                                  | ImGuiWindowFlags_NoMove;
                    const float PAD = 10.0f;
                    const ImGuiViewport* viewport = ImGui::GetMainViewport();
                    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
                    ImVec2 work_size = viewport->WorkSize;
                    ImVec2 window_pos, window_pos_pivot;
                    window_pos.x = work_pos.x + work_size.x - PAD;
                    window_pos.y = work_pos.y + work_size.y - PAD;
                    window_pos_pivot.x = 1.0f;
                    window_pos_pivot.y = 1.0f;
                    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);

                    ImGui::SetNextWindowBgAlpha(0.5f); // Transparent background
                    if (ImGui::Begin("Image Info", NULL, window_flags))
                    {
                        ImGui::Text("%d x %d", img_width, img_height);
                        ImGui::Text(filename.c_str());
                        ImGui::End();
                    }

                    // ImGui::SliderInt("Image size", &f, -10, 10);
                    // ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2((float)img_width, (float)img_height));
        ImVec2 image_window_size = ImVec2(img_width * (ImGui::GetWindowSize().y / img_height) - 60
                                        , ImGui::GetWindowSize().y - 60);


                    ImGui::GetCursorScreenPos();
                    ImVec2 image_size = ImVec2((work_size.y), (work_size.y));

                    ImGui::BeginChild("right pane", ImVec2(0, 0), ImGuiChildFlags_Borders);
                    if (ImGui::BeginTable("images", 2))
                    {
                        for (int row = 0; row < 1; row++)
                        {
                            ImGui::TableNextRow();
                            for (int column = 0; column < 2; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                ImGui::Image((ImTextureID)(intptr_t)texture, image_size);
                            }
                        }
                        ImGui::EndTable();
                    }
                    ImGui::EndChild();
                }
                ImGui::EndTabItem();
            if (ImGui::BeginTabItem("Details"))
            {
                ImGui::Text("ID: 0123456789");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::EndChild();
        if (ImGui::Button("Revert")) {}
        ImGui::SameLine();
        if (ImGui::Button("Save")) {}
        ImGui::EndGroup();
    }

    ImGui::End();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//          CHILD WINDOWS
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        ImGui::SetNextWindowSize(ImVec2(500, 400));

    #ifdef DEVELOPER_OPTIONS
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);
    #endif

        if (show_fd_window)
            drawGui(&filename, &img_width, &img_height, &texture, renderer
                  , display_width, display_height);

        if (show_config_window)
        { // Configuratuion window
            ImGui::Begin("Configuration", &show_config_window, ImGuiWindowFlags_AlwaysAutoResize);
        }
        if (show_credits_window)
        { // Credits window
            ImGui::Begin("Credits", &show_credits_window, ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("...");
            ImGui::End();
        }

//-------------------------------------------------------------------------------------------------
//          RENDERING
//-------------------------------------------------------------------------------------------------

        // renderWithTransparency(renderer, io, transparent_colorref);
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255)
                                       , (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    if (texture) {
        SDL_DestroyTexture(texture);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
