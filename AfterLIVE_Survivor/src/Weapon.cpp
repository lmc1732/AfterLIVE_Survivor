#include "Weapon.h"
#include "Enemy.h"
#include "Bullet.h"

Weapon::Weapon(QObject* parent) : QObject(parent)
{
}

Weapon::~Weapon()
{
}

void Weapon::setDamage(int damage)
{
    if (m_damage == damage) return;
    m_damage = damage;
    emit damageChanged();
}

void Weapon::setAttackSpeed(int attackSpeed)
{
    if (m_attackSpeed == attackSpeed) return;
    m_attackSpeed = attackSpeed;
    emit attackSpeedChanged();
}

void Weapon::setLevel(int level)
{
    if (m_level == level) return;
    m_level = level;
    emit levelChanged();
}

void Weapon::attack(const QPointF& playerPos, const QList<Enemy*>& enemies,
    QList<Bullet*>& bullets, qreal deltaTime)
{
    if (enemies.isEmpty()) return;

    m_cooldownTimer += deltaTime;
    qreal attackInterval = 1.0 / m_attackSpeed;

    while (m_cooldownTimer >= attackInterval) {
        m_cooldownTimer -= attackInterval;

        // Find nearest enemy
        Enemy* nearest = nullptr;
        qreal minDist2 = 1e9;
        for (Enemy* e : enemies) {
            QPointF enemyPos(e->x(), e->y());
            qreal dx = enemyPos.x() - playerPos.x();
            qreal dy = enemyPos.y() - playerPos.y();
            qreal dist2 = dx * dx + dy * dy;
            if (dist2 < minDist2) {
                minDist2 = dist2;
                nearest = e;
            }
        }
        if (nearest) {
            QPointF targetPos(nearest->x(), nearest->y());
            createBullet(playerPos, targetPos, bullets);
        }
    }
}