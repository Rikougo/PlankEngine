//
// Created by Sam on 3/11/2022.
//

#include "Layers/EditorLayer.hpp"

#include <filesystem>

#include <imgui.h>

#include <Application.hpp>

#include <utility>

namespace Elys {
    void EditorLayer::OnAttach() {
        m_debugName = "Editor";

        m_currentScene = std::make_shared<Scene>();
        m_shader = std::make_shared<Shader>("./shaders/model_vertex.glsl",
                                            "./shaders/model_fragment.glsl");
        m_editorCamera = std::make_shared<TrackBallCamera>();
        m_editorCamera->SetViewSize(1280, 720);

        m_playerCamera = std::make_shared<TrackBallCamera>();
        m_playerCamera->SetViewSize(1280, 720);
        m_playerCamera->Rotate(0.0f, -0.5f);

        m_framebuffer = std::make_shared<Framebuffer>(
            FramebufferData{.Width = 1280,
                            .Height = 720,
                            .Attachments = {{GL_RGB}, {GL_RED_INTEGER}},
                            .DepthAttachmentFormat = GL_DEPTH24_STENCIL8});

        InitSystems();

        for(size_t i = 0; i < 5; i++) {
            auto line = m_currentScene->CreateEntity("Line" + std::to_string(i));
            for(size_t j = 0; j < 5; j++) {
                auto entity = m_currentScene->CreateEntity("Showcase" + std::to_string(j));
                entity.GetComponent<Node>().SetPosition(i * 2.10f, j * 2.10f, 0.0f);
                entity.GetComponent<Node>().SetRotation(3.14f, 0.0f, 0.0f);
                entity.AddComponent<MeshRenderer>({
                    .mesh = AssetLoader::MeshFromPath("Sphere"),
                    .material = Material::FromTexture(AssetLoader::TextureFromPath("textures/lava/lava_COLOR.jpg"))
                                     .SetNormalMap(AssetLoader::TextureFromPath(
                                         "textures/lava/lava_NORM.png"))
                                     .SetMetallic(0.25f * i)
                        .SetRoughness(0.25f * j)
                });

                line.AddChildren(entity);
            }
        }
    }

    void EditorLayer::OnDetach() {
        m_lightSystem.reset();
        m_renderSystem.reset();
        m_playerCamera.reset();

        m_framebuffer.reset();
        m_editorCamera.reset();
        m_playerCamera.reset();
        m_shader.reset();
        m_currentScene.reset();
    }

    void EditorLayer::OnUpdate(float p_deltaTime) {
        // m_currentScene->ProcessDestroyQueue();

        // Framebuffer keep track of viewport size since color attachment is used by viewport for
        // the display. Camera should always be the same size of the Framebuffer.
        if ((m_framebuffer->GetData().Width != m_viewport.size.x ||
             m_framebuffer->GetData().Height != m_viewport.size.y) &&
            (m_viewport.size.x != 0 && m_viewport.size.y != 0)) {
            m_framebuffer->Resize(m_viewport.size.x, m_viewport.size.y);
            m_editorCamera->SetViewSize(m_viewport.size.x, m_viewport.size.y);
            m_playerCamera->SetViewSize(m_viewport.size.x, m_viewport.size.y);
        }

        // ORDER HERE IS IMPORTANT
        if (m_currentState == EditorState::PLAYING) {
            m_playerSystem->Update(p_deltaTime);
        }
        m_lightSystem->Update(p_deltaTime);
        m_renderSystem->Update(p_deltaTime);

        if ((m_viewportHovered || m_editorCamera->IsCapturing()) &&
            m_currentState == EditorState::EDITING) {
            auto [mx, my] = ImGui::GetMousePos();
            mx -= m_viewport.offset.x;
            my -= m_viewport.offset.y;

            auto entityID = m_framebuffer->GetPixel((int)mx, (int)(m_viewport.size.y - my), 1);
            m_currentScene->SetHovered(entityID);

            bool l_hasMouseInput = false;
            auto l_pos = Input::GetMousePosition();
            if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle)) {
                m_editorCamera->MouseInput(l_pos.x, l_pos.y, Mouse::ButtonMiddle);
                l_hasMouseInput = true;
            } else if (Input::IsMouseButtonPressed(Mouse::ButtonRight)) {
                m_editorCamera->MouseInput(l_pos.x, l_pos.y, Mouse::ButtonRight);
                l_hasMouseInput = true;
            } else{
                m_editorCamera->ReleaseInput();
            }

            UpdateCursorState(l_hasMouseInput);

            if (!Input::IsKeyPressed(Key::LeftControl)) {
                glm::vec3 cameraDirection{(Input::IsKeyPressed(Key::A) ? -1.0f : (Input::IsKeyPressed(Key::D) ? 1.0f : 0.0f)),
                                          (Input::IsKeyPressed(Key::Q) ? -1.0f : (Input::IsKeyPressed(Key::E) ? 1.0f : 0.0f)),
                                          (Input::IsKeyPressed(Key::S) ? -1.0f : (Input::IsKeyPressed(Key::W) ? 1.0f : 0.0f))};

                m_editorCamera->Pan(cameraDirection * p_deltaTime);
            }
        }
    }

    void EditorLayer::OnImGuiRender() {
        static bool dockSpaceOpen = true;
        static ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode) {
            window_flags |= ImGuiWindowFlags_NoBackground;
        }

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("MainDock", &dockSpaceOpen, window_flags)) {
            ImGui::PopStyleVar(3);
            // DockSpace
            ImGuiIO &io = ImGui::GetIO();
            ImGuiStyle &style = ImGui::GetStyle();
            float minWinSizeX = style.WindowMinSize.x;
            style.WindowMinSize.x = 370.0f;
            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
                ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockSpaceFlags);
            }

            style.WindowMinSize.x = minWinSizeX;

            if (ImGui::BeginMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Exit", "Ctrl + Q"))
                        Application::Get().Shutdown();
                    if (ImGui::MenuItem("Save scene", "Ctrl + S"))
                        AssetLoader::SerializeScene(m_currentScene, "scene.escene");
                    if (ImGui::MenuItem("Reload shader", "Ctrl + R"))
                        m_shader->Reload("./shaders/model_vertex.glsl",
                                         "./shaders/model_fragment.glsl");
                    if (ImGui::MenuItem("Toggle playmode", "Ctrl + P"))
                        TogglePlayMode();

                    ImGui::EndMenu();
                }
            }
            ImGui::EndMenuBar();

            // ImGui::ShowDemoWindow();

            if (ImGui::Begin("Settings")) {
                if (ImGui::CollapsingHeader("Editor camera", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::BeginTable("##EditorCamera", 2, ImGuiTableFlags_NoPadInnerX);

                    ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
                    ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch); // Default to auto

                    ImGui::TableNextColumn();
                    ImGui::Text("Position");
                    ImGui::TableNextColumn();
                    auto position = m_editorCamera->GetPosition();
                    GUI::SliderVec3("##Position", position, 0.1f, false);

                    ImGui::TableNextColumn();
                    ImGui::Text("Speed");
                    ImGui::TableNextColumn();
                    ImGui::DragFloat("##Speed", &m_editorCamera->speed, 0.1f);

                    ImGui::TableNextColumn();
                    ImGui::Text("Sensitivity");
                    ImGui::TableNextColumn();
                    ImGui::DragFloat("##Sensitivity", &m_editorCamera->sensitivity, 0.1f);

                    ImGui::EndTable();
                }

                if (ImGui::CollapsingHeader("Stats", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static bool VSYNC = true;
                    bool tempVSYNC = VSYNC;

                    ImGui::Checkbox("Vsync", &tempVSYNC);
                    if (tempVSYNC != VSYNC) {
                        VSYNC = tempVSYNC;
                        Application::Get().GetWindow().EnableVSync(VSYNC);
                    }

                    ImGui::Text("Framerate : %0.1f", Profile::Framerate);
                    ImGui::Text("Total time : %0.3f", Profile::DeltaTime);
                    ImGui::Text("DrawnMesh : %d", Profile::DrawnMesh);

                    ImGui::Text("%0.3fx%0.3f", Input::GetMousePosition().x,
                                Input::GetMousePosition().y);
                    ImGui::Text("%0.3fx%0.3f", m_viewport.offset.x, m_viewport.offset.y);
                }
            }
            ImGui::End();

            m_graphScene.OnImGUIRender(m_currentScene, nullptr);
            m_currentScene->ProcessDestroyQueue();
            m_componentsEditor.OnImGUIRender(
                m_currentScene->EntityFromID(m_currentScene->GetSelected()), nullptr);
            m_contentBrowser.OnImGUIRender(nullptr);

            // Viewport: where the 3D scene is drawn
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
            if (ImGui::Begin("Viewport")) {
                /*if (ImGui::BeginMenuBar()) {
                    ImGui::Button("Play");
                } ImGui::EndMenuBar();*/

                auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
                auto viewportOffset = ImGui::GetWindowPos();
                auto viewportSize = ImGui::GetContentRegionAvail();

                // it is viewport position in screen space, useful to have relative mouse position
                // in viewport
                m_viewport.offset = {viewportMinRegion.x + viewportOffset.x,
                                     viewportMinRegion.y + viewportOffset.y};
                m_viewport.size = {viewportSize.x, viewportSize.y};

                // Bind Framebuffer color texture to an ImGui image
                unsigned int textureID = m_renderSystem->GetFramebuffer()->GetColorTextureID(0);
                ImGui::Image((ImTextureID)((size_t)textureID), viewportSize, ImVec2{0, 1},
                             ImVec2{1, 0});

                m_viewportHovered = ImGui::IsWindowHovered();

                // If viewport is not hovered, no need to let event goes further than GUI
                Application::Get().GetImGUILayer().SetBlocking(!m_viewportHovered);

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload *payload =
                            ImGui::AcceptDragDropPayload(AssetLoader::DragDropType::FILE_SCENE)) {
                        const auto *path = (const wchar_t *)payload->Data;
                        ChangeScene(AssetLoader::SceneFromPath(path));
                    }

                    ImGui::EndDragDropTarget();
                }
            } ImGui::End();

            ImGui::PopStyleVar();
        } else {
            ImGui::PopStyleVar(3);
        }
        ImGui::End();
    }

    void EditorLayer::OnEvent(Event &event) {
        EventDispatcher dispatcher(event);

        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));

        if (m_currentState == EditorState::EDITING) {
            dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(m_renderSystem->OnKeyPressed));
            dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent &event) {
                m_editorCamera->Zoom(event.GetYOffset() *
                                     (Input::IsKeyPressed(Key::LeftShift) ? 10.0f : 0.1f));

                return false;
            });

            dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent &event) {
                if (event.GetMouseButton() != Mouse::ButtonMiddle)
                    return false;

                auto [mx, my] = ImGui::GetMousePos();
                mx -= m_viewport.offset.x;
                my -= m_viewport.offset.y;

                auto entityID = m_framebuffer->GetPixel((int)mx, (int)(m_viewport.size.y - my), 1);

                m_currentScene->SetSelected(entityID);

                ELYS_CORE_INFO("Clicked entity {0} at [x: {1}, y: {2}]", entityID, mx, my);

                return false;
            });
        }
    }

    void EditorLayer::CreateScene() { m_currentScene = std::make_shared<Scene>(); }

    void EditorLayer::ChangeScene(std::shared_ptr<Scene> p_newScene) {
        m_currentScene = std::move(p_newScene);

        InitSystems();
    }
    
    void EditorLayer::UpdateCursorState(bool p_hasMouseInput) {
        if (p_hasMouseInput && Input::GetCursorMode() == Cursor::Normal) {
            Input::SetCursorMode(Cursor::Disabled);
        }
        
        if (!p_hasMouseInput && Input::GetCursorMode() == Cursor::Disabled) {
            Input::SetCursorMode(Cursor::Normal);
        }
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent &event) {
        auto kc = event.GetKeyCode();

        // --- APPLICATION INTERACTION --- //
        if (kc == Key::P && Input::IsKeyPressed(Key::LeftControl)) {
            TogglePlayMode();
        }

        if (kc == Key::A && Input::IsKeyPressed(Key::LeftControl)) {
            Application::Get().Shutdown();
        }

        if (kc == Key::S && Input::IsKeyPressed(Key::LeftControl)) {
            AssetLoader::SerializeScene(m_currentScene, "scene.escene");
        }

        if (kc == Key::R && Input::IsKeyPressed(Key::LeftControl)) {
            m_shader->Reload("./shaders/model_vertex.glsl", "./shaders/model_fragment.glsl");
        }

        if (kc == Key::D && Input::IsKeyPressed(Key::LeftControl)) {
            m_renderSystem->ToggleDebugMode();
        }

        return false;
    }

    void EditorLayer::InitSystems() {
        m_lightSystem = m_currentScene->RegisterSystem<LightSystem>(m_currentScene, m_editorCamera,
                                                                    m_shader, m_framebuffer);
        m_currentScene->SetSystemSignature<LightSystem, Light, Node>();

        m_renderSystem = m_currentScene->RegisterSystem<RenderSystem>(
            m_currentScene, m_editorCamera, m_shader, m_framebuffer);
        m_currentScene->SetSystemSignature<RenderSystem, MeshRenderer, Node>();

        m_playerSystem =
            m_currentScene->RegisterSystem<PlayerSystem>(m_currentScene, m_playerCamera);
        m_currentScene->SetSystemSignature<PlayerSystem, Player, Node>();
    }

    void EditorLayer::TogglePlayMode() {
        m_currentState =
            (m_currentState == EditorState::EDITING) ? EditorState::PLAYING : EditorState::EDITING;

        switch (m_currentState) {
        case EditorState::EDITING:
            m_renderSystem->SetCamera(m_editorCamera);
            break;
        case EditorState::PLAYING:
            m_renderSystem->SetCamera(m_playerCamera);
            break;
        }
    }
} // namespace Elys