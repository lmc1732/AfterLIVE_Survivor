#pragma once
#include "Skill.h"

class RootSkill : public Skill
{
    Q_OBJECT
public:
    explicit RootSkill(QObject* parent = nullptr);

    void activate(Enemy* caster, Player* player, GameManager* gm) override;
    QString description() const override;

private:
    qreal m_cooldown = 3.0;
    qreal m_lastTriggerTime = -3.0;
};