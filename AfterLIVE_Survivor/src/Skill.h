#pragma once
#include <QObject>

class Enemy;
class Player;
class GameManager;

class Skill : public QObject
{
    Q_OBJECT
public:
    explicit Skill(QObject* parent = nullptr);
    virtual ~Skill();

    virtual void activate(Enemy* caster, Player* player, GameManager* gm) = 0;
    virtual QString description() const = 0;
};