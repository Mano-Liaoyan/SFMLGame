#pragma once

#include <vector>

#include "Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<ENTITY_TAG, EntityVec>;

class EntityManager
{
    EntityVec m_entities;
    EntityMap m_entityMap;
    EntityVec m_entitiesToAdd;
    size_t m_totalEntities{0};

    static void removeDeadEntities(EntityVec& vec);

public:
    EntityManager();

    void update();

    const EntityVec& getEntities();
    const EntityVec& getEntities(const ENTITY_TAG tag);

    std::shared_ptr<Entity> addEntity(const ENTITY_TAG tag);
};
