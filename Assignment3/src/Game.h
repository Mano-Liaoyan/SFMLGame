#pragma once

#include <string>

#include <SFML/Graphics.hpp>

#include "Entity.h"
#include "EntityManager.h"

struct PlayerConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V;
    float S;
};

struct EnemyConfig
{
    int SR, CR, OR, OG, OB, OT, VMIN, VMAX, L, SI;
    float SMIN, SMAX;
};

struct BulletConfig
{
    int SR, CR, FR, FG, FB, OR, OG, OB, OT, V, L;
    float S;
};

class Game
{
    sf::Font m_font;
    sf::Text m_text;
    sf::Clock m_clock;
    sf::Time m_elapsedTime;
    sf::RenderWindow m_window; // the window we draw to

    EnemyConfig m_enemyConfig;
    PlayerConfig m_playerConfig;
    BulletConfig m_bulletConfig;
    EntityManager m_entityManager; // a vector of entities we are going to maintain

    int m_score = 0;
    bool m_paused = false; // whether we update the game logic
    bool m_running = true; // whether the game loop is running
    int m_currentFrame = 0;
    int m_lastEnemySpawnTime = 0;

    std::string m_configFilePath;
    std::shared_ptr<Entity> m_player; // the player entity pointer

    void pause(bool paused); // set if the game was paused
    void initialization(const std::string& path);

    void renderSystem();
    void lifespanSystem();
    void movementSystem();
    void collisionSystem();
    void userInputSystem();
    void enemySpawnerSystem();

    void spawnEnemy();
    void spawnBullet(const std::shared_ptr<Entity>& entity, const Vec2& target);
    void spawnPlayer();
    void spawnSmallEnemies(std::shared_ptr<Entity> entity);
    void spawnSpecialWeapon(const std::shared_ptr<Entity>& entity);

    float deltaTime() const;

public:
    Game(const std::string& configFilePath);

    void run();
};
