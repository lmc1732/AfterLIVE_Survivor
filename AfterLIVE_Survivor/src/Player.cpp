#include "Player.h"
#include "Weapon_Carrot.h"
#include "Weapon_Bone.h"
#include <QtGlobal>
#include <QDebug>

Player::Player(QObject* parent) : QObject(parent)
{
}

void Player::setCharacterType(CharacterType type)
{
    if (m_weapon) {
        m_weapon->deleteLater();
        m_weapon = nullptr;
    }

    if (type == CharacterA) {
        m_maxHp = 100;
        m_hp = 100;
        m_hpRegen = 2;
        m_armor = 0;
        m_moveSpeed = 280;
        m_power = 12;
        m_area = 60;
        m_attackSpeed = 1.5;
        m_weapon = new Carrot(this);
    }
    else {
        m_maxHp = 60;
        m_hp = 60;
        m_hpRegen = 1;
        m_armor = 5;
        m_moveSpeed = 320;
        m_power = 8;
        m_area = 40;
        m_attackSpeed = 3.0;
        m_weapon = new Bone(this);
    }

    emit maxHpChanged();
    emit hpChanged();
    emit hpRegenChanged();
    emit armorChanged();
    emit moveSpeedChanged();
    emit powerChanged();
    emit areaChanged();
    emit attackSpeedChanged();
}

void Player::setMaxHp(int maxHp)
{
    if (m_maxHp == maxHp) return;
    m_maxHp = maxHp;
    emit maxHpChanged();
    if (m_hp > m_maxHp) setHp(m_maxHp);
}

void Player::setHp(int hp)
{
    hp = qBound(0, hp, m_maxHp);
    if (m_hp == hp) return;
    m_hp = hp;
    emit hpChanged();
}

void Player::setHpRegen(int hpRegen)
{
    if (m_hpRegen == hpRegen) return;
    m_hpRegen = hpRegen;
    emit hpRegenChanged();
}

void Player::setArmor(int armor)
{
    if (m_armor == armor) return;
    m_armor = armor;
    emit armorChanged();
}

void Player::setMoveSpeed(int moveSpeed)
{
    if (m_moveSpeed == moveSpeed) return;
    m_moveSpeed = moveSpeed;
    emit moveSpeedChanged();
}

void Player::setPower(int power)
{
    if (m_power == power) return;
    m_power = power;
    emit powerChanged();
}

void Player::setArea(int area)
{
    if (m_area == area) return;
    m_area = area;
    emit areaChanged();
}

void Player::setAttackSpeed(double attackSpeed)
{
    if (qFuzzyCompare(m_attackSpeed, attackSpeed)) return;
    m_attackSpeed = attackSpeed;
    emit attackSpeedChanged();
}

void Player::setExp(int exp)
{
    if (m_exp == exp) return;
    m_exp = exp;
    emit expChanged();
}

void Player::setLevel(int level)
{
    if (m_level == level) return;
    m_level = level;
    emit levelChanged();
}

void Player::setExpMultiplier(double mult)
{
    if (qFuzzyCompare(m_expMultiplier, mult)) return;
    m_expMultiplier = mult;
    emit expMultiplierChanged();
}

void Player::setDamageMultiplier(double mult)
{
    if (qFuzzyCompare(m_damageMultiplier, mult)) return;
    m_damageMultiplier = mult;
    emit damageMultiplierChanged();
}

void Player::setHasSlowEffect(bool enable)
{
    if (m_hasSlowEffect == enable) return;
    m_hasSlowEffect = enable;
    emit hasSlowEffectChanged();
}

void Player::setHasHealOnKill(bool enable)
{
    if (m_hasHealOnKill == enable) return;
    m_hasHealOnKill = enable;
    emit hasHealOnKillChanged();
}

void Player::setHasBurnEffect(bool enable)
{
    if (m_hasBurnEffect == enable) return;
    m_hasBurnEffect = enable;
    emit hasBurnEffectChanged();
}

int Player::expRequiredForLevel(int level) const
{
    // Experience required per level = 100 * level
    return 100 * level;
}

int Player::expToNextLevel() const
{
    // Return total experience needed to level up from current level to next
    return expRequiredForLevel(m_level);
}

void Player::addExp(int amount)
{
    if (amount <= 0) return;
    int finalAmount = static_cast<int>(amount * m_expMultiplier);
    m_exp += finalAmount;
    emit expChanged();

    // Check for level up
    int requiredForCurrentLevel = expRequiredForLevel(m_level);
    while (m_exp >= requiredForCurrentLevel && requiredForCurrentLevel > 0) {
        // Level up: Deduct experience required for current level
        m_exp -= requiredForCurrentLevel;
        m_level++;
        emit levelChanged();
        emit expChanged();
        emit levelUpRequested();
        qDebug() << "Level up! Now level:" << m_level << "Remaining exp for next level:" << m_exp;

        // Recalculate experience required for next level
        requiredForCurrentLevel = expRequiredForLevel(m_level);
    }
}

void Player::takeDamage(int damage, bool ignoreArmor)
{
    if (m_invincibilityTimer > 0) return;

    int actualDamage = damage;
    if (!ignoreArmor) {
        actualDamage = damage - m_armor;
        if (actualDamage < 1) actualDamage = 1;
    }
    int newHp = m_hp - actualDamage;
    setHp(qMax(0, newHp));
    m_invincibilityTimer = m_invincibilityDuration;
    qDebug() << "Player took" << actualDamage << "damage, HP:" << m_hp;
}

void Player::updateInvincibility()
{
    if (m_invincibilityTimer > 0) {
        m_invincibilityTimer--;
    }
}

void Player::addEffect(const QString& effect, qreal duration)
{
    int index = m_activeEffects.indexOf(effect);
    if (index != -1) {
        m_effectTimers[index] = duration;
    }
    else {
        m_activeEffects.append(effect);
        m_effectTimers.append(duration);
    }
    emit activeEffectsChanged();
}

void Player::updateEffects(qreal deltaTime)
{
    bool changed = false;
    for (int i = m_effectTimers.size() - 1; i >= 0; --i) {
        m_effectTimers[i] -= deltaTime;
        if (m_effectTimers[i] <= 0) {
            m_activeEffects.removeAt(i);
            m_effectTimers.removeAt(i);
            changed = true;
        }
    }
    if (changed) emit activeEffectsChanged();
}

void Player::removeEffect(const QString& effect)
{
    int index = m_activeEffects.indexOf(effect);
    if (index != -1) {
        m_activeEffects.removeAt(index);
        m_effectTimers.removeAt(index);
        emit activeEffectsChanged();
    }
}

bool Player::hasEffect(const QString& effect) const
{
    return m_activeEffects.contains(effect);
}

void Player::setRooted(bool rooted)
{
    if (rooted == m_isRooted) return;
    if (rooted) {
        m_savedSpeed = m_moveSpeed;
        setMoveSpeed(0);
        m_isRooted = true;
    }
    else {
        setMoveSpeed(m_savedSpeed);
        m_isRooted = false;
    }
}