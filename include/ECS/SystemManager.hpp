//
// Created by Sam on 3/24/2022.
//

#ifndef ELYS_SYSTEM_MANAGER_HPP
#define ELYS_SYSTEM_MANAGER_HPP

#include <memory>

#include <Core/Base.hpp>
#include <ECS/System.hpp>

namespace Elys {
    class SystemManager {
      public:
        template <typename T, typename ... Args> std::shared_ptr<T> RegisterSystem(Args&& ... args) {
            const char *typeName = typeid(T).name();

            if (m_systems.find(typeName) != m_systems.end())
                throw std::runtime_error("Core::SystemManager::RegisterSystem<T> : "
                                         "System already registered.");

            // Create a pointer to the system and return it so it can be used
            // externally
            auto system = std::make_shared<T>(std::forward<Args>(args)...);
            m_systems.insert({typeName, system});
            return system;
        }

        template <typename T> void SetSignature(Signature signature) {
            const char *typeName = typeid(T).name();

            if (m_systems.find(typeName) == m_systems.end())
                throw std::runtime_error("Core::SystemManager::SetSignature<T> : "
                                         "System is not registered.");

            // Set the signature for this system
            m_signatures.insert({typeName, signature});
        }

        void EntityDestroyed(EntityID entity) {
            // Erase a destroyed entity from all system lists
            // m_entities is a set so no check needed
            for (auto const &pair : m_systems) {
                auto const &system = pair.second;

                system->RemoveEntity(entity);
            }
        }

        void EntitySignatureChanged(EntityID entity, Signature entitySignature) {
            // Notify each system that an entity's signature changed
            for (auto const &pair : m_systems) {
                auto const &type = pair.first;
                auto const &system = pair.second;
                auto const &systemSignature = m_signatures[type];

                // Entity signature matches system signature - insert into set
                if ((entitySignature & systemSignature) == systemSignature) {
                    system->AddEntity(entity);
                }
                // Entity signature does not match system signature - erase from set
                else {
                    system->RemoveEntity(entity);
                }
            }
        }

      private:
        std::unordered_map<const char *, Signature> m_signatures{};
        std::unordered_map<const char *, std::shared_ptr<System>> m_systems{};
    };
} // namespace Elys

#endif // ELYS_SYSTEM_MANAGER_HPP
