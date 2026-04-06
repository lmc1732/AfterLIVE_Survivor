#include "GameManager.h"
#include "Weapon.h"
#include "HomingBullet.h"
#include "StatUpgrade.h"
#include "SpecialUpgrade.h"
#include "EnemyFactory.h"
#include "SporeMistEffect.h"
#include "SporeMistSkill.h"
#include "RootProjectile.h"
#include <QRandomGenerator>
#include <QPointF>
#include <QTimer>
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

    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &GameManager::spawnEnemy);
    m_spawnTimer->start(static_cast<int>(m_spawnInterval * 1000));
}

QList<QObject*> GameManager::enemies() const
{
    qDebug() << "enemies() called, m_enemies.size() =" << m_enemies.size();
    QList<QObject*> list;
    for (Enemy* e : m_enemies) {
        list.append(e);
    }
    qDebug() << "returning list size =" << list.size();
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
    if (m_expBalls.size() > 50) {
        delete ball;
        return;
    }
    m_expBalls.append(ball);
    emit expBallsChanged();
}

void GameManager::addBullet(Bullet* bullet)
{
    if (!bullet) return;
    if (m_bullets.size() > 200) {
        delete bullet;
        return;
    }
    m_bullets.append(bullet);
    emit bulletsChanged();
    qDebug() << "Bullet added, total:" << m_bullets.size();
}

void GameManager::addSporeMistEffect(SporeMistEffect* effect)
{
    m_sporeMistEffects.append(effect);
    emit sporeMistEffectsChanged();
}

QList<QObject*> GameManager::sporeMistEffects() const
{
    QList<QObject*> list;
    for (SporeMistEffect* e : m_sporeMistEffects)
        list.append(e);
    return list;
}

void GameManager::addEnemyDirect(Enemy* enemy)
{
    if (!enemy) return;
    m_enemies.append(enemy);
    emit enemiesChanged();
    connect(enemy, &Enemy::died, this, &GameManager::onEnemyDied);
    if (enemy->type() == EnemyType::RaccoonBoss) {
        enemy->startBossLogic(this);
    }
}

void GameManager::setPlayerPosition(qreal x, qreal y)
{
    m_playerX = x;
    m_playerY = y;
}

void GameManager::spawnEnemy()
{
    if (m_gameOver || m_gamePaused) return;
    if (m_bossSpawned && m_enemies.size() > 15) return;

    EnemyType type;
    if (m_gameTime < WAVE1_END) {
        type = EnemyType::MossBall;
    }
    else if (m_gameTime < WAVE2_END) {
        int r = QRandomGenerator::global()->bounded(2);
        type = (r == 0) ? EnemyType::MossBall : EnemyType::BranchGunner;
    }
    else if (m_gameTime < WAVE3_END) {
        int r = QRandomGenerator::global()->bounded(3);
        if (r == 0) type = EnemyType::MossBall;
        else if (r == 1) type = EnemyType::BranchGunner;
        else type = EnemyType::BerryBomb;
    }
    else {
        int r = QRandomGenerator::global()->bounded(4);
        if (r == 0) type = EnemyType::MossBall;
        else if (r == 1) type = EnemyType::BranchGunner;
        else if (r == 2) type = EnemyType::BerryBomb;
        else type = EnemyType::BarkShield;
    }

    Enemy* enemy = EnemyFactory::createEnemy(type, this);
    qreal x = QRandomGenerator::global()->bounded(0, 800);
    qreal y = QRandomGenerator::global()->bounded(0, 600);
    enemy->setX(x);
    enemy->setY(y);

    connect(enemy, &Enemy::died, this, [this, enemy]() {
        if (m_player->hasHealOnKill()) {
            int healAmount = 5;
            m_player->setHp(qMin(m_player->hp() + healAmount, m_player->maxHp()));
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

void GameManager::update(qreal deltaTime, qreal playerX, qreal playerY)
{
    // If game is over or paused, do not perform any updates
    if (m_gameOver || m_gamePaused) {
        return;
    }

    m_playerX = playerX;
    m_playerY = playerY;

    if (!m_gameOver && !m_gamePaused) {
        m_gameTime += deltaTime;
        emit gameTimeChanged();
    }

    if (!m_bossSpawned && !m_gameOver && !m_gamePaused && m_gameTime >= WAVE4_END) {
        m_bossSpawned = true;
        m_boss = EnemyFactory::createEnemy(EnemyType::RaccoonBoss, this);
        m_boss->setX(400);
        m_boss->setY(200);

        connect(m_boss, &Enemy::died, this, [this]() {
            if (!m_gameWinHandled) {
                m_gameWinHandled = true;
                qDebug() << "Boss defeated! Victory!";
                m_gameOver = true;
                emit gameWon();
                emit gameOverChanged();
            }
            });

        addEnemyDirect(m_boss);
        qDebug() << "Boss spawned at time:" << m_gameTime;
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
    m_player->updateEffects(cappedDeltaTime);


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
        if (enemy && !enemy->isDead()) {
            enemy->updateStatusEffects(cappedDeltaTime);
            enemy->update(playerPos, cappedDeltaTime);
            enemy->updateActiveSkill(cappedDeltaTime, this);
        }
    }

    // Active skill cooldown management (placed after enemy movement updates)
    for (Enemy* enemy : m_enemies) {
        if (enemy->isDead()) continue;
        static QMap<Enemy*, qreal> skillTimers;
        qreal& timer = skillTimers[enemy];
        timer += cappedDeltaTime;
        if (timer >= 4.0 && enemy->getSkill()) {
            timer = 0;
            enemy->getSkill()->activate(enemy, nullptr, this);
        }
    }

    for (Explosion* exp : m_explosions) {
        exp->update(cappedDeltaTime);
    }

    checkCollisions();

    // Update fog and detect vulnerable areas
    for (int i = m_sporeMistEffects.size() - 1; i >= 0; --i) {
        SporeMistEffect* mist = m_sporeMistEffects[i];
        mist->update(cappedDeltaTime);
        if (mist->isExpired()) {
            delete mist;
            m_sporeMistEffects.removeAt(i);
        }
    }
    emit sporeMistEffectsChanged();

    // Check if player is inside any fog (for vulnerability)
    bool inDamageMist = false;
    for (SporeMistEffect* mist : m_sporeMistEffects) {
        qreal dx = m_playerX - mist->x();
        qreal dy = m_playerY - mist->y();
        qreal dist = std::hypot(dx, dy);
        if (dist < mist->radius()) {
            inDamageMist = true;
            break;
        }
    }

    // Update member variables
    m_playerInDamageMist = inDamageMist;

    // Control left panel display effect (add/remove on enter/leave)
    if (inDamageMist && !m_prevInDamageMist) {
        m_player->addEffect("🍄 Spore Mist (20% Vulnerability)", 3.0);
    }
    else if (!inDamageMist && m_prevInDamageMist) {
        m_player->removeEffect("🍄 Spore Mist (20% Vulnerability)");
    }
    m_prevInDamageMist = inDamageMist;

    cleanupDeadObjects();

    if (m_player->isDead()) {
        m_gameOver = true;
        emit gameOverChanged();
        emit playerDied();
        qDebug() << "Game Over!";
    }
}

void GameManager::spawnEnemyAt(EnemyType type, qreal x, qreal y)
{
    Enemy* enemy = EnemyFactory::createEnemy(type, this);
    if (!enemy) return;
    enemy->setX(x);
    enemy->setY(y);
    addEnemyDirect(enemy);
}

void GameManager::resetGameData()
{
    // Stop timer
    if (m_spawnTimer) {
        m_spawnTimer->stop();
        m_spawnTimer->deleteLater();
        m_spawnTimer = nullptr;
    }

    // Clear all game objects
    for (Enemy* enemy : m_enemies) {
        if (enemy->type() == EnemyType::RaccoonBoss) {
            enemy->stopBossLogic();
        }
        enemy->disconnect();
        delete enemy;
    }
    m_enemies.clear();
    for (Bullet* bullet : m_bullets) delete bullet;
    m_bullets.clear();
    for (Explosion* exp : m_explosions) delete exp;
    m_explosions.clear();
    for (ExpBall* ball : m_expBalls) delete ball;
    m_expBalls.clear();
    for (UpgradeItem* item : m_currentUpgrades) delete item;
    m_currentUpgrades.clear();
    for (SporeMistEffect* effect : m_sporeMistEffects) delete effect;
    m_sporeMistEffects.clear();
    m_playerInDamageMist = false;
    m_prevInDamageMist = false;
    m_skillTimers.clear();

    // Reset game state
    m_gameTime = 0;
    m_bossSpawned = false;
    m_boss = nullptr;
    m_gameWinHandled = false;
    m_gameOver = false;
    m_gamePaused = false;
    m_isWaitingForUpgrade = false;
    m_regenTimer = 0;
    m_playerX = 400;
    m_playerY = 300;

    // Emit list changed signal
    emit enemiesChanged();
    emit bulletsChanged();
    emit explosionsChanged();
    emit expBallsChanged();
    emit gameOverChanged();
    emit gamePausedChanged();
    emit gameTimeChanged();

    qDebug() << "Game data reset.";
}

void GameManager::restartGame()
{
    resetGameData();  // Reuse reset logic
    qDebug() << "Game restarted (data only, timer not started).";
}

void GameManager::selectCharacter(int type)
{
    resetGameData();  // Reset data, stop and delete old timer

    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }
    m_player = new Player(this);
    Player::CharacterType charType = (type == 0) ? Player::CharacterA : Player::CharacterB;
    m_player->setCharacterType(charType);
    connect(m_player, &Player::levelUpRequested, this, &GameManager::levelUp);
    emit playerChanged();

    // Start timer (start game)
    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &GameManager::spawnEnemy);
    m_spawnTimer->start(static_cast<int>(m_spawnInterval * 1000));

    qDebug() << "Character selected and game started:" << (type == 0 ? "A" : "B");
}

void GameManager::setIsGamePaused(bool paused)
{
    if (m_gamePaused == paused) return;
    m_gamePaused = paused;
    emit gamePausedChanged();
}

void GameManager::setSpawnTimerEnabled(bool enabled)
{
    if (!m_spawnTimer) return;
    if (enabled) {
        m_spawnTimer->start();
    }
    else {
        m_spawnTimer->stop();
    }
}

void GameManager::levelUp()
{
    if (m_isWaitingForUpgrade || m_gameOver) return;

    for (auto item : m_currentUpgrades) delete item;
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

    for (int i = 0; i < m_currentUpgrades.size(); ++i) {
        if (i != index) delete m_currentUpgrades[i];
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

    pool.append(new SpecialUpgrade(SpecialUpgrade::BraveSword, "Hero Sword", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::Shield, "Glow Shield", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::SilenceBow, "Silence Bow", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::LoveLute, "Love Lute", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::WishBranch, "Wishing Branch", "qrc:/images/placeholder.png", this));
    pool.append(new SpecialUpgrade(SpecialUpgrade::WarmHammer, "Warm Flame Hammer", "qrc:/images/placeholder.png", this));

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
    checkPlayerRootCollision();
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
            qreal enemyRadius = enemy->size() / 2.0;

            qreal dx = bulletX - enemyX;
            qreal dy = bulletY - enemyY;
            qreal dist = std::hypot(dx, dy);

            if (dist < bulletRadius + enemyRadius) {
                // Bark Shield blocks and bullet is not the rebound source
                if (enemy->type() == EnemyType::BarkShield && enemy->isBlocking() && !bullet->isRebounded()) {
                    // Create rebound bullet
                    Bullet* rebounded = new Bullet(this);
                    rebounded->setX(enemyX);
                    rebounded->setY(enemyY);

                    //Basic rebound direction (opposite of incoming direction)
                    qreal vx = -bullet->velX();
                    qreal vy = -bullet->velY();

                    //Light homing: Calculate direction from rebound point to player
                    QPointF dirToPlayer(m_playerX - enemyX, m_playerY - enemyY);
                    qreal len = std::hypot(dirToPlayer.x(), dirToPlayer.y());
                    if (len > 0.01) {
                        dirToPlayer /= len;
                        // Mix rebound direction with homing direction (50% each)
                        vx = (vx + dirToPlayer.x()) * 0.5;
                        vy = (vy + dirToPlayer.y()) * 0.5;
                        // Normalize
                        len = std::hypot(vx, vy);
                        if (len > 0.01) {
                            vx /= len;
                            vy /= len;
                        }
                    }

                    //Random offset of ±10 degrees (adds unpredictability)
                    qreal angle = std::atan2(vy, vx);
                    angle += (QRandomGenerator::global()->bounded(20) - 10) * M_PI / 180.0;
                    vx = std::cos(angle);
                    vy = std::sin(angle);

                    rebounded->setVelocity(vx, vy);
                    rebounded->setSpeed(bullet->speed() * 1.5);   // Increase speed by 50%
                    rebounded->setDamage(bullet->damage() * 1.2);        // Increase damage by 20%
                    rebounded->setIgnoreArmor(true);
                    rebounded->setIsRebounded(true);
                    addBullet(rebounded);
                    bullet->setHit();
                    break;
                }
                else {

                    int finalDamage = static_cast<int>(bullet->damage() * m_player->damageMultiplier());
                    enemy->takeDamage(finalDamage);
                    // Call onHit to trigger skills
                    enemy->onHit(m_player, this);
                    bullet->setHit();
                    if (m_player->hasBurnEffect()) {
                        enemy->applyBurn(10, 3.0);
                    }
                    if (enemy->type() == EnemyType::BerryBomb && !enemy->isDead()) {
                        enemy->explode(this);
                        bullet->setHit();
                        break; // Bullet disappears after explosion and stops further detection
                    }
                    break;
                }
            }
        }
    }
}
void GameManager::checkPlayerEnemyCollision()
{
    if (m_player->isDead() || m_gameOver) return;

    // Invincibility frame check
    static qreal lastDamageTime = 0;
    if (m_gameTime - lastDamageTime < 0.5) return;
    lastDamageTime = m_gameTime;

    qreal playerX = m_playerX;
    qreal playerY = m_playerY;
    qreal playerRadius = 20.0;

    for (Enemy* enemy : m_enemies) {
        if (enemy->isDead()) continue;

        qreal enemyX = enemy->x();
        qreal enemyY = enemy->y();
        qreal enemyRadius = enemy->size() / 2.0;

        qreal dx = playerX - enemyX;
        qreal dy = playerY - enemyY;
        qreal dist = std::hypot(dx, dy);

        if (dist < playerRadius + enemyRadius) {
            // Player takes no collision damage while Bark Shield is blocking
            if (enemy->type() == EnemyType::BarkShield && enemy->isBlocking()) {
                break;
            }
            // Berry Bomb: Trigger explosion (does not deal normal damage)
            if (enemy->type() == EnemyType::BerryBomb) {
                enemy->explode(this);
                break; // After explosion, enemy disappears and this collision ends
            }
            // Other enemies (including small berries) deal normal damage
            else {
                m_player->takeDamage(enemy->damage());
                qDebug() << "Player hit by enemy type" << (int)enemy->type() << "HP:" << m_player->hp();
                break; // Process only one enemy per collision
            }
        }
    }
}

void GameManager::checkPlayerRootCollision()
{
    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        Bullet* bullet = m_bullets[i];
        RootProjectile* rootProj = qobject_cast<RootProjectile*>(bullet);
        if (!rootProj) continue;

        qreal dx = m_playerX - rootProj->x();
        qreal dy = m_playerY - rootProj->y();
        qreal dist = std::hypot(dx, dy);
        if (dist < 40.0) {
            if (!m_player->isRooted()) {
                m_player->setRooted(true);
                // Add status display
                m_player->addEffect("🌿 Stun", rootProj->rootDuration());
                // Deal bullet damage
                m_player->takeDamage(rootProj->damage());
                QTimer::singleShot(static_cast<int>(rootProj->rootDuration() * 1000), this, [this]() {
                    m_player->setRooted(false);
                    });
            }
            delete rootProj;
            m_bullets.removeAt(i);
            qDebug() << "Player rooted!";
            break;
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
        }
    }
    emit expBallsChanged();
}

void GameManager::cleanupDeadObjects()
{
    for (int i = m_enemies.size() - 1; i >= 0; --i) {
        Enemy* enemy = m_enemies[i];
        if (enemy->isDead()) {
            if (enemy->type() == EnemyType::RaccoonBoss) {
                enemy->stopBossLogic();   // Stop recursive chain
            }
            enemy->disconnect();
            delete enemy;
            m_enemies.removeAt(i);
        }
    }
    emit enemiesChanged();

    for (int i = m_bullets.size() - 1; i >= 0; --i) {
        if (m_bullets[i]->isBeingDeleted()) {
            delete m_bullets[i];
            m_bullets.removeAt(i);
        }
    }
    emit bulletsChanged();

    for (int i = m_explosions.size() - 1; i >= 0; --i) {
        if (m_explosions[i]->isFinished()) {
            delete m_explosions[i];
            m_explosions.removeAt(i);
        }
    }
    emit explosionsChanged();
}

void GameManager::onEnemyDied(Enemy* enemy)
{
    if (!enemy) return;

    // Drop experience orb
    ExpBall* ball = new ExpBall(this);
    ball->setX(enemy->x());
    ball->setY(enemy->y());
    ball->setExpValue(enemy->expReward());
    addExpBall(ball);
}

void GameManager::refreshEnemies()
{
    emit enemiesChanged();
}