#pragma once
#include "Skill.h"

class SporeMistSkill : public Skill
{
    Q_OBJECT
public:
    explicit SporeMistSkill(QObject* parent = nullptr);

    void activate(Enemy* caster, Player* player, GameManager* gm) override;
    QString description() const override;

private:
    qreal m_cooldown = 5.0;           // Cooldown (seconds)
    qreal m_lastTriggerTime = -5.0;
};