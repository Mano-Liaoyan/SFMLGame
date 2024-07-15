#include "EntityManager.h"

#include <memory>

EntityManager::EntityManager()
    : m_totalEntities(0)
{
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
    std::erase_if(vec, [](auto& entity) { return !entity->isActive(); });
}

void EntityManager::update()
{
    for (auto& e : m_entitiesToAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->getTag()].push_back(e);
    }

    //Remove dead entities from the vector of all entities
    removeDeadEntities(m_entities);
    //Remove dead entities from each vector in the entity map
    for (auto& [tag, entityVec] : m_entityMap)
    {
        removeDeadEntities(entityVec);
    }

    m_entitiesToAdd.clear();
}

std::shared_ptr<Entity> EntityManager::addEntity(const ENTITY_TAG tag)
{
    auto e = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
    m_entitiesToAdd.push_back(e);
    return e;
}

const EntityVec& EntityManager::getEntities()
{
    return m_entities;
}

const EntityVec& EntityManager::getEntities(const ENTITY_TAG tag)
{
    return m_entityMap[tag];
}
