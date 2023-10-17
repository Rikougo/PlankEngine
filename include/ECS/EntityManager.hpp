//
// Created by Sam on 3/14/2022.
//

#ifndef ELYS_MANAGERS_ENTITY_MANAGER_HPP
#define ELYS_MANAGERS_ENTITY_MANAGER_HPP

#include <array>
#include <queue>
#include <stdexcept>

#include <Core/Base.hpp>

namespace Elys {
    class EntityManager {
      private:
        std::queue<EntityID> m_availableEntities{};
        std::array<Signature, MAX_ENTITIES> m_signatures{};
        uint32_t m_livingEntity;

      public:
        EntityManager();

        EntityID CreateEntity();

        void DestroyEntity(EntityID entity);

        void SetSignature(EntityID entity, Signature signature);
        [[nodiscard]] Signature GetSignature(EntityID entity) const;
    };
} // namespace Elys

#endif // ELYS_MANAGERS_ENTITY_MANAGER_HPP
