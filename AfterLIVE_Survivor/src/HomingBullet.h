#pragma once
#include "Bullet.h"

class Enemy;
class GameManager;

class HomingBullet : public Bullet
{
    Q_OBJECT
public:
    explicit HomingBullet(QObject* parent = nullptr);

    void setTargetEnemy(Enemy* enemy) { m_targetEnemy = enemy; }
    void setGameManager(QObject* gameManager) { m_gameManager = gameManager; }
    void setExplosionRadius(qreal radius) { m_explosionRadius = radius; }
    void setExplosionDamage(int damage) { m_explosionDamage = damage; }

    void update(qreal deltaTime) override;

protected:
    void triggerExplosion();

private:
    Enemy* m_targetEnemy = nullptr;
    QObject* m_gameManager = nullptr;
    qreal m_explosionRadius = 120;  
    int m_explosionDamage = 45;       
    bool m_hasExploded = false;
};