#pragma once
#include <QObject>
#include <QPointF>
#include <QList>

class Enemy;
class Bullet;

class Weapon : public QObject
{
    Q_OBJECT
        Q_PROPERTY(int damage READ damage WRITE setDamage NOTIFY damageChanged)
        Q_PROPERTY(int attackSpeed READ attackSpeed WRITE setAttackSpeed NOTIFY attackSpeedChanged)
        Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)

public:
    explicit Weapon(QObject* parent = nullptr);
    virtual ~Weapon();

    int damage() const { return m_damage; }
    void setDamage(int damage);

    int attackSpeed() const { return m_attackSpeed; }
    void setAttackSpeed(int attackSpeed);

    int level() const { return m_level; }
    void setLevel(int level);

    virtual void attack(const QPointF& playerPos, const QList<Enemy*>& enemies,
        QList<Bullet*>& bullets, qreal deltaTime);

signals:
    void damageChanged();
    void attackSpeedChanged();
    void levelChanged();

protected:
    virtual void createBullet(const QPointF& playerPos, const QPointF& targetPos,
        QList<Bullet*>& bullets) = 0;

private:
    int m_damage = 10;
    int m_attackSpeed = 2;
    int m_level = 1;
    qreal m_cooldownTimer = 0;
};