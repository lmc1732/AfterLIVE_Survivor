#pragma once
#include <QObject>

class Bullet : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(int damage READ damage WRITE setDamage NOTIFY damageChanged)
        Q_PROPERTY(qreal velX READ velX)
        Q_PROPERTY(qreal velY READ velY)
        Q_PROPERTY(bool isPlayerBullet READ isPlayerBullet WRITE setIsPlayerBullet NOTIFY isPlayerBulletChanged)
        Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
        Q_PROPERTY(bool isRebounded READ isRebounded WRITE setIsRebounded NOTIFY isReboundedChanged)
        Q_PROPERTY(bool isBossBullet READ isBossBullet WRITE setIsBossBullet NOTIFY isBossBulletChanged)

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

    bool isPlayerBullet() const { return m_isPlayerBullet; }
    void setIsPlayerBullet(bool value);
    qreal angle() const { return m_angle; }
    void setAngle(qreal angle);

    qreal speed() const { return m_speed; }
    void setSpeed(qreal speed);

    virtual void update(qreal deltaTime);

    bool isBeingDeleted() const { return m_deleted; }
    bool hasHit() const { return m_hasHit; }
    void setHit() { m_hasHit = true; m_deleted = true; deleteLater(); }

    void setMaxLifeTime(qreal life) { m_maxLifeTime = life; }

    void setIgnoreArmor(bool ignore) { m_ignoreArmor = ignore; }
    bool ignoreArmor() const { return m_ignoreArmor; }
    void setIsRebounded(bool rebounded) { m_isRebounded = rebounded; }
    bool isRebounded() const { return m_isRebounded; }
    void setIsBossBullet(bool isBoss) {
        if (m_isBossBullet == isBoss) return;
        m_isBossBullet = isBoss;
        emit isBossBulletChanged();
    }
    bool isBossBullet() const { return m_isBossBullet; }

signals:
    void xChanged();
    void yChanged();
    void damageChanged();
    void isPlayerBulletChanged();
    void angleChanged();
    void isReboundedChanged();
    void isBossBulletChanged();

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

private:
    bool m_ignoreArmor = false;
    bool m_isRebounded = false;
    bool m_isBossBullet = false;
    bool m_isPlayerBullet = false;
    qreal m_angle = 0.0;
};