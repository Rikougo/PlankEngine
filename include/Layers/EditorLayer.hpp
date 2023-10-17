//
// Created by Sam on 3/11/2022.
//

#ifndef ELYS_ECS_LAYER_HPP
#define ELYS_ECS_LAYER_HPP

#include <filesystem>
#include <string>

#include <glm/glm.hpp>

#include <Core/Layer.hpp>
#include <Core/AssetLoader.hpp>

#include <ECS/Scene.hpp>
#include <ECS/Systems/LightSystem.hpp>
#include <ECS/Systems/RenderSystem.hpp>
#include <ECS/Systems/PlayerSystem.hpp>

#include <Core/Event.hpp>

#include <Render/Shader.hpp>
#include <Render/TrackBallCamera.hpp>

#include <GUI/GraphScene.hpp>
#include <GUI/ComponentsEditor.hpp>
#include <GUI/ContentBrowser.hpp>

namespace Elys {
    enum class EditorState {
        EDITING,
        PLAYING
    };

    class EditorLayer : public Layer {
      public:
        struct Viewport {
            glm::vec2 offset{0, 0}, size{0, 0};
        };
      public:
        // --- LIFE CYCLE METHODS --- //
        EditorLayer() = default;
        ~EditorLayer() override = default;
        void OnAttach() override;
        void OnDetach() override;

        void OnUpdate(float deltaTime) override;
        void OnImGuiRender() override;
        void OnEvent(Event& e) override;
        
        TrackBallCamera &GetEditorCamera() {
            return *m_editorCamera;
        }
        
        Viewport GetViewport() {
            return m_viewport;
        }
      private:
        void UpdateCursorState(bool p_hasMouseInput);
        bool OnKeyPressed(KeyPressedEvent &event);

        void CreateScene();
        void ChangeScene(std::shared_ptr<Scene> newScene);

        void InitSystems();

        void TogglePlayMode();
      private:
        EditorState m_currentState{EditorState::EDITING};
        Viewport m_viewport;
        bool m_viewportHovered = false;
        std::shared_ptr<Scene> m_currentScene;

        // --- SYSTEMS --- //
        std::shared_ptr<LightSystem> m_lightSystem;
        std::shared_ptr<RenderSystem> m_renderSystem;
        std::shared_ptr<PlayerSystem> m_playerSystem;

        // --- DISPLAY STUFF --- //
        std::shared_ptr<Framebuffer> m_framebuffer;
        std::shared_ptr<TrackBallCamera> m_editorCamera;
        std::shared_ptr<TrackBallCamera> m_playerCamera;
        std::shared_ptr<Shader> m_shader;

        GUI::ContentBrowser m_contentBrowser{};
        GUI::GraphScene m_graphScene{};
        GUI::ComponentsEditor m_componentsEditor{};
    };
}

#endif // ELYS_ECS_LAYER_HPP
