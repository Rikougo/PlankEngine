//
// Created by sakeiru on 5/5/2022.
//

#include <GUI/ComponentsEditor.hpp>

namespace Elys::GUI {
    void ComponentsEditor::OnImGUIRender(Entity entity, bool *open)  {
        if (ImGui::Begin("Components Editor", open, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!entity.IsValid()) {
                ImGui::End();
                return;
            }

            auto &node = entity.GetComponent<Node>();

            ImGui::PushID(node.name.c_str());
            {

                ImGui::BeginTable("Information", 3);

                ImGui::TableSetupColumn("enabled", ImGuiTableColumnFlags_WidthFixed, 15.0f);
                ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("add", ImGuiTableColumnFlags_WidthFixed, 15.0f);

                ImGui::TableNextColumn();
                bool enabled = node.LocalEnabled();
                ImGui::Checkbox("##enabled", &enabled);
                if (node.LocalEnabled() ^ enabled)
                    node.SetEnabled(enabled);

                ImGui::TableNextColumn();
                ImGui::InputText("##Name", &node.name);

                // ADD COMPONENT STUFF
                ImGui::TableNextColumn();
                if (ImGui::Button("+")) {
                    ImGui::OpenPopup("AddComponentPopup");
                }

                if (ImGui::BeginPopupContextItem("AddComponentPopup"))
                {
                    if (ImGui::Selectable("MeshRenderer", false, entity.HasComponent<MeshRenderer>() ? ImGuiSelectableFlags_Disabled : 0)) {
                        entity.AddComponent<MeshRenderer>({
                            .mesh = AssetLoader::MeshesMap()["Cube"]
                        });
                    }
                    if (ImGui::Selectable("Light", false, entity.HasComponent<Light>() ? ImGuiSelectableFlags_Disabled : 0)) {
                        entity.AddComponent<Light>({});
                    }
                    if (ImGui::Selectable("Player", false, entity.HasComponent<Player>() ? ImGuiSelectableFlags_Disabled : 0)) {
                        entity.AddComponent<Player>({});
                    }
                    ImGui::EndPopup();
                }

                ImGui::EndTable();

                ImGui::SameLine();

                // --- NODE COMPONENT ---
                // Shared by every entity
                if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                    NodeEditor("##Transform", node);
                }

                // --- MESH RENDERER ---
                if (entity.HasComponent<MeshRenderer>()) {
                    if (ImGui::CollapsingHeader("Mesh Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                        bool removed = false;
                        if (ImGui::BeginPopupContextItem()) {
                            if (ImGui::Selectable("Remove")) {
                                entity.RemoveComponent<MeshRenderer>();
                                removed = true;
                            }
                            ImGui::EndPopup();
                        }

                        if (!removed) MeshRenderEditor("##MeshRenderer", entity.GetComponent<MeshRenderer>());
                    }
                }

                if (entity.HasComponent<Light>()) {
                    if (ImGui::CollapsingHeader("Light Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
                        bool removed = false;
                        if (ImGui::BeginPopupContextItem()) {
                            if (ImGui::Selectable("Remove")) {
                                entity.RemoveComponent<Light>();
                                removed = true;
                            }
                            ImGui::EndPopup();
                        }

                        if (!removed) LightEditor("##Light", entity.GetComponent<Light>());
                    }
                }

                if (entity.HasComponent<Player>()) {
                    if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
                        bool removed = false;
                        if (ImGui::BeginPopupContextItem()) {
                            if (ImGui::Selectable("Remove")) {
                                entity.RemoveComponent<Player>();
                                removed = true;
                            }
                            ImGui::EndPopup();
                        }

                        if (!removed) PlayerEditor("##Player", entity.GetComponent<Player>());
                    }
                }
            }
            ImGui::PopID();
        }
        ImGui::End();
    }
    void ComponentsEditor::NodeEditor(const std::string &label, Node &node) {
        auto pos = node.LocalPosition();
        auto rot = eulerAngles(node.LocalRotation()) * (180.0f / static_cast<float>(M_PI));
        auto scale = node.LocalScale();

        static bool uniformScale = true;

        auto tableFlags = ImGuiTableFlags_NoPadInnerX;
        ImGui::BeginTable(label.c_str(), 3, tableFlags);

        ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                100.0f); // Default to 100.0f
        ImGui::TableSetupColumn("widget",
                                ImGuiTableColumnFlags_WidthStretch); // Default to auto
        ImGui::TableSetupColumn("alt", ImGuiTableColumnFlags_WidthFixed, 50.0f); // Default to auto

        ImGui::TableNextColumn();
        ImGui::Text("Position");
        ImGui::TableNextColumn();
        SliderVec3("##Position", pos);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Rotation");
        ImGui::TableNextColumn();
        SliderVec3("##Rotation", rot);

        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::Text("Scale");
        ImGui::TableNextColumn();
        SliderVec3("##Scale", scale);
        ImGui::TableNextColumn();
        ImGui::Checkbox("##UniformScale", &uniformScale);

        ImGui::EndTable();

        if (uniformScale) {
            for (uint8_t i = 0; i < 3; i++) {
                if (scale[i] != node.LocalScale()[i]) {
                    node.SetScale(scale[i]);
                    break;
                }
            }
        } else
            node.SetScale(scale);

        node.SetPosition(pos);
        node.SetRotation(glm::quat(rot * (static_cast<float>(M_PI) / 180.0f)));
    }
    void ComponentsEditor::MeshRenderEditor(const std::string &label, MeshRenderer &meshRenderer)  {
        auto tableFlags = ImGuiTableFlags_NoPadInnerX;
        ImGui::BeginTable(label.c_str(), 3, tableFlags);

        ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                100.0f); // Default to 100.0f
        ImGui::TableSetupColumn("widget",
                                ImGuiTableColumnFlags_WidthStretch); // Default to auto
        ImGui::TableSetupColumn("widget_modifier", ImGuiTableColumnFlags_WidthFixed,
                                20.0f); // Default to auto

        ImGui::TableNextColumn();
        ImGui::Text("Mesh");
        ImGui::TableNextColumn();
        if (ImGui::BeginCombo("##MeshSelector", meshRenderer.mesh.Path().c_str(), 0))
        {
            auto meshes = AssetLoader::MeshesMap();
            for (const auto& pair : meshes)
            {
                const bool is_selected = (pair.first == meshRenderer.mesh.Path());
                if (ImGui::Selectable(pair.first.c_str(), is_selected)) {
                    meshRenderer.mesh = pair.second;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Color");
        ImGui::TableNextColumn();
        auto &a = meshRenderer.material.albedo;
        if (ImGui::ColorButton("MaterialAlbedo", ImVec4(a.r, a.g, a.b, a.a), 0,
                               ImVec2(ImGui::GetColumnWidth(), ImGui::GetFrameHeight())))
            ImGui::OpenPopup("MaterialAlbedo");
        if (ImGui::BeginPopup("MaterialAlbedo")) {
            ImGui::ColorPicker4("##MaterialAlbedoPicker", glm::value_ptr(a),
                                ImGuiColorEditFlags_None, nullptr);
            ImGui::EndPopup();
        }
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("UV Tiling");
        ImGui::TableNextColumn();
        GUI::SliderVec2("##UVTiling", meshRenderer.material.tiling);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Texture");
        ImGui::TableNextColumn();
        GUI::TextureInput(meshRenderer.material.texture, "Texture");
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Normal map");
        ImGui::TableNextColumn();
        GUI::TextureInput(meshRenderer.material.normalMap, "Normalmap");
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Height map");
        ImGui::TableNextColumn();
        GUI::TextureInput(meshRenderer.material.heightMap, "Heightmap");
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Shaded");
        ImGui::TableNextColumn();
        ImGui::Checkbox("##Shaded", &meshRenderer.material.shaded);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Metallic");
        ImGui::TableNextColumn();
        ImGui::DragFloat("##Metallic", &meshRenderer.material.metallic, 0.05f, 0.0f, 1.0f);
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::Text("Roughness");
        ImGui::TableNextColumn();
        ImGui::DragFloat("##Roughness", &meshRenderer.material.roughness, 0.05f, 0.0f, 1.0f);
        ImGui::TableNextRow();

        ImGui::EndTable();
    }
    void ComponentsEditor::LightEditor(const std::string &label, Light &light)  {
        auto tableFlags = ImGuiTableFlags_NoPadInnerX;
        ImGui::BeginTable(label.c_str(), 2, tableFlags);

        ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed,
                                100.0f); // Default to 100.0f
        ImGui::TableSetupColumn("widget",
                                ImGuiTableColumnFlags_WidthStretch); // Default to auto

        ImGui::TableNextColumn();
        ImGui::Text("Color");
        ImGui::TableNextColumn();
        auto &lc = light.color;
        if (ImGui::ColorButton("LightColor", ImVec4(lc.r, lc.g, lc.b, 1.0), 0,
                               ImVec2(ImGui::GetColumnWidth(), ImGui::GetFrameHeight())))
            ImGui::OpenPopup("LightColor");
        if (ImGui::BeginPopup("LightColor")) {
            ImGui::ColorPicker3("##LightColorPicker", glm::value_ptr(lc),
                                ImGuiColorEditFlags_None);
            ImGui::EndPopup();
        }

        ImGui::TableNextColumn();
        ImGui::Text("Intensity");
        ImGui::TableNextColumn();
        ImGui::DragFloat("##intensity", &light.intensity, 1.0f, 0.0f, 500.0f, "%0.1f");

        ImGui::EndTable();
    }
    void ComponentsEditor::PlayerEditor(const std::string &label, Player &player)  {
        auto tableFlags = ImGuiTableFlags_NoPadInnerX;
        ImGui::BeginTable(label.c_str(), 2, tableFlags);

        ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed, 100.0f); // Default to 100.0f
        ImGui::TableSetupColumn("widget", ImGuiTableColumnFlags_WidthStretch); // Default to auto

        ImGui::TableNextColumn();
        ImGui::Text("Speed");
        ImGui::TableNextColumn();
        ImGui::DragFloat("##speed", &player.speed, 0.1f, 0.0f, 20.0f, "%0.1f");

        ImGui::EndTable();
    }
}