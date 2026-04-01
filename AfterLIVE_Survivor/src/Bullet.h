#pragma once
#include <QObject>

class Bullet : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(int damage READ damage WRITE setDamage NOTIFY damageChanged)

public:
    explicit Bullet(QObject* parent = nullptr);

    qreal x() const { return m_x; }
    void setX(qreal x);

    qreal y() const { return m_y; }
    void setY(qreal y);

    int damage() const { return m_damage; }
    void setDamage(int damage);

    qreal velX() const { return m_velX; }
    qreal velY() const { return m_velY; }
    void setVelocity(qreal vx, qreal vy);

    qreal speed() const { return m_speed; }
    void setSpeed(qreal speed);

    virtual void update(qreal deltaTime);

    bool isBeingDeleted() const { return m_deleted; }
    bool hasHit() const { return m_hasHit; }
    void setHit() { m_hasHit = true; m_deleted = true; deleteLater(); }

signals:
    void xChanged();
    void yChanged();
    void damageChanged();

protected:
    qreal m_x = 0;
    qreal m_y = 0;
    int m_damage = 10;
    qreal m_velX = 0;
    qreal m_velY = 0;
    qreal m_speed = 400;
    qreal m_lifeTime = 0;
    qreal m_maxLifeTime = 3.0;
    bool m_deleted = false;
    bool m_hasHit = false;
};