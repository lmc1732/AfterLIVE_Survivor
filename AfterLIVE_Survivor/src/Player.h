#pragma once
#include <QObject>
#include "Weapon.h"

class Player : public QObject
{
    Q_OBJECT
        Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp NOTIFY maxHpChanged)
        Q_PROPERTY(int hp READ hp WRITE setHp NOTIFY hpChanged)
        Q_PROPERTY(int hpRegen READ hpRegen WRITE setHpRegen NOTIFY hpRegenChanged)
        Q_PROPERTY(int armor READ armor WRITE setArmor NOTIFY armorChanged)
        Q_PROPERTY(int moveSpeed READ moveSpeed WRITE setMoveSpeed NOTIFY moveSpeedChanged)
        Q_PROPERTY(int power READ power WRITE setPower NOTIFY powerChanged)
        Q_PROPERTY(int area READ area WRITE setArea NOTIFY areaChanged)
        Q_PROPERTY(double attackSpeed READ attackSpeed WRITE setAttackSpeed NOTIFY attackSpeedChanged)
        Q_PROPERTY(int exp READ exp WRITE setExp NOTIFY expChanged)
        Q_PROPERTY(int level READ level WRITE setLevel NOTIFY levelChanged)
        Q_PROPERTY(Weapon* weapon READ weapon CONSTANT)
        Q_PROPERTY(double expMultiplier READ expMultiplier WRITE setExpMultiplier NOTIFY expMultiplierChanged)
        Q_PROPERTY(double damageMultiplier READ damageMultiplier WRITE setDamageMultiplier NOTIFY damageMultiplierChanged)
        Q_PROPERTY(bool hasSlowEffect READ hasSlowEffect WRITE setHasSlowEffect NOTIFY hasSlowEffectChanged)
        Q_PROPERTY(bool hasHealOnKill READ hasHealOnKill WRITE setHasHealOnKill NOTIFY hasHealOnKillChanged)
        Q_PROPERTY(bool hasBurnEffect READ hasBurnEffect WRITE setHasBurnEffect NOTIFY hasBurnEffectChanged)
        Q_PROPERTY(int expToNextLevel READ expToNextLevel NOTIFY expChanged)
        Q_PROPERTY(QStringList activeEffects READ activeEffects NOTIFY activeEffectsChanged)

public:
    explicit Player(QObject* parent = nullptr);

    enum CharacterType {
        CharacterA,
        CharacterB
    };
    Q_ENUM(CharacterType)
        void setCharacterType(CharacterType type);

    // Base Stats
    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp);
    int hp() const { return m_hp; }
    void setHp(int hp);
    int hpRegen() const { return m_hpRegen; }
    void setHpRegen(int hpRegen);
    int armor() const { return m_armor; }
    void setArmor(int armor);
    int moveSpeed() const { return m_moveSpeed; }
    void setMoveSpeed(int moveSpeed);
    int power() const { return m_power; }
    void setPower(int power);
    int area() const { return m_area; }
    void setArea(int area);
    double attackSpeed() const { return m_attackSpeed; }
    void setAttackSpeed(double attackSpeed);
    int exp() const { return m_exp; }
    void setExp(int exp);
    int level() const { return m_level; }
    void setLevel(int level);

    int expToNextLevel() const;
    void addExp(int amount);
    Weapon* weapon() const { return m_weapon; }

    void takeDamage(int damage, bool ignoreArmor = false);
    bool isDead() const { return m_hp <= 0; }
    void updateInvincibility();

    // Special Effect
    double expMultiplier() const { return m_expMultiplier; }
    void setExpMultiplier(double mult);
    double damageMultiplier() const { return m_damageMultiplier; }
    void setDamageMultiplier(double mult);
    bool hasSlowEffect() const { return m_hasSlowEffect; }
    void setHasSlowEffect(bool enable);
    bool hasHealOnKill() const { return m_hasHealOnKill; }
    void setHasHealOnKill(bool enable);
    bool hasBurnEffect() const { return m_hasBurnEffect; }
    void setHasBurnEffect(bool enable);

    void addEffect(const QString& effect, qreal duration);
    void updateEffects(qreal deltaTime);
    QStringList activeEffects() const { return m_activeEffects; }
    void removeEffect(const QString& effect);
    bool hasEffect(const QString& effect) const;

    void setRooted(bool rooted);
    bool isRooted() const { return m_isRooted; }

signals:
    void maxHpChanged();
    void hpChanged();
    void hpRegenChanged();
    void armorChanged();
    void moveSpeedChanged();
    void powerChanged();
    void areaChanged();
    void attackSpeedChanged();
    void expChanged();
    void levelChanged();
    void levelUpRequested();
    void expMultiplierChanged();
    void damageMultiplierChanged();
    void hasSlowEffectChanged();
    void hasHealOnKillChanged();
    void hasBurnEffectChanged();
    void activeEffectsChanged();

private:
    int expRequiredForLevel(int level) const;

    // Base Stats
    int m_maxHp = 100;
    int m_hp = 100;
    int m_hpRegen = 0;
    int m_armor = 0;
    int m_moveSpeed = 300;
    int m_power = 10;
    int m_area = 50;
    double m_attackSpeed = 2.0;
    int m_exp = 0;
    int m_level = 1;
    Weapon* m_weapon = nullptr;
    int m_invincibilityTimer = 0;
    int m_invincibilityDuration = 60;
    bool m_isRooted = false;
    int m_savedSpeed = 0;

    // Special Effect
    double m_expMultiplier = 1.0;
    double m_damageMultiplier = 1.0;
    bool m_hasSlowEffect = false;
    bool m_hasHealOnKill = false;
    bool m_hasBurnEffect = false;

    QStringList m_activeEffects;
    QList<qreal> m_effectTimers;
};