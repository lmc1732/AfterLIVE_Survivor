#include "Weapon_Bone.h"
#include "HomingBullet.h"
#include "GameManager.h"
#include <cmath>

Bone::Bone(QObject* parent) : Weapon(parent)
{
    setDamage(45);        // 单颗子弹基础伤害大幅提升到 45
    setAttackSpeed(2.5);  // 攻速略微降低到 2.5，平衡单发高伤害
}

void Bone::createBullet(const QPointF& playerPos, const QPointF& targetPos,
    QList<Bullet*>& bullets)
{
    HomingBullet* bullet = new HomingBullet(this);
    bullet->setX(playerPos.x());
    bullet->setY(playerPos.y());

    // 计算方向
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
    bullet->setSpeed(500);           // 子弹速度提升到 500
    bullet->setDamage(damage());

    // 爆炸半径增大到 120，爆炸伤害提升到 45
    bullet->setExplosionRadius(120);
    bullet->setExplosionDamage(45);

    // 传递 GameManager 指针
    QObject* gm = this;
    while (gm && !qobject_cast<GameManager*>(gm)) {
        gm = gm->parent();
    }
    if (gm) {
        bullet->setGameManager(gm);
    }

    bullets.append(bullet);
}