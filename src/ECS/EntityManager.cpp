//
// Created by Sam on 3/14/2022.
//

#include "ECS/EntityManager.hpp"

namespace Elys {
    EntityManager::EntityManager() : m_livingEntity{0} {
        for (EntityID entity = 0; entity < MAX_ENTITIES; ++entity) {
            m_availableEntities.push(entity);
        }
    }

    EntityID EntityManager::CreateEntity() {
        if (m_livingEntity >= MAX_ENTITIES)
            throw std::runtime_error("Amount of living entities reached the max.");

        EntityID id = m_availableEntities.front();
        m_availableEntities.pop();
        ++m_livingEntity;

        return id;
    }

    void EntityManager::DestroyEntity(EntityID entity) {
        if (entity >= MAX_ENTITIES)
            throw std::runtime_error("Trying to destroy Entity "
                                     "with an id over max entities capacity.");

        m_signatures[entity].reset();

        m_availableEntities.push(entity);
        --m_livingEntity;
    }

    void EntityManager::SetSignature(EntityID entity, Signature signature) {
        if (entity >= MAX_ENTITIES)
            throw std::runtime_error("Trying "
                                     "to edit signature of an Entity "
                                     "with an id over max entities capacity.");

        m_signatures[entity] = signature;
    }

    Signature EntityManager::GetSignature(EntityID entity) const {
        if (entity >= MAX_ENTITIES)
            throw std::runtime_error("Trying "
                                     "to get signature of an Entity "
                                     "with an id over max entities capacity.");

        return m_signatures[entity];
    }
} // namespace Elys
