#pragma once
#include "Weapon.h"

class Bone : public Weapon
{
    Q_OBJECT
public:
    explicit Bone(QObject* parent = nullptr);

protected:
    void createBullet(const QPointF& playerPos, const QPointF& targetPos,
        QList<Bullet*>& bullets) override;
};