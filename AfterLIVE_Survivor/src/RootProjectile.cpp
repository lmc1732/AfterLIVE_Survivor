#include "RootProjectile.h"

RootProjectile::RootProjectile(QObject* parent) : Bullet(parent)
{
    setDamage(0);
    setSpeed(250);
    setMaxLifeTime(3.0);   // Ensure long-lasting display
}