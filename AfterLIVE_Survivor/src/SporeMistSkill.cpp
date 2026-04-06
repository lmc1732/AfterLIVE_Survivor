#include "SporeMistSkill.h"
#include "Enemy.h"
#include "Player.h"
#include "GameManager.h"
#include "SporeMistEffect.h"

SporeMistSkill::SporeMistSkill(QObject* parent) : Skill(parent) {}

void SporeMistSkill::activate(Enemy* caster, Player* player, GameManager* gm)
{
    if (!caster || !player || !gm) return;
    if (caster->isDead()) return;

    qreal currentTime = gm->gameTime();
    if (currentTime - m_lastTriggerTime < m_cooldown) return;
    m_lastTriggerTime = currentTime;

    // Create fog effect
    SporeMistEffect* mist = new SporeMistEffect(gm);
    mist->setPosition(caster->x(), caster->y());
    gm->addSporeMistEffect(mist);
}

QString SporeMistSkill::description() const
{
    return QString("Release Spore Mist, increases damage taken by 20% within the area");
}