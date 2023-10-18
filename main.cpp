#define STB_IMAGE_IMPLEMENTATION
#define IMGUI_DEFINE_MATH_OPERATORS // quick fix for IMGUIZMO
#include <utility>

#include <Application.hpp>

#include <Core/Logger.hpp>
#include <Core/Profiling.hpp>

int main(int argc, char *argv[]) {
    Elys::Application* app = Elys::CreateApplication();

    app->Run();
    return 0;
}