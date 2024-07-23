#include <iostream>
#include <fstream>

#include "Game.h"

#include <memory>
#include <cmath>
#include <memory>
#include <memory>

void Game::pause(const bool paused)
{
    m_paused = paused;
}

void Game::initialization(const std::string& path)
{
    int fr, fg, fb;
    std::string mode;
    std::string font_path;
    int frame_limit = 60, fullscreen = 0;
    int window_w = 1920, window_h = 1080, font_size = 30;

    std::ifstream fin(path);

    while (fin >> mode)
    {
        if (mode == "Window")
        {
            fin >> window_w >> window_h >> frame_limit >> fullscreen;
        }
        else if (mode == "Font")
        {
            fin >> font_path >> font_size >> fr >> fg >> fb;
            if (!m_font.loadFromFile(font_path))
            {
                std::cerr << "Could not load the font!" << '\n';
                return;
            }
            m_text.setFillColor(sf::Color(fr, fg, fb));
            m_text.setCharacterSize(font_size);
            m_text.setFont(m_font);
        }
        else if (mode == "Player")
        {
            fin >> m_playerConfig.SR >> m_playerConfig.CR >> m_playerConfig.S >> m_playerConfig.FR
                >> m_playerConfig.FG >> m_playerConfig.FB >> m_playerConfig.OR >> m_playerConfig.OG
                >> m_playerConfig.OB >> m_playerConfig.OT >> m_playerConfig.V;
        }
        else if (mode == "Enemy")
        {
            fin >> m_enemyConfig.SR >> m_enemyConfig.CR >> m_enemyConfig.SMIN >> m_enemyConfig.SMAX
                >> m_enemyConfig.OR >> m_enemyConfig.OG >> m_enemyConfig.OB >> m_enemyConfig.OT
                >> m_enemyConfig.VMIN >> m_enemyConfig.VMAX >> m_enemyConfig.L >> m_enemyConfig.SI;
        }
        else if (mode == "Bullet")
        {
            fin >> m_bulletConfig.SR >> m_bulletConfig.CR >> m_bulletConfig.S >> m_bulletConfig.FR
                >> m_bulletConfig.FG >> m_bulletConfig.FB >> m_bulletConfig.OR >> m_bulletConfig.OG
                >> m_bulletConfig.OB >> m_bulletConfig.OT >> m_bulletConfig.V >> m_bulletConfig.L;
        }
    }
    if (fullscreen)
    {
        m_window.create(sf::VideoMode::getDesktopMode(), "Assignment 2", sf::Style::Fullscreen);
        m_window.setVerticalSyncEnabled(true);
    }
    else
    {
        m_window.create(sf::VideoMode(window_w, window_h), "Assignment 2");
        m_window.setFramerateLimit(frame_limit);
    }
    m_text.setPosition(0, 0);
    spawnPlayer();
}

void Game::renderSystem()
{
    m_window.clear();
    for (auto& e : m_entityManager.getEntities())
    {
        e->c_shape->shape.setPosition(e->c_transform->position.x, e->c_transform->position.y);
        e->c_transform->angle += 5.0f;
        e->c_shape->shape.setRotation(e->c_transform->angle);
        m_window.draw(e->c_shape->shape);
    }
    m_text.setString(std::to_string(m_score));
    m_window.draw(m_text);
    m_window.display();
}

void Game::lifespanSystem()
{
    for (auto& e : m_entityManager.getEntities())
    {
        if (!e->c_lifespan) { continue; }
        if (e->c_lifespan->remaining > 0)
        {
            --e->c_lifespan->remaining;
        }
        if (e->isActive() && e->c_lifespan->remaining > 0)
        {
            const float alpha_multiplier{
                static_cast<float>(e->c_lifespan->remaining) / static_cast<float>(e->c_lifespan->total)
            };

            const auto fill_color{e->c_shape->shape.getFillColor()};
            sf::Color new_fill_color{
                fill_color.r, fill_color.g, fill_color.b, static_cast<sf::Uint8>(255 * alpha_multiplier)
            };
            e->c_shape->shape.setFillColor(new_fill_color);

            const auto outline_color{e->c_shape->shape.getOutlineColor()};
            sf::Color new_outline_color{
                outline_color.r, outline_color.g, outline_color.b, static_cast<sf::Uint8>(255 * alpha_multiplier)
            };
            e->c_shape->shape.setOutlineColor(new_outline_color);
        }
        else if (e->c_lifespan->remaining <= 0)
        {
            e->destroy();
        }
    }
}

void Game::movementSystem()
{
    m_player->c_transform->velocity = {0, 0};

    if (m_player->c_input->up)
    {
        m_player->c_transform->velocity.y = -m_playerConfig.S;
    }
    if (m_player->c_input->left)
    {
        m_player->c_transform->velocity.x = -m_playerConfig.S;
    }
    if (m_player->c_input->right)
    {
        m_player->c_transform->velocity.x = m_playerConfig.S;
    }
    if (m_player->c_input->down)
    {
        m_player->c_transform->velocity.y = m_playerConfig.S;
    }

    for (auto& e : m_entityManager.getEntities())
    {
        e->c_transform->position += e->c_transform->velocity;
    }
}

void Game::collisionSystem()
{
    for (auto& entity : m_entityManager.getEntities())
    {
        if (entity->getTag() == ENTITY_TAG::Player)
        {
            if (entity->c_transform->position.x + entity->c_collision->radius >= m_window.getSize().x ||
                entity->c_transform->position.x - entity->c_collision->radius <= 0)
            {
                entity->c_transform->position.x -= entity->c_transform->velocity.x;
            }

            if (entity->c_transform->position.y + entity->c_collision->radius >= m_window.getSize().y ||
                entity->c_transform->position.y - entity->c_collision->radius <= 0)
            {
                entity->c_transform->position.y -= entity->c_transform->velocity.y;
            }
        }
        else
        {
            if (entity->c_transform->position.x + entity->c_collision->radius >= m_window.getSize().x ||
                entity->c_transform->position.x - entity->c_collision->radius <= 0)
            {
                entity->c_transform->velocity.x *= -1;
            }

            if (entity->c_transform->position.y + entity->c_collision->radius >= m_window.getSize().y ||
                entity->c_transform->position.y - entity->c_collision->radius <= 0)
            {
                entity->c_transform->velocity.y *= -1;
            }

            for (auto& player : m_entityManager.getEntities(ENTITY_TAG::Player))
            {
                if (entity->getTag() == ENTITY_TAG::Bullet) continue;
                if (player->c_transform->position.distanceSquare(entity->c_transform->position) <=
                    std::pow(player->c_collision->radius + entity->c_collision->radius, 2))
                {
                    //makes sure the player is alive and does not spawn 2 players
                    if (player->isActive())
                    {
                        m_score = 0;
                        entity->destroy();
                        player->destroy();
                        spawnPlayer();
                    }
                }
            }

            for (auto& enemy : m_entityManager.getEntities(ENTITY_TAG::Enemy))
            {
                if (entity->getId() == enemy->getId()) continue;
                if (entity->c_transform->position.distanceSquare(enemy->c_transform->position) <=
                    std::pow(entity->c_collision->radius + enemy->c_collision->radius, 2))
                {
                    if (entity->getTag() == ENTITY_TAG::Bullet)
                    {
                        m_score += 100;
                        enemy->destroy();
                        entity->destroy();
                    }
                    else if (entity->getTag() == ENTITY_TAG::Enemy)
                    {
                        const float distance = entity->c_transform->position.distance(enemy->c_transform->position);
                        const float overlap = (distance - entity->c_collision->radius - enemy->c_collision->radius);
                        const float dx = (
                            entity->c_transform->position.x -
                            enemy->c_transform->position.x
                        ) / distance;

                        const float dy = (
                            entity->c_transform->position.y -
                            enemy->c_transform->position.y
                        ) / distance;

                        entity->c_transform->velocity.x -= 0.5f * overlap * dx;
                        entity->c_transform->velocity.y -= 0.5f * overlap * dy;
                    }
                }
            }
        }
    }
}

void Game::userInputSystem()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
            m_running = false;
        }
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::Escape:
                m_running = false;
                break;
            case sf::Keyboard::P:
                pause(!m_paused);
                break;
            case sf::Keyboard::A:
                m_player->c_input->left = true;
                break;
            case sf::Keyboard::D:
                m_player->c_input->right = true;
                break;
            case sf::Keyboard::W:
                m_player->c_input->up = true;
                break;
            case sf::Keyboard::S:
                m_player->c_input->down = true;
                break;
            default: break;
            }
        }
        if (event.type == sf::Event::KeyReleased)
        {
            switch (event.key.code)
            {
            case sf::Keyboard::A:
                m_player->c_input->left = false;
                break;
            case sf::Keyboard::D:
                m_player->c_input->right = false;
                break;
            case sf::Keyboard::W:
                m_player->c_input->up = false;
                break;
            case sf::Keyboard::S:
                m_player->c_input->down = false;
                break;
            default: break;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed)
        {
            switch (event.mouseButton.button)
            {
            case sf::Mouse::Left:
                spawnBullet(m_player, Vec2(event.mouseButton.x, event.mouseButton.y));
                break;
            case (sf::Mouse::Right):
                spawnSpecialWeapon(m_player);
                break;
            default: break;
            }
        }
    }
}

void Game::enemySpawnerSystem()
{
    //Enemy Spawner
    if ((m_currentFrame - m_lastEnemySpawnTime) >= m_enemyConfig.SI)
    {
        spawnEnemy();
    }
}

void Game::spawnEnemy()
{
    const int min_pos_x{m_enemyConfig.CR};
    const int min_pos_y{m_enemyConfig.CR};
    const int max_pos_x{static_cast<int>(m_window.getSize().x - m_enemyConfig.CR)};
    const int max_pos_y{static_cast<int>(m_window.getSize().y - m_enemyConfig.CR)};
    Vec2 rand_pos
    {
        static_cast<float>(min_pos_x + (rand() % (max_pos_x - min_pos_x + 1))),
        static_cast<float>(min_pos_y + (rand() % (max_pos_y - min_pos_y + 1)))
    };

    int vertices{m_enemyConfig.VMIN + (rand() % (m_enemyConfig.VMAX - m_enemyConfig.VMIN + 1))};

    float rand_speed
    {
        m_enemyConfig.SMIN + rand() % static_cast<int>((m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1))
    };
    //Makes sure the velocity is not 0
    while (std::fabs(rand_speed - 0) <= std::numeric_limits<float>::epsilon())
    {
        rand_speed = m_enemyConfig.SMIN + rand() % static_cast<int>((m_enemyConfig.SMAX - m_enemyConfig.SMIN + 1));
    }

    sf::Color fill_color
    {
        static_cast<sf::Uint8>(rand() % (1 + 255)),
        static_cast<sf::Uint8>(rand() % (1 + 255)),
        static_cast<sf::Uint8>(rand() % (1 + 255))
    };

    sf::Color outline_color
    {
        static_cast<sf::Uint8>(m_enemyConfig.OR),
        static_cast<sf::Uint8>(m_enemyConfig.OG),
        static_cast<sf::Uint8>(m_enemyConfig.OB)
    };

    const auto enemy = m_entityManager.addEntity(ENTITY_TAG::Enemy);
    enemy->c_transform = std::make_shared<CTransform>(rand_pos, Vec2(rand_speed, rand_speed), 0.0f);
    enemy->c_shape = std::make_shared<CShape>(m_enemyConfig.SR, vertices, fill_color, outline_color, m_enemyConfig.OT);
    enemy->c_collision = std::make_shared<CCollision>(m_enemyConfig.CR);
    m_lastEnemySpawnTime = m_currentFrame;
}

void Game::spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2& target)
{
    const auto bullet = m_entityManager.addEntity(ENTITY_TAG::Bullet);
    sf::Color fill_color
    {
        static_cast<sf::Uint8>(m_bulletConfig.FR),
        static_cast<sf::Uint8>(m_bulletConfig.FG),
        static_cast<sf::Uint8>(m_bulletConfig.FB)
    };

    sf::Color outline_color
    {
        static_cast<sf::Uint8>(m_bulletConfig.OR),
        static_cast<sf::Uint8>(m_bulletConfig.OG),
        static_cast<sf::Uint8>(m_bulletConfig.OB)
    };

    bullet->c_shape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
                                               fill_color, outline_color, m_bulletConfig.OT);
    bullet->c_lifespan = std::make_shared<CLifespan>(m_bulletConfig.L);
    bullet->c_collision = std::make_shared<CCollision>(m_bulletConfig.CR);
    Vec2 difference = target - entity->c_transform->position;
    difference.normalize();
    Vec2 velocity{m_bulletConfig.S * difference.x, m_bulletConfig.S * difference.y};
    bullet->c_transform = std::make_shared<CTransform>(entity->c_transform->position, velocity, 0.0f);
}

void Game::spawnPlayer()
{
    const auto player = m_entityManager.addEntity(ENTITY_TAG::Player);
    Vec2 player_position(static_cast<float>(m_window.getSize().x) * 0.5f,
                         static_cast<float>(m_window.getSize().y) * 0.5f);
    player->c_transform = std::make_shared<CTransform>(player_position, Vec2(m_playerConfig.S, m_playerConfig.S), 0.0f);
    player->c_shape = std::make_shared<CShape>(m_playerConfig.SR, m_playerConfig.V,
                                               sf::Color(m_playerConfig.FR, m_playerConfig.FG, m_playerConfig.FB),
                                               sf::Color(m_playerConfig.OR, m_playerConfig.OG, m_playerConfig.OB),
                                               m_playerConfig.OT);
    player->c_collision = std::make_shared<CCollision>(m_playerConfig.CR);
    player->c_input = std::make_shared<CInput>();
    m_player = player;
}

void Game::spawnSmallEnemies(std::shared_ptr<Entity> entity)
{
}

void Game::spawnSpecialWeapon(const std::shared_ptr<Entity>& entity)
{
    float angle{0};

    for (int j{0}; j < 36; ++j)
    {
        const auto bullet = m_entityManager.addEntity(ENTITY_TAG::Bullet);

        bullet->c_shape = std::make_shared<CShape>(m_bulletConfig.SR, m_bulletConfig.V,
                                                   sf::Color(m_bulletConfig.FR, m_bulletConfig.FG, m_bulletConfig.FB),
                                                   sf::Color(m_bulletConfig.OR, m_bulletConfig.OG, m_bulletConfig.OB),
                                                   m_bulletConfig.OT);

        bullet->c_collision = std::make_shared<CCollision>(m_bulletConfig.CR);

        bullet->c_lifespan = std::make_shared<CLifespan>(m_bulletConfig.L * 10);


        Vec2 normalized_pos = entity->c_transform->position;
        normalized_pos.normalize();

        //Calculate the velocity

        const float radians{angle * 3.1415926f / 180.0f};

        Vec2 velocity
        {
            std::cos(radians) * normalized_pos.x + std::sin(radians) * normalized_pos.y,
            std::sin(radians) * normalized_pos.x - std::cos(radians) * normalized_pos.y,
        };

        const float l{velocity.length()};

        const Vec2 normalized_velocity{velocity.x / l, velocity.y / l};

        Vec2 new_velocity{normalized_velocity.x * m_bulletConfig.S, normalized_velocity.y * m_bulletConfig.S};

        bullet->c_transform = std::make_shared<CTransform>(m_player->c_transform->position, new_velocity, 0);

        angle += 360 / 36.0f;
    }
}

float Game::deltaTime() const
{
    return m_elapsedTime.asSeconds() * 100;
}

Game::Game(const std::string& configFilePath)
{
    initialization(configFilePath);
}

void Game::run()
{
    while (m_running)
    {
        m_entityManager.update();

        if (!m_paused)
        {
            enemySpawnerSystem();
            movementSystem();
            collisionSystem();
            lifespanSystem();
        }

        userInputSystem();
        renderSystem();
        m_elapsedTime = m_clock.restart();
        m_currentFrame++; // Need to be moved when pause implemented
    }
}
