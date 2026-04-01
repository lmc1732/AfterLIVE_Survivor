#pragma once
#include "UpgradeItem.h"

class StatUpgrade : public UpgradeItem
{
    Q_OBJECT
public:
    enum StatType {
        MaxHp,      // Max Health
        HpRegen,    // Recovery
        Armor,      // Armor
        MoveSpeed,  // Move Speed
        Power,      // Might
        Area,       // Area
        AttackSpeed // Attack Speed
    };

    StatUpgrade(StatType type, int value, const QString& name, const QString& icon, QObject* parent = nullptr);

    QString name() const override;
    QString description() const override;
    QString iconPath() const override;
    void apply(Player* player) override;

private:
    StatType m_type;
    int m_value;
    QString m_name;
    QString m_icon;
};