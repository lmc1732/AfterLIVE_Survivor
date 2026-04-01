#pragma once
#include "UpgradeItem.h"

class SpecialUpgrade : public UpgradeItem
{
    Q_OBJECT
public:
    enum SpecialType {
        BraveSword,   // Hero Sword: Increases damage
        Shield,       // Glow Shield: Increases Armor
        SilenceBow,   // Silence Bow: Attacks slow enemies
        LoveLute,     // Love Lute: Heal on kill
        WishBranch,   // Wishing Branch: Increases Growth
        WarmHammer    // Warm Flame Hammer: Attacks inflict Burn
    };

    SpecialUpgrade(SpecialType type, const QString& name, const QString& icon, QObject* parent = nullptr);

    QString name() const override;
    QString description() const override;
    QString iconPath() const override;
    void apply(Player* player) override;

private:
    SpecialType m_type;
    QString m_name;
    QString m_icon;
};