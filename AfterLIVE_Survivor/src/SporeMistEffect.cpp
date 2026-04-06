#include "SporeMistEffect.h"

SporeMistEffect::SporeMistEffect(QObject* parent) : QObject(parent) {}

void SporeMistEffect::setPosition(qreal x, qreal y)
{
    setX(x);
    setY(y);
}

void SporeMistEffect::update(qreal deltaTime)
{
    m_lifeTime += deltaTime;
}

void SporeMistEffect::setX(qreal x)
{
    if (qFuzzyCompare(m_x, x)) return;
    m_x = x;
    emit xChanged();
}

void SporeMistEffect::setY(qreal y)
{
    if (qFuzzyCompare(m_y, y)) return;
    m_y = y;
    emit yChanged();
}