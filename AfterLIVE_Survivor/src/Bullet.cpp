#include "Bullet.h"
#include <QtMath>

Bullet::Bullet(QObject* parent) : QObject(parent)
{
}

void Bullet::setX(qreal x)
{
    if (qFuzzyCompare(m_x, x)) return;
    m_x = x;
    emit xChanged();
}

void Bullet::setY(qreal y)
{
    if (qFuzzyCompare(m_y, y)) return;
    m_y = y;
    emit yChanged();
}

void Bullet::setDamage(int damage)
{
    if (m_damage == damage) return;
    m_damage = damage;
    emit damageChanged();
}

void Bullet::setVelocity(qreal vx, qreal vy)
{
    m_velX = vx;
    m_velY = vy;
}

void Bullet::setIsPlayerBullet(bool value)
{
    if (m_isPlayerBullet == value)
        return;                     // Value unchanged, do not emit signal
    m_isPlayerBullet = value;
    emit isPlayerBulletChanged();   // Emit signal
}

void Bullet::setAngle(qreal angle)
{
    if (qFuzzyCompare(m_angle, angle))
        return;
    m_angle = angle;
    emit angleChanged();
}

void Bullet::setSpeed(qreal speed)
{
    m_speed = speed;
}
void Bullet::update(qreal deltaTime)
{
    if (m_hasHit) return;

    setX(m_x + m_velX * m_speed * deltaTime);
    setY(m_y + m_velY * m_speed * deltaTime);

    m_lifeTime += deltaTime;
    if (m_lifeTime >= m_maxLifeTime) {
        m_deleted = true;
        deleteLater();
    }
}