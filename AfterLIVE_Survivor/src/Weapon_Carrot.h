#pragma once
#include "Weapon.h"

class Carrot : public Weapon
{
    Q_OBJECT
public:
    explicit Carrot(QObject* parent = nullptr);

protected:
    void createBullet(const QPointF& playerPos, const QPointF& targetPos,
        QList<Bullet*>& bullets) override;
};