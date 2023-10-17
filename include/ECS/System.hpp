//
// Created by Sam on 3/24/2022.
//

#ifndef ELYS_SYSTEM_HPP
#define ELYS_SYSTEM_HPP

#include <set>

#include <Core/Base.hpp>

namespace Elys{
    class System {
      public:
        virtual void Update(float deltaTime) = 0;

        virtual void AddEntity(EntityID newEntity);
        virtual void RemoveEntity(EntityID removedEntity);
      protected:
        std::set<EntityID> m_entities;
    };
}

#endif // ELYS_SYSTEM_HPP
