#pragma once

#include <memory>
#include "Components.h"

enum ENTITY_TAG { Default, Player, Enemy, SmallEnemy, Bullet };

class Entity
{
    friend class EntityManager;

    bool m_isActive = true;
    size_t m_id = 0;
    ENTITY_TAG m_tag = Default;

    // Private constructor
    Entity(const size_t id, const ENTITY_TAG tag);

public:
    // Components pointers
    std::shared_ptr<CInput> c_input;
    std::shared_ptr<CScore> c_score;
    std::shared_ptr<CShape> c_shape;
    std::shared_ptr<CLifespan> c_lifespan;
    std::shared_ptr<CCollision> c_collision;
    std::shared_ptr<CTransform> c_transform;

    bool isActive() const;
    ENTITY_TAG getTag() const;
    size_t getId() const;
    void destroy();
};
