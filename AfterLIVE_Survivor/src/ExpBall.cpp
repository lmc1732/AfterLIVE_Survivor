#include "ExpBall.h"
#include <cmath>

ExpBall::ExpBall(QObject* parent) : QObject(parent)
{
}

void ExpBall::setX(qreal x)
{
    if (qFuzzyCompare(m_x, x)) return;
    m_x = x;
    emit xChanged();
}

void ExpBall::setY(qreal y)
{
    if (qFuzzyCompare(m_y, y)) return;
    m_y = y;
    emit yChanged();
}

void ExpBall::setExpValue(int expValue)
{
    if (m_expValue == expValue) return;
    m_expValue = expValue;
    emit expValueChanged();
}

void ExpBall::update(qreal deltaTime, const QPointF& playerPos, qreal attractRadius, qreal attractSpeed)
{
    // Experience orbs are not moving toward the player
    // The player must walk over the experience orbs to pick them up
    Q_UNUSED(deltaTime);
    Q_UNUSED(playerPos);
    Q_UNUSED(attractRadius);
    Q_UNUSED(attractSpeed);
}