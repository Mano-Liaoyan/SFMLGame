#include "Entity.h"

Entity::Entity(const size_t id, const ENTITY_TAG tag): m_id(id), m_tag(tag)
{
}

bool Entity::isActive() const
{
    return m_isActive;
}

ENTITY_TAG Entity::getTag() const
{
    return m_tag;
}

size_t Entity::getId() const
{
    return m_id;
}

void Entity::destroy()
{
    m_isActive = false;
}
