#include "HomingBullet.h"
#include "GameManager.h"
#include "Explosion.h"
#include "Enemy.h"
#include <cmath>

HomingBullet::HomingBullet(QObject* parent) : Bullet(parent)
{
    m_maxLifeTime = 2.0;
}

void HomingBullet::update(qreal deltaTime)
{
    if (m_hasExploded) {
        return;
    }

    // Update target enemy
    if (!m_targetEnemy || (m_gameManager && !static_cast<GameManager*>(m_gameManager)->enemies().contains(m_targetEnemy))) {
        if (m_gameManager) {
            GameManager* gm = static_cast<GameManager*>(m_gameManager);
            const auto& enemies = gm->enemies();
            qreal minDist2 = 1e9;
            Enemy* nearest = nullptr;
            for (auto obj : enemies) {
                Enemy* e = qobject_cast<Enemy*>(obj);
                if (e) {
                    qreal dx = e->x() - x();
                    qreal dy = e->y() - y();
                    qreal dist2 = dx * dx + dy * dy;
                    if (dist2 < minDist2) {
                        minDist2 = dist2;
                        nearest = e;
                    }
                }
            }
            m_targetEnemy = nearest;
        }
    }

    // Track target
    if (m_targetEnemy) {
        qreal dx = m_targetEnemy->x() - x();
        qreal dy = m_targetEnemy->y() - y();
        qreal dist = std::hypot(dx, dy);

        // If the distance is less than the explosion radius, trigger the explosion
        if (dist <= m_explosionRadius) {
            triggerExplosion();
            return;
        }

        if (dist > 0.01) {
            dx /= dist;
            dy /= dist;
            setVelocity(dx, dy);
        }
    }

    Bullet::update(deltaTime);
}

void HomingBullet::triggerExplosion()
{
    if (m_hasExploded) return;
    m_hasExploded = true;

    GameManager* gm = qobject_cast<GameManager*>(m_gameManager);
    if (!gm) return;

    // Record killed enemies (to determine whether to play fireworks)
    Enemy* killedEnemy = nullptr;

    // Deal massive damage to all enemies within the explosion radius
    const auto& enemies = gm->enemies();
    for (auto obj : enemies) {
        Enemy* e = qobject_cast<Enemy*>(obj);
        if (e) {
            qreal dx = e->x() - x();
            qreal dy = e->y() - y();
            qreal dist = std::hypot(dx, dy);

            if (dist <= m_explosionRadius) {
                int oldHp = e->hp();
                // Damage falls off with distance, but center damage is extremely high
                int damage = static_cast<int>(m_explosionDamage * (1.0 - dist / m_explosionRadius * 0.7));
                if (damage < 5) damage = 5;  // Min damage: 5
                int newHp = oldHp - damage;
                e->setHp(qMax(0, newHp));

                // Key: Trigger enemy skills (Spore Mist, Stun, etc.)
                e->onHit(gm->player(), gm);

                // If this enemy is killed, record it
                if (oldHp > 0 && newHp <= 0) {
                    killedEnemy = e;
                }
            }
        }
    }

    // Play firework effect only on enemy kill
    if (killedEnemy) {
        Explosion* explosion = new Explosion();
        explosion->setCenter(killedEnemy->x(), killedEnemy->y());
        gm->addExplosion(explosion);
    }

    m_deleted = true;
    deleteLater();
}