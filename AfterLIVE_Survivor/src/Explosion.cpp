#include "Explosion.h"
#include <QRandomGenerator>
#include <cmath>

// ---------- Particle ----------
Particle::Particle(QObject* parent) : QObject(parent)
{
}

void Particle::setX(qreal x)
{
    if (qFuzzyCompare(m_x, x)) return;
    m_x = x;
    emit xChanged();
}

void Particle::setY(qreal y)
{
    if (qFuzzyCompare(m_y, y)) return;
    m_y = y;
    emit yChanged();
}

void Particle::setVx(qreal vx)
{
    if (qFuzzyCompare(m_vx, vx)) return;
    m_vx = vx;
    emit vxChanged();
}

void Particle::setVy(qreal vy)
{
    if (qFuzzyCompare(m_vy, vy)) return;
    m_vy = vy;
    emit vyChanged();
}

void Particle::setColorR(int r)
{
    if (m_colorR == r) return;
    m_colorR = r;
    emit colorRChanged();
}

void Particle::setColorG(int g)
{
    if (m_colorG == g) return;
    m_colorG = g;
    emit colorGChanged();
}

void Particle::setColorB(int b)
{
    if (m_colorB == b) return;
    m_colorB = b;
    emit colorBChanged();
}

void Particle::setSize(qreal size)
{
    if (qFuzzyCompare(m_size, size)) return;
    m_size = size;
    emit sizeChanged();
}

void Particle::setLife(qreal life)
{
    if (qFuzzyCompare(m_life, life)) return;
    m_life = life;
    emit lifeChanged();
}

void Particle::update(qreal deltaTime)
{
    if (m_life <= 0) return;
    setX(m_x + m_vx * deltaTime);
    setY(m_y + m_vy * deltaTime);
    m_life -= deltaTime;
    if (m_life <= 0) {
        m_life = 0;
        emit lifeChanged();
    }
}

// ---------- Explosion ----------
Explosion::Explosion(QObject* parent) : QObject(parent)
{
}

Explosion::~Explosion()
{
    for (Particle* p : m_particles) {
        delete p;
    }
}

void Explosion::setCenter(qreal x, qreal y)
{
    m_centerX = x;
    m_centerY = y;
    generateParticles();
    m_initialized = true;
}

void Explosion::generateParticles()
{
    const int particleCount = 12;
    for (int i = 0; i < particleCount; ++i) {
        Particle* p = new Particle(this);
        // Random direction, low speed, small area
        qreal angle = QRandomGenerator::global()->bounded(0, 360) * M_PI / 180.0;
        qreal speed = QRandomGenerator::global()->bounded(30, 80);  // Lower speed, small spread
        p->setVy(std::sin(angle) * speed);
        p->setX(m_centerX);
        p->setY(m_centerY);
        // Random color (warm tones / golden)
        int r = QRandomGenerator::global()->bounded(220, 255);
        int g = QRandomGenerator::global()->bounded(100, 200);
        int b = QRandomGenerator::global()->bounded(30, 100);
        p->setColorR(r);
        p->setColorG(g);
        p->setColorB(b);
        // Random size (slightly smaller)
        p->setSize(QRandomGenerator::global()->bounded(2, 5));
        // Random lifetime (short)
        p->setLife(QRandomGenerator::global()->bounded(200, 400) / 1000.0);
        m_particles.append(p);
    }
    emit particlesChanged();
}

void Explosion::update(qreal deltaTime)
{
    if (!m_initialized) return;
    bool anyAlive = false;
    for (Particle* p : m_particles) {
        p->update(deltaTime);
        if (!p->isDead()) anyAlive = true;
    }
    if (!anyAlive) {
        m_initialized = false;
    }
    emit particlesChanged();
}

bool Explosion::isFinished() const
{
    return !m_initialized;
}

QList<QObject*> Explosion::particles() const
{
    QList<QObject*> list;
    for (Particle* p : m_particles) {
        list.append(p);
    }
    return list;
}