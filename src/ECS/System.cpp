//
// Created by sakeiru on 5/8/2022.
//

#include <ECS/System.hpp>

namespace Elys {
    void System::AddEntity(EntityID newEntity) { m_entities.insert(newEntity); }
    void System::RemoveEntity(EntityID removedEntity) { m_entities.erase(removedEntity); }
}