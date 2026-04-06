#include "Enemy.h"
#include "Bullet.h"
#include "Player.h"
#include "GameManager.h"
#include "Skill.h"
#include "SporeMistSkill.h"
#include "RootSkill.h"
#include "EnemyFactory.h"
#include <QPointF>
#include <QtMath>
#include <QRandomGenerator>
#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <cmath>

// ========== Constructor & Destructor ==========
Enemy::Enemy(QObject* parent) : QObject(parent)
{
    // Original Initialization
    m_blockTimer = nullptr;
    m_cooldownTimer = nullptr;
    m_initialDirectionSet = false;
}

Enemy::~Enemy()
{
    stopBossLogic();  // Ensure the recursive chain terminates
}

void Enemy::setX(qreal x)
{
    if (qFuzzyCompare(m_x, x)) return;
    m_x = x;
    emit xChanged();
}

void Enemy::setY(qreal y)
{
    if (qFuzzyCompare(m_y, y)) return;
    m_y = y;
    emit yChanged();
}

void Enemy::setHp(int hp)
{
    hp = qMax(0, hp);
    if (m_hp == hp) return;
    int oldHp = m_hp;
    m_hp = hp;
    emit hpChanged();

    // Phase Switch
    if (m_type == EnemyType::RaccoonBoss && m_bossPhase == 1 && m_hp <= m_maxHp / 2) {
        qDebug() << "=== Boss phase switched to 2! (HP:" << m_hp << "/" << m_maxHp << ") ===";
        m_bossPhase = 2;
    }

    // On death, emit signal passing this
    if (m_hp == 0 && oldHp > 0) {
        stopBossLogic();
        emit died(this);
    }
}

void Enemy::setMaxHp(int maxHp)
{
    if (m_maxHp == maxHp) return;
    m_maxHp = maxHp;
    emit maxHpChanged();
}

void Enemy::setSpeed(qreal speed) { m_speed = speed; }
void Enemy::setType(EnemyType type)
{
    if (m_type == type) return;
    m_type = type;
    emit typeChanged();
}
void Enemy::setDamage(int damage) { m_damage = damage; }
void Enemy::setExpReward(int exp) { m_expReward = exp; }
void Enemy::setSize(qreal size) { m_size = size; }
void Enemy::takeDamage(int damage)
{
    if (m_isBlocking) return;  // Invincible while blocking
    setHp(m_hp - damage);
}

void Enemy::update(const QPointF& playerPos, qreal deltaTime)
{
    if (isDead()) return;

    // Correct invalid position
    if (std::isnan(m_x) || std::isnan(m_y)) {
        setX(400);
        setY(300);
    }
    // Boundary Limit
    if (m_x < 0) m_x = 0;
    if (m_x > 800 - m_size) m_x = 800 - m_size;
    if (m_y < 0) m_y = 0;
    if (m_y > 600 - m_size) m_y = 600 - m_size;
    setX(m_x);
    setY(m_y);

    if (m_isBlocking) {
        // Slowly move toward player while blocking (half speed)
        QPointF dir = playerPos - QPointF(m_x, m_y);
        qreal len = std::hypot(dir.x(), dir.y());
        if (len > 0.01) {
            dir /= len;
            setX(m_x + dir.x() * (m_speed * 0.5) * deltaTime);
            setY(m_y + dir.y() * (m_speed * 0.5) * deltaTime);
        }
        return;
    }

    // Active Skill Timer
    static qreal skillTimer = 0;
    skillTimer += deltaTime;
    if (m_skill && skillTimer >= 4.0) {
        skillTimer = 0;
        m_skill->activate(this, nullptr, nullptr);
    }

    if (m_type == EnemyType::SmallBerry) {
        // Move
        setX(m_x + m_dirX * m_speed * deltaTime);
        setY(m_y + m_dirY * m_speed * deltaTime);
        // Boundary bounce (assuming window size 800x600)
        if (m_x < 0 || m_x > 800) {
            m_dirX = -m_dirX;
            setX(qBound(0.0, m_x, 800.0));
        }
        if (m_y < 0 || m_y > 600) {
            m_dirY = -m_dirY;
            setY(qBound(0.0, m_y, 600.0));
        }
        return;
    }

    // Boss Move
    if (m_type == EnemyType::RaccoonBoss && m_isMoving && m_gameManager) {
        qreal speed = (m_bossPhase == 1) ? m_baseSpeed : m_baseSpeed * 1.5;
        qreal newX = m_x + m_moveDirection.x() * speed * deltaTime;
        qreal newY = m_y + m_moveDirection.y() * speed * deltaTime;

        const qreal leftBound = 0;
        const qreal rightBound = 800 - m_size;
        const qreal topBound = 0;
        const qreal bottomBound = 600 - m_size;
        const qreal edgeMargin = 50.0; // Boundary warning distance
                                       // Turn if below this value

        bool adjusted = false;

        // If near left boundary and moving left, or near right boundary and moving right, reverse X direction
        if ((newX < leftBound + edgeMargin && m_moveDirection.x() < 0) ||
            (newX > rightBound - edgeMargin && m_moveDirection.x() > 0)) {
            m_moveDirection.setX(-m_moveDirection.x());
            adjusted = true;
        }
        // Similarly handle Y direction
        if ((newY < topBound + edgeMargin && m_moveDirection.y() < 0) ||
            (newY > bottomBound - edgeMargin && m_moveDirection.y() > 0)) {
            m_moveDirection.setY(-m_moveDirection.y());
            adjusted = true;
        }

        // If direction is adjusted, recalculate position to avoid getting stuck inside the wall
        if (adjusted) {
            newX = m_x + m_moveDirection.x() * speed * deltaTime;
            newY = m_y + m_moveDirection.y() * speed * deltaTime;
        }

        // Finally clamp to within boundaries
        newX = qBound(leftBound, newX, rightBound);
        newY = qBound(topBound, newY, bottomBound);
        setX(newX);
        setY(newY);

        // Corner anti-stuck: If speed is below threshold for an extended period (e.g., 3 seconds), force a random direction change
        static int stuckCounter = 0;
        qreal moveDist = std::hypot(newX - m_x, newY - m_y);
        if (moveDist < 1.0) {
            stuckCounter++;
            if (stuckCounter > 60) { // Approximately 1 second (assuming 60fps)
                qreal angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
                m_moveDirection = QPointF(std::cos(angle), std::sin(angle));
                stuckCounter = 0;
                qDebug() << "Boss stuck, forced new direction";
            }
        }
        else {
            stuckCounter = 0;
        }
        return;
    }

    // Normal homing movement
    QPointF dir = playerPos - QPointF(m_x, m_y);
    qreal len = std::hypot(dir.x(), dir.y());
    if (len > 0.01) {
        dir /= len;
        setX(m_x + dir.x() * m_speed * deltaTime);
        setY(m_y + dir.y() * m_speed * deltaTime);
    }
}

void Enemy::onHit(Player* player, GameManager* gm)
{
    // On hit, trigger block if not already blocking and off cooldown
    if (m_type == EnemyType::BarkShield && !m_isBlocking && !m_isBlockCooldown) {
        startBlocking();
        return; // Other skills are not triggered while blocking
    }

    // Dynamically create skill (ensure skill exists)
    if (m_skill == nullptr) {
        if (m_type == EnemyType::MossBall) {
            m_skill = new SporeMistSkill(this);
        }
        else if (m_type == EnemyType::BranchGunner) {
            m_skill = new RootSkill(this);
        }
        else if (m_type == EnemyType::BerryBomb) {
            m_skill = new RootSkill(this);
        }
        else if (m_type == EnemyType::BarkShield) {
            m_skill = new RootSkill(this);
        }
    }

    if (m_skill && !isDead()) {
        m_skill->activate(this, player, gm);
    }
}

void Enemy::startBossLogic(GameManager* gm)
{
    qDebug() << "startBossLogic called, using recursive singleShot";
    m_gameManager = gm;
    m_baseSpeed = m_speed;
    m_isMoving = true;
    m_bossLogicActive = true;

    // Initialize movement direction (avoid zero vector)
    if (!m_initialDirectionSet) {
        qreal angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
        m_moveDirection = QPointF(std::cos(angle), std::sin(angle));
        m_initialDirectionSet = true;
        qDebug() << "Initial move direction:" << m_moveDirection;
    }

    // Start three recursive chains
    scheduleNextShoot();
    scheduleNextMove();
    scheduleNextSummon();

    qDebug() << "Recursive chains started";
}

void Enemy::stopBossLogic()
{
    qDebug() << "stopBossLogic called for" << this;
    m_bossLogicActive = false;   // Cancel all future recursive invocations
}

void Enemy::scheduleNextShoot()
{
    qDebug() << "scheduleNextShoot called, active:" << m_bossLogicActive << "dead:" << isDead();
    if (!m_bossLogicActive) return;
    if (isDead()) return;
    if (!m_gameManager) return;

    int interval = (m_bossPhase == 1) ? 2000 : 1500;
    QTimer::singleShot(interval, this, [this]() {
        qDebug() << "Shoot lambda executing, phase:" << m_bossPhase;
        if (!m_bossLogicActive || isDead() || !m_gameManager) {
            qDebug() << "Shoot lambda aborted";
            return;
        }
        onShootTimeout();
        scheduleNextShoot();
        });
}

void Enemy::scheduleNextMove()
{
    if (!m_bossLogicActive || isDead() || !m_gameManager) return;

    int delay = m_isMoving ? QRandomGenerator::global()->bounded(800, 1500)
        : QRandomGenerator::global()->bounded(300, 600);
    QTimer::singleShot(delay, this, [this]() {
        if (!m_bossLogicActive || isDead() || !m_gameManager) return;
        onMoveTimeout();
        scheduleNextMove();
        });
}

void Enemy::scheduleNextSummon()
{
    if (!m_bossLogicActive || isDead() || !m_gameManager) return;

    int interval = (m_bossPhase == 1) ? 10000 : 12000;
    QTimer::singleShot(interval, this, [this]() {
        if (!m_bossLogicActive || isDead() || !m_gameManager) return;
        onSummonTimeout();
        scheduleNextSummon();
        });
}

void Enemy::onMoveTimeout()
{
    // Always keep moving, only change direction
    qreal angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
    m_moveDirection = QPointF(std::cos(angle), std::sin(angle));
    // Recursively change direction again after a 1-2 second delay
}

void Enemy::onShootTimeout()
{
    qDebug() << "onShootTimeout ENTER, phase:" << m_bossPhase;
    if (!m_gameManager || isDead()) return;

    QPointF playerPos(m_gameManager->playerX(), m_gameManager->playerY());
    QPointF dir = playerPos - QPointF(m_x, m_y);
    qreal len = std::hypot(dir.x(), dir.y());
    if (len > 0.01) dir /= len;
    else dir = QPointF(1, 0);

    if (m_bossPhase == 1) {
        // Straight triple shot, slight angle offset
        for (int i = -1; i <= 1; ++i) {
            qreal angle = std::atan2(dir.y(), dir.x()) + i * 5 * M_PI / 180.0;
            qDebug() << "Phase 1 bullet angle (deg):" << angle * 180 / M_PI;
            qreal vx = std::cos(angle);
            qreal vy = std::sin(angle);
            Bullet* bullet = new Bullet(m_gameManager);
            bullet->setX(m_x);
            bullet->setY(m_y);
            bullet->setVelocity(vx, vy);
            bullet->setSpeed(400);
            bullet->setDamage(20);
            bullet->setIsBossBullet(true);
            m_gameManager->addBullet(bullet);
        }
    }
    else {
        // Cone 5-shot
        qreal baseAngle = std::atan2(dir.y(), dir.x());
        qreal angleStep = 60.0 * M_PI / 180.0 / 4.0;
        qreal startAngle = baseAngle - 30 * M_PI / 180.0;
        for (int i = 0; i < 5; ++i) {
            qreal angle = startAngle + angleStep * i;
            qDebug() << "Phase 2 bullet angle (deg):" << angle * 180 / M_PI;
            qreal vx = std::cos(angle);
            qreal vy = std::sin(angle);
            Bullet* bullet = new Bullet(m_gameManager);
            bullet->setX(m_x);
            bullet->setY(m_y);
            bullet->setVelocity(vx, vy);
            bullet->setSpeed(500);
            bullet->setDamage(25);
            bullet->setIsBossBullet(true);
            m_gameManager->addBullet(bullet);
        }
    }
}

void Enemy::onSummonTimeout()
{
    if (!m_gameManager || isDead()) return;

    if (m_bossPhase == 1) {
        // Summon 2 Branch Gunners
        for (int i = 0; i < 2; ++i) {
            qreal offsetX = QRandomGenerator::global()->bounded(-80, 80);
            qreal offsetY = QRandomGenerator::global()->bounded(-80, 80);
            m_gameManager->spawnEnemyAt(EnemyType::BranchGunner, m_x + offsetX, m_y + offsetY);
        }
    }
    else {
        // Spin spray 8 bullets
        for (int i = 0; i < 8; ++i) {
            qreal angle = i * 45.0 * M_PI / 180.0;
            Bullet* bullet = new Bullet(m_gameManager);
            bullet->setX(m_x);
            bullet->setY(m_y);
            bullet->setVelocity(std::cos(angle), std::sin(angle));
            bullet->setSpeed(450);
            bullet->setDamage(20);
            m_gameManager->addBullet(bullet);
        }
        // Summon 3 Berry Bombs
        for (int i = 0; i < 3; ++i) {
            qreal offsetX = QRandomGenerator::global()->bounded(-60, 60);
            qreal offsetY = QRandomGenerator::global()->bounded(-60, 60);
            m_gameManager->spawnEnemyAt(EnemyType::BerryBomb, m_x + offsetX, m_y + offsetY);
        }
    }
}

void Enemy::updateActiveSkill(qreal deltaTime, GameManager* gm)
{
    if (m_skill && !isDead()) {
        static qreal skillTimer = 0;
        skillTimer += deltaTime;
        if (skillTimer >= 4.0) {
            skillTimer = 0;
            m_skill->activate(this, nullptr, gm);
        }
    }

}

void Enemy::updateStatusEffects(qreal deltaTime)
{
    if (m_slowTimer > 0) {
        m_slowTimer -= deltaTime;
        if (m_slowTimer <= 0) m_speedMultiplier = 1.0;
    }
    if (m_burnTimer > 0) {
        m_burnTimer -= deltaTime;
        if (m_burnTimer <= 0) {
            m_burnDamagePerSecond = 0;
        }
        else {
            int damage = static_cast<int>(m_burnDamagePerSecond * deltaTime);
            if (damage > 0) takeDamage(damage);
        }
    }
}

void Enemy::applyBurn(int damagePerSecond, qreal duration)
{
    m_burnDamagePerSecond = damagePerSecond;
    m_burnTimer = duration;
}

void Enemy::applySlow(qreal factor, qreal duration)
{
    m_speedMultiplier = factor;
    m_slowTimer = duration;
}

void Enemy::explode(GameManager* gm)
{
    if (!gm) return;

    // 1. Deal area damage to player
    qreal dx = gm->playerX() - m_x;
    qreal dy = gm->playerY() - m_y;
    qreal dist = std::hypot(dx, dy);
    if (dist < m_explosionRadius) {
        // Damage increases with proximity (linear falloff, min half)
        int damage = m_explosionDamage * (1.0 - dist / m_explosionRadius);
        if (damage < 1) damage = 1;
        gm->player()->takeDamage(damage);
        qDebug() << "BerryBomb explosion dealt" << damage << "damage to player";
    }

    // 2. Spawn small berries (small berries disappear when killed)
    for (int i = 0; i < m_smallBerryCount; ++i) {
        Enemy* small = EnemyFactory::createEnemy(EnemyType::SmallBerry, gm);
        // Randomly offset positions to avoid overlapping
        qreal angle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
        qreal offsetX = std::cos(angle) * 30;
        qreal offsetY = std::sin(angle) * 30;
        small->setX(m_x + offsetX);
        small->setY(m_y + offsetY);
        // Set random movement direction for small berries (unit vector)
        qreal dirAngle = QRandomGenerator::global()->bounded(360) * M_PI / 180.0;
        small->setDirection(std::cos(dirAngle), std::sin(dirAngle));
        gm->addEnemyDirect(small);
    }

    // 3. Destroy self (mark as dead, will be removed in cleanupDeadObjects)
    setHp(0);
}

void Enemy::startBlocking()
{
    if (m_isBlocking || m_isBlockCooldown) return;

    m_isBlocking = true;
    emit isBlockingChanged();

    m_blockTimer = new QTimer(this);
    m_blockTimer->setSingleShot(true);
    connect(m_blockTimer, &QTimer::timeout, this, &Enemy::stopBlocking);
    m_blockTimer->start(1500); // Block lasts 1.5 seconds

    m_isBlockCooldown = true;
    m_cooldownTimer = new QTimer(this);
    m_cooldownTimer->setSingleShot(true);
    connect(m_cooldownTimer, &QTimer::timeout, this, [this]() {
        m_isBlockCooldown = false;
        });
    m_cooldownTimer->start(4000); // Cooldown 4 seconds
}

void Enemy::stopBlocking()
{
    if (!m_isBlocking) return;
    m_isBlocking = false;
    emit isBlockingChanged();
    if (m_blockTimer) {
        m_blockTimer->stop();
        m_blockTimer->deleteLater();
        m_blockTimer = nullptr;
    }
}