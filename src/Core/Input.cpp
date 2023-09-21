//
// Created by Sam on 3/30/2022.
//

#include <Core/Input.hpp>

#include <Application.hpp>

namespace Elys {
    bool Input::IsKeyPressed(KeyCode code) {
        GLFWwindow *window = Application::Get().GetWindow().GetGLFWWindow();
        int state = glfwGetKey(window, static_cast<int32_t>(code));
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool Input::IsMouseButtonPressed(MouseCode button) {
        GLFWwindow *window = Application::Get().GetWindow().GetGLFWWindow();
        return glfwGetMouseButton(window, static_cast<int32_t>(button)) == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition() {
        GLFWwindow *window = Application::Get().GetWindow().GetGLFWWindow();
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        return {static_cast<float>(xPos), static_cast<float>(yPos)};
    }

    CursorCode Input::GetCursorMode() { 
        GLFWwindow *window = Application::Get().GetWindow().GetGLFWWindow();
        
        return static_cast<CursorCode>(glfwGetInputMode(window, GLFW_CURSOR));
    }
    void Input::SetCursorMode(CursorCode l_code) {
        GLFWwindow *window = Application::Get().GetWindow().GetGLFWWindow();

        glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(l_code));
    }

} // namespace Elys
