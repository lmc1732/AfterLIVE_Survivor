#pragma once
#include "Enemy.h"
#include "EnemyType.h"

class EnemyFactory
{
public:
    static Enemy* createEnemy(EnemyType type, QObject* parent = nullptr);
};