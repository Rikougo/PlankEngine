//
// Created by Sam on 3/14/2022.
//

#include "ECS/Scene.hpp"

namespace Elys {
    Scene::Scene() {
        m_entityManager = std::make_unique<EntityManager>();
        m_componentManager = std::make_unique<ComponentManager>();
        m_systemManager = std::make_unique<SystemManager>();

        m_componentManager->RegisterComponent<Node>();
        m_componentManager->RegisterComponent<MeshRenderer>();
        m_componentManager->RegisterComponent<Light>();
        m_componentManager->RegisterComponent<Player>();
    }

    Entity Scene::CreateEntity(std::string name) {
        auto newID = m_entityManager->CreateEntity();
        auto entity = Entity(this, newID);
        entity.AddComponent(Node{}).name = std::move(name);

        m_entities.insert(newID);

        ELYS_CORE_INFO("Created entity {0}", newID);
        return entity;
    }
    Entity Scene::EntityFromNode(const Node &component) {
        auto id = m_componentManager->GetEntity<Node>(component);
        return {this, id};
    }
    Entity Scene::EntityFromID(EntityID id) { 
        return {this, id}; 
    }

    void Scene::PushToDestroyQueue(EntityID id) { 
        m_toDestroy.insert(id); 
    
    }
    void Scene::ProcessDestroyQueue() {
        for (auto id : m_toDestroy) {
            if (id == m_selected)
                m_selected = MAX_ENTITIES;
            if (id == m_hovered)
                m_hovered = MAX_ENTITIES;

            m_entities.erase(id);
            m_componentManager->GetComponent<Node>(id).OnDelete();

            m_systemManager->EntityDestroyed(id);
            m_componentManager->EntityDestroyed(id);
            m_entityManager->DestroyEntity(id);
        }

        m_toDestroy.clear();
    }
}
