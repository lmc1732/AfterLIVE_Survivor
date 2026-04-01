#include "StatUpgrade.h"
#include "Player.h"

StatUpgrade::StatUpgrade(StatType type, int value, const QString& name, const QString& icon, QObject* parent)
    : UpgradeItem(parent), m_type(type), m_value(value), m_name(name), m_icon(icon)
{
}

QString StatUpgrade::name() const
{
    return m_name;
}

QString StatUpgrade::description() const
{
    switch (m_type) {
    case MaxHp:     return QString("Max Health +%1").arg(m_value);
    case HpRegen:   return QString("Recovery +%1/s").arg(m_value);
    case Armor:     return QString("Armor +%1").arg(m_value);
    case MoveSpeed: return QString("Move Speed +%1").arg(m_value);
    case Power:     return QString("Might +%1").arg(m_value);
    case Area:      return QString("Area +%1").arg(m_value);
    case AttackSpeed: return QString("Attack Speed +%1").arg(m_value / 10.0); // Assume integer input, display as decimal
    }
    return "";
}

QString StatUpgrade::iconPath() const
{
    return m_icon;
}

void StatUpgrade::apply(Player* player)
{
    switch (m_type) {
    case MaxHp:
        player->setMaxHp(player->maxHp() + m_value);
        player->setHp(player->hp() + m_value); // Also restore the same amount of HP
        break;
    case HpRegen:
        player->setHpRegen(player->hpRegen() + m_value);
        break;
    case Armor:
        player->setArmor(player->armor() + m_value);
        break;
    case MoveSpeed:
        player->setMoveSpeed(player->moveSpeed() + m_value);
        break;
    case Power:
        player->setPower(player->power() + m_value);
        break;
    case Area:
        player->setArea(player->area() + m_value);
        break;
    case AttackSpeed:
        player->setAttackSpeed(player->attackSpeed() + m_value / 10.0); // Assume integer value represents 0.1x multiplier
        break;
    }
}