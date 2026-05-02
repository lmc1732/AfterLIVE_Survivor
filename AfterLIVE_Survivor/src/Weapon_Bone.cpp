#include "Weapon_Bone.h"
#include "HomingBullet.h"
#include "GameManager.h"
#include <cmath>

Bone::Bone(QObject* parent) : Weapon(parent)
{
    setDamage(45);        
    setAttackSpeed(2.5);  
}

void Bone::createBullet(const QPointF& playerPos, const QPointF& targetPos,
    QList<Bullet*>& bullets)
{
    HomingBullet* bullet = new HomingBullet(this);
    bullet->setX(playerPos.x());
    bullet->setY(playerPos.y());

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
    bullet->setSpeed(500);
    bullet->setDamage(damage());

    bullet->setExplosionRadius(80);
    bullet->setExplosionDamage(35);

    // 传递 GameManager 指针
    QObject* gm = this;
    while (gm && !qobject_cast<GameManager*>(gm)) {
        gm = gm->parent();
    }
    if (gm) {
        bullet->setGameManager(gm);
    }

    bullet->setIsPlayerBullet(true);
    qreal angleRad = std::atan2(dy, dx);
    bullet->setAngle(angleRad * 180 / M_PI);

    bullets.append(bullet);
}