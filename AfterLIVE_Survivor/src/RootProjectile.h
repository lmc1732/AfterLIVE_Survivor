#pragma once
#include "Bullet.h"

class RootProjectile : public Bullet
{
    Q_OBJECT
        Q_PROPERTY(bool isRootProjectile READ isRootProjectile CONSTANT)

public:
    explicit RootProjectile(QObject* parent = nullptr);
    void setRootDuration(qreal duration) { m_rootDuration = duration; }
    qreal rootDuration() const { return m_rootDuration; }
    bool isRootProjectile() const { return true; }

private:
    qreal m_rootDuration = 1.5;
};