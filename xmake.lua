add_requires("assimp", "glfw", "glm", "spdlog", "glad")
add_requires("imgui v1.89.8-docking", { configs = { glfw = true, opengl3 = true } })

add_rules("mode.debug", "mode.release")

set_project("Elys")

set_languages("cxx20")

set_rundir("./")

target("Elys")
  add_files("main.cpp", "src/**.cpp")
  add_includedirs("include", "libs/include")
  add_packages("assimp", "glfw", "glm", "imgui", "spdlog", "glad")