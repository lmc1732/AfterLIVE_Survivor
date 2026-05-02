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
        enemy->setMaxHp(25);
        enemy->setHp(25);
        enemy->setSpeed(100);
        enemy->setDamage(10);
        enemy->setExpReward(15);
        enemy->setSize(30);
        enemy->setSkill(new SporeMistSkill(enemy));
        break;
    case EnemyType::BranchGunner:
        enemy->setMaxHp(35);
        enemy->setHp(35);
        enemy->setSpeed(80);
        enemy->setDamage(15);
        enemy->setExpReward(20);
        enemy->setSize(35);
        enemy->setSkill(new RootSkill(enemy));
        break;
    case EnemyType::BerryBomb:
        enemy->setMaxHp(20);
        enemy->setHp(20);
        enemy->setSpeed(50);
        enemy->setDamage(12);
        enemy->setExpReward(18);
        enemy->setSize(30);
        break;
    case EnemyType::BarkShield:
        enemy->setMaxHp(70);
        enemy->setHp(70);
        enemy->setSpeed(50);
        enemy->setDamage(8);
        enemy->setExpReward(25);
        enemy->setSize(40);
        break;
    case EnemyType::RaccoonBoss:
        enemy->setMaxHp(250);
        enemy->setHp(250);
        enemy->setSpeed(120);
        enemy->setDamage(35);
        enemy->setExpReward(100);
        enemy->setSize(50);
        break;
    case EnemyType::SmallBerry:
        enemy->setMaxHp(8);
        enemy->setHp(8);
        enemy->setSpeed(150);
        enemy->setDamage(4);
        enemy->setExpReward(5);
        enemy->setSize(15);
        break;
    }
    enemy->setSkill(nullptr);
    return enemy;
}