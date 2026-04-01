#include "Enemy.h"
#include <QPointF>
#include <QtMath>
#include <QDebug>

Enemy::Enemy(QObject* parent) : QObject(parent)
{
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
    m_hp = hp;
    emit hpChanged();

    if (m_hp <= 0) {
        emit died();
    }
}

void Enemy::setSpeed(qreal speed)
{
    m_speed = speed;
}

void Enemy::setDamage(int damage)
{
    m_damage = damage;
}

void Enemy::setExpReward(int exp)
{
    m_expReward = exp;
}

void Enemy::takeDamage(int damage)
{
    int newHp = m_hp - damage;
    setHp(newHp);
}

void Enemy::update(const QPointF& playerPos, qreal deltaTime)
{
    if (isDead()) return;

    qreal dx = playerPos.x() - m_x;
    qreal dy = playerPos.y() - m_y;
    qreal len = qSqrt(dx * dx + dy * dy);
    if (len > 0.01) {
        dx /= len;
        dy /= len;
        qreal currentSpeed = m_speed * m_speedMultiplier;
        setX(m_x + dx * currentSpeed * deltaTime);
        setY(m_y + dy * currentSpeed * deltaTime);
    }
}

void Enemy::applySlow(qreal factor, qreal duration)
{
    m_speedMultiplier = factor;
    m_slowTimer = duration;
}

void Enemy::applyBurn(int damagePerSecond, qreal duration)
{
    m_burnDamagePerSecond = damagePerSecond;
    m_burnTimer = duration;
}

void Enemy::updateStatusEffects(qreal deltaTime)
{  
    if (m_slowTimer > 0) {
        m_slowTimer -= deltaTime;
        if (m_slowTimer <= 0) {
            m_speedMultiplier = 1.0;
        }
    }

    if (m_burnTimer > 0) {
        m_burnTimer -= deltaTime;
        if (m_burnTimer <= 0) {
            m_burnDamagePerSecond = 0;
        }
        else {
            int damage = static_cast<int>(m_burnDamagePerSecond * deltaTime);
            if (damage > 0) {
                takeDamage(damage);
            }
        }
    }
}