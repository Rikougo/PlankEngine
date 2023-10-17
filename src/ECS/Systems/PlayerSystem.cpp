//
// Created by sakeiru on 5/3/2022.
//

#include "ECS/Systems/PlayerSystem.hpp"

namespace Elys {
    void Elys::PlayerSystem::Update(float deltaTime) {
        using glm::vec3;

        for(auto id : m_entities) {
            auto entity = mCurrentScene->EntityFromID(id);
            auto const &node = entity.GetComponent<Node>();

            mPlayerCamera->SetPosition(node.InheritedPosition() + vec3{0.0f, 4.0f, -6.0f});
            mPlayerCamera->SetTarget(node.InheritedPosition() + vec3{0.0f, node.InheritedScale().y / 2.0f, 0.0f});
        }
    }
}