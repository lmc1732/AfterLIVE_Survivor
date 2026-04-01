#include "Weapon_Carrot.h"
#include "Bullet.h"
#include <cmath>

Carrot::Carrot(QObject* parent) : Weapon(parent)
{
    setDamage(10);
    setAttackSpeed(1.5);  
}

void Carrot::createBullet(const QPointF& playerPos, const QPointF& targetPos,
    QList<Bullet*>& bullets)
{ 
    const int bulletCount = 5;
    const float angleSpread = 60.0f * M_PI / 180.0f;
    float startAngle = -angleSpread / 2.0f;
      
    qreal dx = targetPos.x() - playerPos.x();
    qreal dy = targetPos.y() - playerPos.y();
    qreal len = std::hypot(dx, dy);
    if (len < 0.01) {
        dx = 0; dy = 1;
        len = 1;
    }
    qreal baseAngle = std::atan2(dy, dx);

    for (int i = 0; i < bulletCount; ++i) {
        qreal angle = baseAngle + startAngle + (angleSpread / (bulletCount - 1)) * i;
        qreal vx = std::cos(angle);
        qreal vy = std::sin(angle);

        Bullet* bullet = new Bullet(this);
        bullet->setX(playerPos.x());
        bullet->setY(playerPos.y());
        bullet->setVelocity(vx, vy);
        bullet->setSpeed(400);
        bullet->setDamage(damage());

        bullets.append(bullet);
    }
}