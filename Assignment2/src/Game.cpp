#include <iostream>
#include <fstream>

#include "Game.h"

#include <memory>
#include <cmath>

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
        // m_window.setVerticalSyncEnabled(true);
    }
    else
    {
        m_window.create(sf::VideoMode(window_w, window_h), "Assignment 2");
        m_window.setFramerateLimit(frame_limit);
    }

    spawnPlayer();
}

void Game::renderSystem()
{
    m_window.clear();
    for (auto& e : m_entityManager.getEntities())
    {
        e->c_shape->shape.setPosition(e->c_transform->position.x, e->c_transform->position.y);
        e->c_transform->angle += 5.0f * deltaTime();
        e->c_shape->shape.setRotation(e->c_transform->angle);
        m_window.draw(e->c_shape->shape);
    }

    m_window.display();
}

void Game::lifespanSystem()
{
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
        e->c_transform->position += e->c_transform->velocity * deltaTime();
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
                if (player->c_transform->position.distanceSquare(entity->c_transform->position) <=
                    std::pow(player->c_collision->radius + entity->c_collision->radius, 2))
                {
                    //makes sure the player is alive and does not spawn 2 players
                    if (player->isActive())
                    {
                        m_score = 0;
                        m_text.setString(std::to_string(m_score));
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
                    entity->c_transform->velocity *= -1.0f;
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

void Game::spawnBullet()
{
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

void Game::spawnSmallEnemies()
{
}

void Game::spawnSpecialWeapon()
{
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
        }

        userInputSystem();
        renderSystem();
        m_elapsedTime = m_clock.restart();
        m_currentFrame++; // Need to be moved when pause implemented
    }
}
