//
// Created by Sam on 12/10/2021.
//

#ifndef ELYS_APPLICATION_HPP
#define ELYS_APPLICATION_HPP

#include <functional>
#include <memory>

#include <Core/AssetLoader.hpp>
#include <Core/Base.hpp>
#include <Core/LayerStack.hpp>
#include <Core/Profiling.hpp>
#include <Core/Window.hpp>

#include <Core/Event.hpp>

#include <Layers/EditorLayer.hpp>
#include <Layers/ImGuiLayer.hpp>

int main(int argc, char *argv[]);

namespace Elys {
    class Application {
      public:
        ~Application();

        static Application &Get() { return *s_instance; }

        Window &GetWindow() { return *mWindow; }
        ImGuiLayer &GetImGUILayer() { return *m_imGUILayer; }

        void OnEvent(Event &event);

        void Shutdown() { mRunning = false; }

      private:
        explicit Application(std::string name);

        /// \Brief
        /// Run the Game loop, everything after this function call will happen once the window
        /// closed.
        void Run();

        bool OnWindowClose(WindowCloseEvent &e);
        bool OnWindowResize(WindowResizeEvent &e);

      private:
        std::unique_ptr<Window> mWindow;
        bool mRunning = true;
        bool mMinimized = false;
        float mLastFrameTime = 0.0f;

        std::shared_ptr<EditorLayer> m_editorLayer;
        std::shared_ptr<ImGuiLayer> m_imGUILayer;
        LayerStack m_layerStack;

        // STATIC INSTANCE HANDLE
      private:
        static Application *s_instance;
        friend int ::main(int argc, char *argv[]);
        friend Application *CreateApplication(std::string name);
    };

    /// \Brief
    /// Application is a static instance. There is no need to handle more than one instance. From
    /// this point you can create your application.
    Application *CreateApplication(std::string name = "PlankEngine");
} // namespace Elys

#endif // ELYS_APPLICATION_HPP
