#include "SpecialUpgrade.h"
#include "Player.h"

SpecialUpgrade::SpecialUpgrade(SpecialType type, const QString& name, const QString& icon, QObject* parent)
    : UpgradeItem(parent), m_type(type), m_name(name), m_icon(icon)
{
}

QString SpecialUpgrade::name() const
{
    return m_name;
}

QString SpecialUpgrade::description() const
{
    switch (m_type) {
    case BraveSword:   return "Hero Sword: +20% might";
    case Shield:       return "Glow Shield: +10 Armor";
    case SilenceBow:   return "Silence Bow: Attacks slow enemies by 30% for 2 seconds";
    case LoveLute:     return "Love Lute: Heal 5 HP on enemy kill";
    case WishBranch:   return "Wishing Branch: +50% Growth";
    case WarmHammer:   return "Warm Flame Hammer: Attacks inflict Burn, dealing 10 damage per second for 3 seconds";
    }
    return "";
}

QString SpecialUpgrade::iconPath() const
{
    return m_icon;
}

void SpecialUpgrade::apply(Player* player)
{
    // Only sets a flag here; actual effects are implemented in collision detection, attack, and other modules
    switch (m_type) {
    case BraveSword:
        player->setDamageMultiplier(player->damageMultiplier() + 0.2);
        break;
    case Shield:
        player->setArmor(player->armor() + 10);
        break;
    case SilenceBow:
        player->setHasSlowEffect(true);
        break;
    case LoveLute:
        player->setHasHealOnKill(true);
        break;
    case WishBranch:
        player->setExpMultiplier(player->expMultiplier() + 0.5);
        break;
    case WarmHammer:
        player->setHasBurnEffect(true);
        break;
    }
}