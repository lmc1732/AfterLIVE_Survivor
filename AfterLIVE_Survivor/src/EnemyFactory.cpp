#include "EnemyFactory.h"
#include "Enemy.h"
#include "SporeMistSkill.h"
#include "RootSkill.h"
#include <QDebug>

Enemy* EnemyFactory::createEnemy(EnemyType type, QObject* parent)
{
    Enemy* enemy = new Enemy(parent);
    enemy->setType(type);

    switch (type) {
    case EnemyType::MossBall:
        enemy->setMaxHp(30);
        enemy->setHp(30);
        enemy->setSpeed(100);
        enemy->setDamage(12);
        enemy->setExpReward(15);
        enemy->setSize(30);
        enemy->setSkill(new SporeMistSkill(enemy));
        break;
    case EnemyType::BranchGunner:
        enemy->setMaxHp(40);
        enemy->setHp(40);
        enemy->setSpeed(80);
        enemy->setDamage(18);
        enemy->setExpReward(20);
        enemy->setSize(35);
        enemy->setSkill(new RootSkill(enemy));
        break;
    case EnemyType::BerryBomb:
        enemy->setMaxHp(25);
        enemy->setHp(25);
        enemy->setSpeed(50);
        enemy->setDamage(15);
        enemy->setExpReward(18);
        enemy->setSize(30);
        break;
    case EnemyType::BarkShield:
        enemy->setMaxHp(80);
        enemy->setHp(80);
        enemy->setSpeed(50);
        enemy->setDamage(10);
        enemy->setExpReward(25);
        enemy->setSize(40);
        break;
    case EnemyType::RaccoonBoss:
        enemy->setMaxHp(400);
        enemy->setHp(400);
        enemy->setSpeed(120);
        enemy->setDamage(40);
        enemy->setExpReward(100);
        enemy->setSize(50);
        break;
    case EnemyType::SmallBerry:
        enemy->setMaxHp(10);
        enemy->setHp(10);
        enemy->setSpeed(150);
        enemy->setDamage(5);
        enemy->setExpReward(5);
        enemy->setSize(15);
        break;
    }
    enemy->setSkill(nullptr);
    return enemy;
}