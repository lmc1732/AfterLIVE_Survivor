#pragma once
#include <QObject>

class Enemy : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(int hp READ hp WRITE setHp NOTIFY hpChanged)

public:
    explicit Enemy(QObject* parent = nullptr);

    qreal x() const { return m_x; }
    void setX(qreal x);
    qreal y() const { return m_y; }
    void setY(qreal y);
    int hp() const { return m_hp; }
    void setHp(int hp);
    qreal speed() const { return m_speed; }
    void setSpeed(qreal speed);
    int damage() const { return m_damage; }
    void setDamage(int damage);
    int expReward() const { return m_expReward; }
    void setExpReward(int exp);

    void takeDamage(int damage);
    void update(const QPointF& playerPos, qreal deltaTime);
    bool isDead() const { return m_hp <= 0; }

    void applySlow(qreal factor, qreal duration);
    void applyBurn(int damagePerSecond, qreal duration);
    void updateStatusEffects(qreal deltaTime);

signals:
    void xChanged();
    void yChanged();
    void hpChanged();
    void died();

private:
    qreal m_x = 0;
    qreal m_y = 0;
    int m_hp = 30;
    qreal m_speed = 100;
    int m_damage = 10;
    int m_expReward = 10;

    qreal m_speedMultiplier = 1.0;
    qreal m_slowTimer = 0.0;
    int m_burnDamagePerSecond = 0;
    qreal m_burnTimer = 0.0;
};