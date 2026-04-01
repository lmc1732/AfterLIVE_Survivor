#include "Weapon_Bone.h"
#include "HomingBullet.h"
#include "GameManager.h"
#include <cmath>

Bone::Bone(QObject* parent) : Weapon(parent)
{
    setDamage(45);        // Base damage per bullet greatly increased to 45
    setAttackSpeed(2.5);  // Attack speed slightly reduced to 2.5 to balance high single-shot damage
}

void Bone::createBullet(const QPointF& playerPos, const QPointF& targetPos,
    QList<Bullet*>& bullets)
{
    HomingBullet* bullet = new HomingBullet(this);
    bullet->setX(playerPos.x());
    bullet->setY(playerPos.y());

    // Calculate direction
    qreal dx = targetPos.x() - playerPos.x();
    qreal dy = targetPos.y() - playerPos.y();
    qreal len = std::hypot(dx, dy);
    if (len > 0.01) {
        dx /= len;
        dy /= len;
    }
    else {
        dx = 0; dy = 1;
    }
    bullet->setVelocity(dx, dy);
    bullet->setSpeed(500);           // Projectile speed increased to 500
    bullet->setDamage(damage());

    // Explosion radius increased to 120, explosion damage increased to 45
    bullet->setExplosionRadius(120);
    bullet->setExplosionDamage(45);

    // Pass GameManager pointer
    QObject* gm = this;
    while (gm && !qobject_cast<GameManager*>(gm)) {
        gm = gm->parent();
    }
    if (gm) {
        bullet->setGameManager(gm);
    }

    bullets.append(bullet);
}