#include "RootSkill.h"
#include "Enemy.h"
#include "Player.h"
#include "GameManager.h"
#include "RootProjectile.h"
#include <QDeBug>
#include <cmath>

RootSkill::RootSkill(QObject* parent) : Skill(parent) {}

void RootSkill::activate(Enemy* caster, Player* player, GameManager* gm)
{
    qDebug() << "RootProjectile created at:" << caster->x() << caster->y();
    if (!caster || !gm) return;
    if (caster->isDead()) return;

    qreal currentTime = gm->gameTime();
    if (currentTime - m_lastTriggerTime < m_cooldown) return;
    m_lastTriggerTime = currentTime;

    // Calculate direction towards player
    qreal dx = gm->playerX() - caster->x();
    qreal dy = gm->playerY() - caster->y();
    qreal len = std::hypot(dx, dy);
    if (len > 0.01) {
        dx /= len;
        dy /= len;
    }
    else {
        dx = 0; dy = 1;
    }

    // Create Stun Bullet
    RootProjectile* projectile = new RootProjectile(gm);
    projectile->setX(caster->x() + dx * 30);
    projectile->setY(caster->y() + dy * 30);
    projectile->setVelocity(dx, dy);
    projectile->setSpeed(400);
    projectile->setDamage(10);
    projectile->setRootDuration(1.5);
    projectile->setMaxLifeTime(2.5);
    gm->addBullet(projectile);
}

QString RootSkill::description() const
{
    return "Root shot";
}