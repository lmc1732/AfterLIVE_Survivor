#include "GameManager.h"
#include "Weapon.h"
#include "HomingBullet.h"
#include "StatUpgrade.h"
#include "SpecialUpgrade.h"
#include <QRandomGenerator>
#include <QPointF>
#include <QDebug>
#include <cmath>

GameManager::GameManager(QObject* parent)
    : QObject(parent)
    , m_player(new Player(this))
{
    m_player->setCharacterType(Player::CharacterA);
    m_playerX = 400;
    m_playerY = 300;

    connect(m_player, &Player::levelUpRequested, this, &GameManager::levelUp);
}

QList<QObject*> GameManager::enemies() const
{
    QList<QObject*> list;
    for (Enemy* e : m_enemies)
        list.append(e);
    return list;
}

QList<QObject*> GameManager::bullets() const
{
    QList<QObject*> list;
    for (Bullet* b : m_bullets)
        list.append(b);
    return list;
}

QList<QObject*> GameManager::explosions() const
{
    QList<QObject*> list;
    for (Explosion* e : m_explosions)
        list.append(e);
    return list;
}

QList<QObject*> GameManager::expBalls() const
{
    QList<QObject*> list;
    for (ExpBall* e : m_expBalls)
        list.append(e);
    return list;
}

void GameManager::addExplosion(Explosion* explosion)
{
    m_explosions.append(explosion);
    emit explosionsChanged();
}

void GameManager::addExpBall(ExpBall* ball)
{
    m_expBalls.append(ball);
    emit expBallsChanged();
}

void GameManager::selectCharacter(int type)
{
    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }

    m_player = new Player(this);
    Player::CharacterType charType = (type == 0) ? Player::CharacterA : Player::CharacterB;
    m_player->setCharacterType(charType);

    connect(m_player, &Player::levelUpRequested, this, &GameManager::levelUp);

    qDebug() << "Selected character type:" << (type == 0 ? "A" : "B");
}

void GameManager::update(qreal deltaTime, qreal playerX, qreal playerY)
{
    m_playerX = playerX;
    m_playerY = playerY;

    // Update game timer (only when game is active)
    if (!m_gameOver && !m_gamePaused) {
        m_gameTime += deltaTime;
        emit gameTimeChanged();
    }

    if (m_gameOver || m_gamePaused) {
        for (Explosion* exp : m_explosions) {
            exp->update(deltaTime);
        }
        cleanupDeadObjects();
        return;
    }

    qreal cappedDeltaTime = qMin(deltaTime, 0.033f);
    QPointF playerPos(playerX, playerY);

    m_player->updateInvincibility();

    if (m_player->hp() < m_player->maxHp() && m_player->hpRegen() > 0) {
        m_regenTimer += cappedDeltaTime;
        if (m_regenTimer >= 1.0) {
            int regenAmount = m_player->hpRegen();
            m_player->setHp(m_player->hp() + regenAmount);
            m_regenTimer -= 1.0;
        }
    }

    Weapon* weapon = m_player->weapon();
    if (weapon) {
        weapon->attack(playerPos, m_enemies, m_bullets, cappedDeltaTime);
    }

    for (Bullet* bullet : m_bullets) {
        bullet->update(cappedDeltaTime);
    }

    for (Enemy* enemy : m_enemies) {
        enemy->updateStatusEffects(cappedDeltaTime);
        enemy->update(playerPos, cappedDeltaTime);
    }

    for (Explosion* exp : m_explosions) {
        exp->update(cappedDeltaTime);
    }

    checkCollisions();

    m_spawnTimer += cappedDeltaTime;
    while (m_spawnTimer >= m_spawnInterval) {
        m_spawnTimer -= m_spawnInterval;
        spawnEnemy();
    }

    cleanupDeadObjects();

    if (m_player->isDead()) {
        m_gameOver = true;
        emit gameOverChanged();
        emit playerDied();
        qDebug() << "Game Over!";
    }
}

void GameManager::restartGame()
{
    // Clear all game objects
    for (Enemy* enemy : m_enemies) {
        delete enemy;
    }
    m_enemies.clear();

    for (Bullet* bullet : m_bullets) {
        delete bullet;
    }
    m_bullets.clear();

    for (Explosion* exp : m_explosions) {
        delete exp;
    }
    m_explosions.clear();

    for (ExpBall* ball : m_expBalls) {
        delete ball;
    }
    m_expBalls.clear();

    for (UpgradeItem* item : m_currentUpgrades) {
        delete item;
    }
    m_currentUpgrades.clear();

    // Recreate the player
    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }

    m_player = new Player(this);
    m_player->setCharacterType(Player::CharacterA);

    // Reconnect upgrade signals
    connect(m_player, &Player::levelUpRequested, this, &GameManager::levelUp);

    // Reset game state
    m_gameOver = false;
    m_gamePaused = false;
    m_isWaitingForUpgrade = false;
    m_spawnTimer = 0;
    m_regenTimer = 0;
    m_playerX = 400;
    m_playerY = 300;
    m_gameTime = 0;  // Reset timer
    emit gameTimeChanged();

    emit enemiesChanged();
    emit bulletsChanged();
    emit explosionsChanged();
    emit expBallsChanged();
    emit gameOverChanged();
    emit gamePausedChanged();

    qDebug() << "Game restarted!";
}

void GameManager::levelUp()
{
    if (m_isWaitingForUpgrade || m_gameOver) return;

    // Clear old options
    for (auto item : m_currentUpgrades) {
        delete item;
    }
    m_currentUpgrades.clear();

    m_currentUpgrades = generateRandomUpgrades();
    m_isWaitingForUpgrade = true;
    m_gamePaused = true;
    emit gamePausedChanged();

    QStringList names, descs, icons;
    for (auto item : m_currentUpgrades) {
        names << item->name();
        descs << item->description();
        icons << item->iconPath();
    }
    emit showUpgradeOptions(names, descs, icons);
}

void GameManager::selectUpgrade(int index)
{
    if (!m_isWaitingForUpgrade) return;
    if (index < 0 || index >= m_currentUpgrades.size()) return;

    m_currentUpgrades[index]->apply(m_player);

    // Remove unselected items
    for (int i = 0; i < m_currentUpgrades.size(); ++i) {
        if (i != index) {
            delete m_currentUpgrades[i];
        }
    }
    m_currentUpgrades.clear();
    m_isWaitingForUpgrade = false;
    m_gamePaused = false;
    emit gamePausedChanged();
}

QList<UpgradeItem*> GameManager::generateRandomUpgrades()
{
    QList<UpgradeItem*> options;
    QVector<UpgradeItem*> pool;

    // Base Stat Item
    pool.append(new StatUpgrade(StatUpgrade::MaxHp, 20, "Rice", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::MaxHp, 20, "Soft Blanket", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::HpRegen, 2, "Dubai Chocolate", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::HpRegen, 2, "Braised Spare Ribs", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Armor, 3, "Scarf", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Armor, 3, "Gat", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::MoveSpeed, 20, "Wings", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::MoveSpeed, 20, "Ribbon", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Power, 5, "Megaphone", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Power, 5, "Flame", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Area, 15, "Windmill", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::Area, 15, "Bubble", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::AttackSpeed, 5, "Wind Chime", "qrc:/images/placeholder.png", this));
    pool.append(new StatUpgrade(StatUpgrade::AttackSpeed, 5, "Rattle", "qrc:/images/placeholder.png", this));

    // Special Effect Item
    pool.append(new SpecialUpgrade(SpecialUpgrade::BraveSword, "Hero Sword", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::Shield, "Glow Shield", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::SilenceBow, "Silence Bow", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::LoveLute, "Love Lute", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::WishBranch, "Wishing Branch", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::WarmHammer, "Warm Flame Hammer", "qrc:/images/placeholder.png", this));

    // Randomly select 3 distinct items
    QList<int> indices;
    while (indices.size() < 3 && indices.size() < pool.size()) {
        int idx = QRandomGenerator::global()->bounded(pool.size());
        if (!indices.contains(idx)) {
            indices.append(idx);
            options.append(pool[idx]);
        }
    }
    return options;
}

void GameManager::checkCollisions()
{
    checkBulletEnemyCollision();
    checkPlayerEnemyCollision();
    checkPlayerExpBallCollision();
}

void GameManager::checkBulletEnemyCollision()
{
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet* bullet = m_bullets[i];
        if (bullet->hasHit()) continue;

        qreal bulletX = bullet->x();
        qreal bulletY = bullet->y();
        qreal bulletRadius = 8.0;

        for (int j = m_enemies.size() - 1; j >= 0; --j) {
            Enemy* enemy = m_enemies[j];
            if (enemy->isDead()) continue;

            qreal enemyX = enemy->x();
            qreal enemyY = enemy->y();
            qreal enemyRadius = 15.0;

            qreal dx = bulletX - enemyX;
            qreal dy = bulletY - enemyY;
            qreal dist = std::hypot(dx, dy);

            if (dist < bulletRadius + enemyRadius) {
                int finalDamage = static_cast<int>(bullet->damage() * m_player->damageMultiplier());
                enemy->takeDamage(finalDamage);
                bullet->setHit();

                if (m_player->hasBurnEffect()) {
                    enemy->applyBurn(10, 3.0);
                }
                break;
            }
        }
    }
}

void GameManager::checkPlayerEnemyCollision()
{
    if (m_player->isDead() || m_gameOver) return;

    qreal playerX = m_playerX;
    qreal playerY = m_playerY;
    qreal playerRadius = 20.0;

    for (Enemy* enemy : m_enemies) {
        if (enemy->isDead()) continue;

        qreal enemyX = enemy->x();
        qreal enemyY = enemy->y();
        qreal enemyRadius = 15.0;

        qreal dx = playerX - enemyX;
        qreal dy = playerY - enemyY;
        qreal dist = std::hypot(dx, dy);

        if (dist < playerRadius + enemyRadius) {
            // Player takes damage, no damage to enemies
            m_player->takeDamage(enemy->damage());
            qDebug() << "Player hit! HP:" << m_player->hp();
        }
    }
}

void GameManager::checkPlayerExpBallCollision()
{
    if (m_gameOver) return;

    qreal playerX = m_playerX;
    qreal playerY = m_playerY;
    qreal playerRadius = 25.0;

    for (int i = m_expBalls.size() - 1; i >= 0; --i) {
        ExpBall* ball = m_expBalls[i];
        if (ball->isCollected()) continue;

        qreal ballX = ball->x();
        qreal ballY = ball->y();
        qreal ballRadius = 12.0;

        qreal dx = playerX - ballX;
        qreal dy = playerY - ballY;
        qreal dist = std::hypot(dx, dy);

        if (dist < playerRadius + ballRadius) {
            m_player->addExp(ball->expValue());
            delete ball;
            m_expBalls.removeAt(i);
            qDebug() << "ExpBall collected!";
        }
    }
    emit expBallsChanged();
}

void GameManager::spawnEnemy()
{
    Enemy* enemy = new Enemy(this);
    qreal x = QRandomGenerator::global()->bounded(0, 800);
    qreal y = QRandomGenerator::global()->bounded(0, 600);
    enemy->setX(x);
    enemy->setY(y);
    enemy->setHp(30);
    enemy->setDamage(10);
    enemy->setExpReward(10);

    connect(enemy, &Enemy::died, this, [this, enemy]() {
        if (m_player->hasHealOnKill()) {
            int healAmount = 5;
            m_player->setHp(qMin(m_player->hp() + healAmount, m_player->maxHp()));
            qDebug() << "Healed by LoveLute:" << healAmount;
        }

        ExpBall* ball = new ExpBall(this);
        ball->setX(enemy->x());
        ball->setY(enemy->y());
        ball->setExpValue(enemy->expReward());
        addExpBall(ball);
        });

    m_enemies.append(enemy);
    emit enemiesChanged();
}

void GameManager::cleanupDeadObjects()
{
    // Remove dead enemies
    for (int i = m_enemies.size() - 1; i >= 0; --i) {
        if (m_enemies[i]->isDead()) {
            delete m_enemies[i];
            m_enemies.removeAt(i);
        }
    }
    emit enemiesChanged();

    // Clear bullets
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (m_bullets[i]->isBeingDeleted()) {
            delete m_bullets[i];
            m_bullets.removeAt(i);
        }
    }
    emit bulletsChanged();

    // Clear explosions
    for (int i = m_explosions.size() - 1; i >= 0; --i) {
        if (m_explosions[i]->isFinished()) {
            delete m_explosions[i];
            m_explosions.removeAt(i);
        }
    }
    emit explosionsChanged();
}