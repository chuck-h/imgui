// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <stdlib.h>         // NULL, malloc, free, atoi
#include <math.h>           // sqrtf, powf, cosf, sinf, floorf, ceilf


// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Console / ShowExampleAppConsole()
//-----------------------------------------------------------------------------

// Demonstrate creating a simple console window, with scrolling, filtering, completion and history.
// For the console example, here we are using a more C++ like approach of declaring a class to hold the data and the functions.

char* gen_symbol(char* symbol_name) {
    const char* extension = ".lib";
    char* lib_name;
    lib_name == malloc(strlen(symbol_name) + 5);
    strcpy(lib_name, symbol_name);
    strcat(lib_name, extension);
    return lib_name;
    free(lib_name);
}

void write_header(FILE* fp, char* symbol_name, char* designator) {
    fprintf(fp, "EESchema-LIBRARY Version 2.4\n");
    fprintf(fp, "DEF %s %s 0 40 Y Y 1 F N\n", symbol_name, designator);
    fprintf(fp, "F0 \"%s\" 0 50 50 H V C CNN\n", designator);
    fprintf(fp, "F1 \"%s\" 0 -50 50 H V C CNN\n", symbol_name);
    fprintf(fp, "DRAW\n");
}

void end_symbol(FILE* fp) {
    fprintf(fp, "ENDDRAW\n");
    fprintf(fp, "ENDDEF\n");
    fclose(fp);
}

void draw_lrtb_sym(FILE *fp, int unit_number, int symbol_width_default, int symbol_line_width, int pin_distance, int pin_width, int l_pins, int r_pins, int t_pins, int b_pins,int text_width, int text_height) {

    int symbol_height, symbol_width, x1, y1, x2, y2, x3, y3, x4, y4, i, j, k, l;

    if (l_pins > r_pins) {
        symbol_height = (pin_distance * l_pins) + pin_distance;
    }
    else {
        symbol_height = (pin_distance * r_pins) + pin_distance;
    }

    if (t_pins > b_pins) {
        symbol_width = (pin_distance * t_pins) + pin_distance;
        if (symbol_width > symbol_width_default) {
            symbol_width = symbol_width;
        }
        else {
            symbol_width = symbol_width_default;
        }
    }
    else if (t_pins == 0 && b_pins == 0) {
        symbol_width = symbol_width_default;
    }
    else {
        symbol_width = (pin_distance * b_pins) + pin_distance;
        if (symbol_width > symbol_width_default) {
            symbol_width;
        }
        else {
            symbol_width = symbol_width_default;
        }
    }

    x1 = (0 - (symbol_width / 2));
    y1 = (0 + (symbol_height / 2));
    x2 = (0 - (symbol_width / 2));
    y2 = (0 - (symbol_height / 2));
    x3 = (0 + (symbol_width / 2));
    y3 = (0 + (symbol_height / 2));
    x4 = (0 + (symbol_width / 2));
    y4 = (0 - (symbol_height / 2));

    fprintf(fp, "P 2 %d 0 %d %d %d %d %d N\n", unit_number, symbol_line_width, x1, y1, x2, y2);
    fprintf(fp, "P 2 %d 0 %d %d %d %d %d N\n", unit_number, symbol_line_width, x2, y2, x4, y4);
    fprintf(fp, "P 2 %d 0 %d %d %d %d %d N\n", unit_number, symbol_line_width, x4, y4, x3, y3);
    fprintf(fp, "P 2 %d 0 %d %d %d %d %d N\n", unit_number, symbol_line_width, x3, y3, x1, y1);

    int total_pins = l_pins + r_pins + t_pins + b_pins;
    int rstart = l_pins + b_pins + r_pins;

    if (t_pins == 0 && b_pins == 0) {
        for (i = 1; i <= l_pins; i++) {
            fprintf(fp, "X ~ %d %d %d %d R %d %d %d 0 B\n",i, (x1 - pin_width), y1 - (pin_distance*i), pin_width, text_width, text_height, unit_number);
        }
        for (j = 1; j <= r_pins; j++) {
            fprintf(fp, "X ~ %d %d %d %d L %d %d %d 0 B\n",l_pins+j, (x3 + pin_width), y3 - (pin_distance*j), pin_width, text_width, text_height, unit_number);
        }
    }
    else {
        for (i = 1; i <= l_pins; i++) {
            fprintf(fp, "X ~ %d %d %d %d R %d %d %d 0 B\n", i, (x1 - pin_width), y1 - (pin_distance*i), pin_width, text_width, text_height, unit_number);
        }
        for (j = 1; j <= r_pins; j++) {
            fprintf(fp, "X ~ %d %d %d %d L %d %d %d 0 B\n", rstart--, (x3 + pin_width), y3 - (pin_distance*j), pin_width, text_width, text_height, unit_number);
        }
        for (k = 1; k <= t_pins; k++) {
            fprintf(fp, "X ~ %d %d %d %d D %d %d %d 0 B\n", total_pins--, (x1 + (pin_distance*k)), (y1 + pin_width), pin_width, text_width, text_height, unit_number);
        }
        for (l = 1; l <= b_pins; l++) {
            fprintf(fp, "X ~ %d %d %d %d U %d %d %d 0 B\n", ++l_pins, (x2 + (pin_distance*l)), (y2 - pin_width), pin_width, text_width, text_height, unit_number);
        }
    }
}

static float mils_to_pix(int value) {
    float mils;
    mils = value / 10.416;
    return mils;
}

static float mm_to_pix(int value) {
    float mm;
    mm = value / 0.264;
    return mm;
}


void ToggleButton(const char* str_id, bool* v)
{
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float height = ImGui::GetFrameHeight();
    float width = height * 1.55f;
    float radius = height * 0.50f;

    if (ImGui::InvisibleButton(str_id, ImVec2(width, height)))
        *v = !*v;
    ImU32 col_bg;
    if (ImGui::IsItemHovered())
        col_bg = *v ? IM_COL32(145 + 20, 211, 68 + 20, 255) : IM_COL32(218 - 20, 218 - 20, 218 - 20, 255);
    else
        col_bg = *v ? IM_COL32(145, 211, 68, 255) : IM_COL32(218, 218, 218, 255);

    draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
    draw_list->AddCircleFilled(ImVec2(*v ? (p.x + width - radius) : (p.x + radius), p.y + radius), radius - 1.5f, IM_COL32(255, 255, 255, 255));
}

static void preview_draw_lr_sym(int symbol_width, int pin_distance, int pin_width, int l_pins, int r_pins, char * symbol_name, char* symbol_descriptor) {
    int x1, x2, x3, x4, y1, y2, y3, y4, i, j, symbol_height;
    static int func_type = 0, display_count = 70;

    const ImVec2 position = ImGui::GetCursorScreenPos();
    const ImVec2 size = ImGui::GetContentRegionAvail();
    
    static float sz = 5.0f;
    ImDrawCornerFlags corners_none = 0;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    const ImU32 col = ImColor(colf);
    
    if (l_pins > r_pins) {
        symbol_height = (pin_distance * l_pins) + pin_distance;
    }
    else {
        symbol_height = (pin_distance * r_pins) + pin_distance;
    }
    x1 = (0 - (symbol_width / 2));
    y1 = (0 + (symbol_height / 2));
    x2 = (0 - (symbol_width / 2));
    y2 = (0 - (symbol_height / 2));
    x3 = (0 + (symbol_width / 2));
    y3 = (0 + (symbol_height / 2));
    x4 = (0 + (symbol_width / 2));
    y4 = (0 - (symbol_height / 2));


    float x_origin = (position.x + (size.x/2)) - (symbol_width/2) , y_origin =  ((size.y / 2) + position.y) - (symbol_height/2);
    
    ImGui::Text("%f", position.x);
    ImGui::Text("%f", position.y);
    ImGui::Text("%f", size.x);
    ImGui::Text("%f", size.y);

    float rect_x1 = x_origin - x1 - (symbol_width / 2);
    float rect_y1 = y_origin + y1 - (symbol_height / 2);

    float rect_x3 = x_origin - x1 + (symbol_width / 2);
    float rect_y3 = y_origin + y1 + (symbol_height / 2);
 
    draw_list->AddRect(ImVec2(rect_x1 , rect_y1), ImVec2(rect_x3 , rect_y3), col, 0.0f, corners_none, 1.0f);
    draw_list->AddText(ImVec2(rect_x1, rect_y1 - 25), col, symbol_name);
    draw_list->AddText(ImVec2(rect_x1, rect_y1 - 15), col, symbol_descriptor);

    for (i = 1; i <= l_pins; i++) {

        draw_list->AddLine(ImVec2(rect_x1 - (pin_width), rect_y1 + (pin_distance*i)), ImVec2(rect_x1, rect_y1 + (pin_distance*i)), col, 1.0f);
        draw_list->AddText(ImVec2(rect_x1 -15, rect_y1 + (pin_distance*i) - 15), col,"1" );
    }
    for (j = 1; j <= r_pins; j++) {
        
        draw_list->AddLine(ImVec2(rect_x1 + symbol_width , rect_y3 - symbol_height + (pin_distance*j)), ImVec2(rect_x1 + symbol_width + (pin_width), rect_y3 - symbol_height + (pin_distance*j)), col, 1.0f);

    }
}

static void preview_draw_lrtb_sym(int symbol_width_default, int pin_distance, int pin_width, int l_pins, int r_pins, int t_pins, int b_pins, char * symbol_name, char* symbol_descriptor, bool show_pin_number) {
    int x1, x2, x3, x4, y1, y2, y3, y4, i, j,k,l, symbol_height, symbol_width;
    static int func_type = 0, display_count = 70;

    const ImVec2 position = ImGui::GetCursorScreenPos();
    //const ImVec2 position = ImGui::GetCursorStartPos();
    const ImVec2 size = ImGui::GetContentRegionAvail();

    static float sz = 5.0f;
    ImDrawCornerFlags corners_none = 0;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    static ImVec4 colf = ImVec4(1.0f, 1.0f, 0.4f, 1.0f);
    const ImU32 col = ImColor(colf);

    if (l_pins > r_pins) {
        symbol_height = (pin_distance * l_pins) + pin_distance;
    }
    else {
        symbol_height = (pin_distance * r_pins) + pin_distance;
    }

    if (t_pins > b_pins) {
        symbol_width = (pin_distance * t_pins) + pin_distance;
        if (symbol_width > symbol_width_default) {
            symbol_width = symbol_width;
        }
        else {
            symbol_width = symbol_width_default;
        }
    }
    else if (t_pins == 0 && b_pins == 0) {
        symbol_width = symbol_width_default;
    }
    else {
        symbol_width = (pin_distance * b_pins) + pin_distance;
        if (symbol_width > symbol_width_default) {
            symbol_width;
        }
        else {
            symbol_width = symbol_width_default;
        }
    }

    x1 = (0 - (symbol_width / 2));
    y1 = (0 + (symbol_height / 2));
    x2 = (0 - (symbol_width / 2));
    y2 = (0 - (symbol_height / 2));
    x3 = (0 + (symbol_width / 2));
    y3 = (0 + (symbol_height / 2));
    x4 = (0 + (symbol_width / 2));
    y4 = (0 - (symbol_height / 2));


    float x_origin = (position.x + (size.x / 2)) - (symbol_width / 2), y_origin = ((size.y / 2) + position.y) - (symbol_height / 2);

    float rect_x1 = x_origin - x1 - (symbol_width / 2);
    float rect_y1 = y_origin + y1 - (symbol_height / 2);

    float rect_x3 = x_origin - x1 + (symbol_width / 2);
    float rect_y3 = y_origin + y1 + (symbol_height / 2);

    draw_list->AddRect(ImVec2(rect_x1, rect_y1), ImVec2(rect_x3, rect_y3), col, 0.0f, corners_none, 1.0f);
    draw_list->AddText(ImVec2(rect_x1, rect_y1 - pin_width - 25), col, symbol_name);
    draw_list->AddText(ImVec2(rect_x1, rect_y1 - pin_width - 15), col, symbol_descriptor);

    char c[5];
    int total_pins = l_pins + r_pins + t_pins + b_pins;
    int rstart = l_pins + b_pins + r_pins;

    if (t_pins == 0 && b_pins == 0) {
        for (i = 1; i <= l_pins; i++) {
            draw_list->AddLine(ImVec2(rect_x1 - (pin_width), rect_y1 + (pin_distance*i)), ImVec2(rect_x1, rect_y1 + (pin_distance*i)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", i);
                draw_list->AddText(ImVec2(rect_x1 - 15, rect_y1 + (pin_distance*i) - 15), col, c);
            }

        }
        for (j = 1; j <= r_pins; j++) {
            draw_list->AddLine(ImVec2(rect_x1 + symbol_width, rect_y3 - symbol_height + (pin_distance*j)), ImVec2(rect_x1 + symbol_width + (pin_width), rect_y3 - symbol_height + (pin_distance*j)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", j + l_pins + b_pins);
                draw_list->AddText(ImVec2(rect_x1 + symbol_width + 5, rect_y3 - symbol_height + (pin_distance*j) - 15), col, c);
            }
        }
    }

    else {
        for (i = 1; i <= l_pins; i++) {
            draw_list->AddLine(ImVec2(rect_x1 - (pin_width), rect_y1 + (pin_distance*i)), ImVec2(rect_x1, rect_y1 + (pin_distance*i)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", i);
                draw_list->AddText(ImVec2(rect_x1 - 15, rect_y1 + (pin_distance*i) - 15), col, c);
            }
        }
        for (j = 1; j <= r_pins; j++) {
            draw_list->AddLine(ImVec2(rect_x1 + symbol_width, rect_y3 - symbol_height + (pin_distance*j)), ImVec2(rect_x1 + symbol_width + (pin_width), rect_y3 - symbol_height + (pin_distance*j)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", rstart--);
                draw_list->AddText(ImVec2(rect_x1 + symbol_width + 5, rect_y3 - symbol_height + (pin_distance*j) - 15), col, c);
            }
        }
        for (k = 1; k <= t_pins; k++) {
            draw_list->AddLine(ImVec2(rect_x1 + (pin_distance*k), rect_y1), ImVec2(rect_x1 + ((pin_distance*k)), rect_y1 - (pin_width)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", total_pins--);
                draw_list->AddTextVertical(ImVec2(rect_x1 - 15 + (pin_distance*k), rect_y1 - 15), col, c, true);
            }
        }
        for (l = 1; l <= b_pins; l++) {
            draw_list->AddLine(ImVec2(rect_x3 - symbol_width + (pin_distance*l), rect_y3), ImVec2(rect_x3 - symbol_width + (pin_distance*l), rect_y3 + (pin_width)), col, 1.0f);
            if (show_pin_number == true) {
                sprintf(c, "%d", ++l_pins);
                draw_list->AddTextVertical(ImVec2(rect_x3 - symbol_width + (pin_distance*l) - 15, rect_y3 + 10), col, c, true);
            }
        }
    }
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 780, "KiCad Symbol Generator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    bool show_demo_window = false;
    bool show_another_window = false;
    bool show_preview_window = true;
    bool show_symbol_type_window = true;
    bool show_generate_window = true;
    bool show_console_window = false;
    //static int gererate = 0;
    static bool generate = false;
    

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);       
        {
            static bool no_collapse = true;
            static bool no_close = true;
            static float f = 0.0f;
            static int counter = 0;
            const ImU8    u8_zero = 0, u8_one = 1, u8_fifty = 50, u8_min = 1, u8_max = 255;
            static bool inputs_step = true;
            static int no_of_units = 1;
            static int uno = 1;
            static int symbol_width = 800;
            static char designator[128] = "";
            static char symbolName[128] = "";
            static bool show_pin_number = true;
            static bool incpins = true;
            bool generated = false;

            {
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::SetNextWindowSize(ImVec2(420, 200));
                ImGui::Begin("Symbol Properties");                          
                ImGui::Columns(2, "mycolumns3", false);  
               // Symbol Name entities
                ImGui::Text("Symbol Name *");
                ImGui::SameLine();
                ImGui::NextColumn();                
                ImGui::InputText("##symbolName", symbolName, IM_ARRAYSIZE(symbolName));
                ImGui::SameLine(); HelpMarker("Enter the name of the symbol you wish to create");
                ImGui::NextColumn();

                // Symbol descriptor
                ImGui::Text("Default reference designator *");
                ImGui::SameLine();
                ImGui::NextColumn();
                
                ImGui::InputText("##designator", designator, IM_ARRAYSIZE(designator));
                ImGui::SameLine(); HelpMarker("Enter the name of the symbol's default designator");
                ImGui::NextColumn();
            }

            
            ImGui::Text("Symbol Type");
            ImGui::NextColumn();
            const char* symbol_type[] = { "Single Unit","Multi Unit" };
            static const char* current_symbol_type = symbol_type[0];            
            static ImGuiComboFlags current_symbol_flags = 0;
            if (ImGui::BeginCombo("##combo_1", current_symbol_type, current_symbol_flags)) 
            {
                for (int n = 0; n < IM_ARRAYSIZE(symbol_type); n++)
                {
                    bool is_selected = (current_symbol_type == symbol_type[n]);
                    if (ImGui::Selectable(symbol_type[n], is_selected))
                        current_symbol_type = symbol_type[n];
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();   
                }
                ImGui::EndCombo();
            }
            if (current_symbol_type == "Multi Unit") {
                ImGui::NextColumn();
                ImGui::Text("Number Of Units per package");                               
                ImGui::SameLine();
                ImGui::NextColumn();
                ImGui::SliderInt("##units_items", &no_of_units, 1, 255, "%d");
                ImGui::SameLine(); HelpMarker("Select Number of units in Part.\n");
            }
            if (current_symbol_type == "Single Unit") {
                no_of_units = 1;
            }
            //metric units for symbol

            ImGui::NextColumn();
            ImGui::Text("Metric Units");
            ImGui::NextColumn();
            const char* metric_units[] = { "Mils","Millimeter" };
            static const char* current_metric_units = metric_units[0];            
            static ImGuiComboFlags unit_flags = 0;
            if (ImGui::BeginCombo("##combo_2", current_metric_units, unit_flags)) 
            {
                for (int nm = 0; nm < IM_ARRAYSIZE(metric_units); nm++)
                {
                    bool unit_selected = (current_metric_units == metric_units[nm]);
                    if (ImGui::Selectable(metric_units[nm], unit_selected))
                        current_metric_units = metric_units[nm];
                    if (unit_selected)
                        ImGui::SetItemDefaultFocus();  
                }
                ImGui::EndCombo();                
            }

            //font width
            static int text_width = 60;
            ImGui::NextColumn();
            ImGui::Text("Text Width");
            ImGui::NextColumn();
            ImGui::InputU8Int("##text_width", &text_width,10);

            //font height
            static int text_height = 60;
            ImGui::NextColumn();
            ImGui::Text("Text Height");
            ImGui::NextColumn();
            ImGui::InputU8Int("##text_hidth", &text_height, 10);

            //enable preview
            ImGui::NextColumn();
            ImGui::Text("Show Symbol Preview");
            ImGui::NextColumn();
            ImGui::Checkbox("##Symbol_preview", &show_preview_window);
            ImGui::NextColumn();

            ImGui::End();

            if (show_symbol_type_window)
            {
                static bool no_close = true;
                static bool no_collapse = true;
                ImGuiWindowFlags syboltypeflag;
                syboltypeflag |= ImGuiWindowFlags_NoCollapse;                
                
                ImGui::SetNextWindowPos(ImVec2(0, 240));
                ImGui::SetNextWindowSize(ImVec2(420, 320));
                ImGui::Begin("Symbol Type",NULL);  
                ImGui::Columns(2, "mycolumns4", false);
                ImGui::Text("Unit   Number");
                ImGui::NextColumn();
                
                ImGui::SliderInt("##units",&uno,1,no_of_units,"%d");
                ImGui::NextColumn();
                ImGui::Text("Symbol Outline");
                ImGui::NextColumn();
                const char* symbol_outline_type[] = { "Rectangle","Triangle","Rectangle Segmented","DeMorgan" };
                static const char* current_symbol_outline = symbol_outline_type[0];
                static ImGuiComboFlags symbol_type_flags = 0;
                if (ImGui::BeginCombo("##combo_3", current_symbol_outline, symbol_type_flags)) 
                {
                    for (int nm = 0; nm < IM_ARRAYSIZE(symbol_outline_type); nm++)
                    {
                        bool outline_selected = (current_symbol_outline == symbol_outline_type[nm]);
                        if (ImGui::Selectable(symbol_outline_type[nm], outline_selected))
                            current_symbol_outline = symbol_outline_type[nm];
                        if (outline_selected)
                            ImGui::SetItemDefaultFocus();  
                    }
                    ImGui::EndCombo();
                }

                static int symbol_thickness = 5;
                ImGui::NextColumn();
                ImGui::Text("Thickness of %s",current_symbol_outline);
                ImGui::NextColumn();
                ImGui::InputU8Int("##Outline_thickness", &symbol_thickness);
                ImGui::SameLine(); HelpMarker("Enter thickness of Symbol Outline");

                static int pin_distance = 200;
                ImGui::NextColumn();
                ImGui::Text("Distance between Pins (%s)", current_metric_units);
                ImGui::NextColumn();
                ImGui::InputU16Int("##Pin_distance", &pin_distance, 10);
                ImGui::SameLine(); HelpMarker("Enter distance between pins");

                static int pin_width = 200;
                ImGui::NextColumn();
                ImGui::Text("Pin width (%s)", current_metric_units);
                ImGui::NextColumn();
                ImGui::InputU16Int("##Pin_width", &pin_width, 10);
                ImGui::SameLine(); HelpMarker("Enter pin width");

                static int left_pins = 5;
                ImGui::NextColumn();
                ImGui::Text("Left Pins");
                ImGui::NextColumn();
                ImGui::InputU8Int("##left_pins", &left_pins);
                ImGui::SameLine(); HelpMarker("Enter the number of pins on the left hand side of the Symbol");

                static int right_pins = 5;
                ImGui::NextColumn();
                ImGui::Text("Right Pins");
                ImGui::NextColumn();
                ImGui::InputU8Int("##Right_pins", &right_pins);
                ImGui::SameLine(); HelpMarker("Enter the number of pins on the right hand side of the Symbol");

                static int top_pins = 0;
                ImGui::NextColumn();
                ImGui::Text("Top Pins");
                ImGui::NextColumn();
                ImGui::InputU8Int("##top_pins", &top_pins);
                ImGui::SameLine(); HelpMarker("Enter the number of pins on the top of the Symbol");

                static int bottom_pins = 0;
                ImGui::NextColumn();
                ImGui::Text("Bottom Pins");
                ImGui::NextColumn();
                ImGui::InputU8Int("##Bottom_pins", &bottom_pins);
                ImGui::SameLine(); HelpMarker("Enter the number of pins on the bottom of the Symbol");

                
                if (top_pins == 0 && bottom_pins == 0) {
                    ImGui::NextColumn();
                    ImGui::Text("Width Of Symbol (%s)", current_metric_units);
                    ImGui::NextColumn();
                    ImGui::InputU16Int("##symbol_width", &symbol_width, 10);
                    ImGui::SameLine(); HelpMarker("Enter Symbol width");
                }

                ImGui::NextColumn();
                ImGui::Text("Show Pin Numbers");
                ImGui::NextColumn();
                ImGui::Checkbox("##Show_pin_numbers", &show_pin_number);
                ImGui::NextColumn();
                ImGui::Text("Zoom Factor");
                ImGui::NextColumn();
                static int zoom_factor = 0;
                ImGui::DragInt("##zoom", &zoom_factor, 0.2f, -100, 100, "%d");
                ImGui::NextColumn();


                if (show_generate_window == true) {
                    static bool no_close = true;
                    static bool no_collapse = true;
                    ImGuiWindowFlags generatewindowflag;
                    generatewindowflag |= ImGuiWindowFlags_NoCollapse;
                    ImGui::SetNextWindowPos(ImVec2(0, 580));
                    ImGui::SetNextWindowSize(ImVec2(420, 200));
                    ImGui::Begin("Symbol Generate",NULL);
                    ImGui::Columns(2, "##genc1", false);
                    ImGui::Text("Include Pins in Symbol");
                    ImGui::NextColumn();
                    ImGui::Checkbox("##includesympins", &incpins);
                    ImGui::NextColumn();
                    ImGui::Text("Generate Kicad Symbol");
                    ImGui::NextColumn();
                    generate = ImGui::Button("Generate");
                    if (generate == true) {
                        char lib[128];
                        FILE *fp;
                        strcpy(lib, symbolName);
                        strcat(lib, ".lib");
                        fp = fopen(lib, "a");
                        write_header(fp, lib, "u");
                        draw_lrtb_sym(fp, no_of_units, symbol_width, symbol_thickness, pin_distance, pin_width, left_pins, right_pins, top_pins, bottom_pins, text_width, text_height);

                        end_symbol(fp);
                        generate = false;
                        generated = false;
                        show_console_window = true;
                    }                              
                    if ((show_console_window == true)) {
                        static bool no_close = true;
                        static bool no_collapse = true;
                        ImGuiWindowFlags syboltypeflag;
                        syboltypeflag |= ImGuiWindowFlags_NoCollapse;
                        ImGui::SetNextWindowPos(ImVec2(430, 580));
                        ImGui::SetNextWindowSize(ImVec2(850, 200));
                        ImGui::Begin("Console", NULL);
                        ImGui::Text("Symbol %s Genearted", symbolName);
                        ImGui::NewLine();
                        ImGui::Text("Total Number Of pins in the symbol are %d", left_pins + right_pins + top_pins + bottom_pins);
                        ImGui::NewLine();
                        generated = false;
                        generate = false;
                        ImGui::End();
                    }
                    ImGui::End();

                }              
                if (show_preview_window == true) {
                    static bool no_collapse = true;
                    static bool no_close = true;
                    ImGuiWindowFlags previewflag;
                    previewflag |= ImGuiWindowFlags_NoCollapse;
                    ImGui::SetNextWindowPos(ImVec2(430, 0));
                    ImGui::SetNextWindowSize(ImVec2(850, 560));
                    ImGui::Begin("Symbol Preview", NULL);   

                    if (current_metric_units == "Mils") {
                        preview_draw_lrtb_sym(mils_to_pix(symbol_width)+zoom_factor, mils_to_pix(pin_distance)+zoom_factor, mils_to_pix(pin_width), left_pins, right_pins,top_pins,bottom_pins, symbolName, designator, show_pin_number);
                    }
                    else {
                        preview_draw_lrtb_sym(mm_to_pix(symbol_width), mm_to_pix(pin_distance), mm_to_pix(pin_width), left_pins, right_pins,top_pins,bottom_pins, symbolName, designator, show_pin_number);
                    }
                    ImGui::End();
                }               
                ImGui::End();
            }
        }
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwMakeContextCurrent(window);
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwMakeContextCurrent(window);
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
