//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_SCENE_HPP
#define ELYS_SCENE_HPP

#include <filesystem>
#include <string>
#include <set>
#include <cmath>
#include <utility>

#include <Core/Base.hpp>

#include "Core/Event.hpp"

// Components
#include <ECS/Components/Node.hpp>
#include <ECS/Components/Light.hpp>
#include <ECS/Components/MeshRenderer.hpp>
#include <ECS/Components/Player.hpp>

// Managers
#include <ECS/ComponentManager.hpp>
#include <ECS/EntityManager.hpp>
#include <ECS/SystemManager.hpp>

namespace Elys {
    class Entity;

    class Scene {
      public:
        Scene();

        Entity CreateEntity(std::string name = "Entity");
        Entity EntityFromNode(Node const &component);
        Entity EntityFromID(EntityID id);

        void SetSelected(int id) { m_selected = id; }
        [[nodiscard]] int GetSelected() const { return m_selected; }

        void SetHovered(int id) { m_hovered = id; }
        [[nodiscard]] int GetHovered() const { return m_hovered; }

        template<typename T, typename ... Args> std::shared_ptr<T> RegisterSystem(Args&& ... args) {
            return m_systemManager->RegisterSystem<T>(std::forward<Args>(args)...);
        }
        template<typename T, typename ... Components> void SetSystemSignature() {
            m_systemManager->SetSignature<T>(m_componentManager->GetSignature<Components...>());

            for(auto entityID : m_entities) {
                m_systemManager->EntitySignatureChanged(entityID,
                                                       m_entityManager->GetSignature(entityID));
            }
        }

        void PushToDestroyQueue(EntityID id);
        void ProcessDestroyQueue();

        std::set<EntityID>::iterator begin() { return m_entities.begin(); }
        std::set<EntityID>::iterator end() { return m_entities.end(); }
        [[nodiscard]] std::set<EntityID>::const_iterator begin() const {return m_entities.begin();}
        [[nodiscard]] std::set<EntityID>::const_iterator end() const { return m_entities.end(); }
      private:
        std::set<EntityID> m_entities;
        std::set<EntityID> m_toDestroy;

        int m_selected{MAX_ENTITIES}, m_hovered{MAX_ENTITIES};

        std::unique_ptr<ComponentManager> m_componentManager;
        std::unique_ptr<EntityManager> m_entityManager;
        std::unique_ptr<SystemManager> m_systemManager;

        friend class Entity;
    };

    class Entity {
      public:
        Entity() = default;
        Entity(Scene* scene, EntityID id) : m_scene(scene), m_id(id) {};
        Entity(const Entity& other) = default;

        ~Entity() = default;

        template<typename T> T& AddComponent(T value) const {
            T& comp = m_scene->m_componentManager->AddComponent(m_id, value);

            // CHANGE SIGNATURE OF ENTITY AND UPDATE SYSTEMS
            Signature newSignature = m_scene->m_entityManager->GetSignature(m_id);
            newSignature.set(m_scene->m_componentManager->GetComponentType<T>(), true);
            m_scene->m_entityManager->SetSignature(m_id, newSignature);
            m_scene->m_systemManager->EntitySignatureChanged(m_id, newSignature);
            return comp;
        }

        template<typename T> [[nodiscard]] T& GetComponent() const {
            return m_scene->m_componentManager->GetComponent<T>(m_id);
        }

        template<typename T> void RemoveComponent() const {
            m_scene->m_componentManager->RemoveComponent<T>(m_id);

            // CHANGE SIGNATURE OF ENTITY AND UPDATE SYSTEMS
            Signature newSignature = m_scene->m_entityManager->GetSignature(m_id);
            newSignature.set(m_scene->m_componentManager->GetComponentType<T>(), false);
            m_scene->m_entityManager->SetSignature(m_id, newSignature);
            m_scene->m_systemManager->EntitySignatureChanged(m_id, newSignature);
        }

        template<typename T> [[nodiscard]] bool HasComponent() const {
            return m_scene->m_componentManager->HasComponent<T>(m_id);
        }

        void SetParent(Entity &p_parent) {
            auto &node = GetComponent<Node>();
            auto &parentNode = p_parent.GetComponent<Node>();

            node.SetParent(&parentNode);
        }

        void AddChildren(Entity &child) {
            auto &node = GetComponent<Node>();
            auto &childNode = child.GetComponent<Node>();

            node.AddChild(&childNode);
        }

        [[nodiscard]] Entity Parent() const {
            auto const &node = GetComponent<Node>();

            if (!node.Parent()) return {};

            return m_scene->EntityFromNode(*node.Parent());
        }

        [[nodiscard]] std::vector<Entity> Children() const {
            auto const &node = GetComponent<Node>();
            std::vector<Entity> children;

            for (auto childNode : node.Children()) {
                children.push_back(m_scene->EntityFromNode(*childNode));
            }

            return children;
        }

        [[nodiscard]] EntityID ID() const { return m_id; }

        [[nodiscard]] bool IsValid() const { return m_scene != nullptr && m_id < MAX_ENTITIES; }

        bool operator==(const Entity &other) const {
            return m_id == other.m_id && m_scene == other.m_scene;
        }

        bool operator<(const Entity &other) const {
            return m_id < other.m_id;
        }

        bool operator>(const Entity &other) const {
            return m_id > other.m_id;
        }

      private:
        EntityID m_id{MAX_ENTITIES};
        Scene*m_scene = nullptr;
    };
}

#endif // ELYS_SCENE_HPP
