#pragma once
#include <QObject>
#include <QPointF>
#include "EnemyType.h"

class Skill;
class Player;
class GameManager;

class Enemy : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(int hp READ hp WRITE setHp NOTIFY hpChanged)
        Q_PROPERTY(int maxHp READ maxHp WRITE setMaxHp NOTIFY maxHpChanged)
        Q_PROPERTY(EnemyType type READ type NOTIFY typeChanged)
        Q_PROPERTY(qreal size READ size CONSTANT)
        Q_PROPERTY(bool isBlocking READ isBlocking NOTIFY isBlockingChanged)
        Q_PROPERTY(bool facingRight READ facingRight WRITE setFacingRight NOTIFY facingRightChanged)
        Q_PROPERTY(bool isSporeFogActive READ isSporeFogActive WRITE setIsSporeFogActive NOTIFY sporeFogChanged)
        Q_PROPERTY(bool isSummonedByBoss READ isSummonedByBoss WRITE setIsSummonedByBoss NOTIFY summonedByBossChanged)

public:
    explicit Enemy(QObject* parent = nullptr);
    ~Enemy();

    qreal x() const { return m_x; }
    void setX(qreal x);
    qreal y() const { return m_y; }
    void setY(qreal y);
    int hp() const { return m_hp; }
    void setHp(int hp);
    int maxHp() const { return m_maxHp; }
    void setMaxHp(int maxHp);
    qreal speed() const { return m_speed; }
    void setSpeed(qreal speed);
    EnemyType type() const { return m_type; }
    void setType(EnemyType type);
    int damage() const { return m_damage; }
    void setDamage(int damage);
    int expReward() const { return m_expReward; }
    void setExpReward(int exp);
    qreal size() const { return m_size; }
    void setSize(qreal size);

    bool facingRight() const { return m_facingRight; }
    void setFacingRight(bool right);

    void takeDamage(int damage);
    void update(const QPointF& playerPos, qreal deltaTime);
    bool isDead() const { return m_hp <= 0; }

    bool isSummonedByBoss() const { return m_isSummonedByBoss; }
    void setIsSummonedByBoss(bool value);

    // Skill System
    Skill* getSkill() const { return m_skill; }
    void setSkill(Skill* skill) { m_skill = skill; }
    void onHit(Player* player, GameManager* gm);   // Trigger skill on hit
    void updateActiveSkill(qreal deltaTime, GameManager* gm);
    void explode(GameManager* gm);
    void setDirection(qreal dx, qreal dy) { m_dirX = dx; m_dirY = dy; }
    void setGameManager(GameManager* gm) { m_gameManager = gm; }
    bool isBlocking() const { return m_isBlocking; }
    void startBlocking();
    void stopBlocking();
    bool isBlockCooldown() const { return m_isBlockCooldown; }

    // Boss Related
    void startBossLogic(GameManager* gm);
    void stopBossLogic();
    void setBossPhase(int phase) { m_bossPhase = phase; }
    int bossPhase() const { return m_bossPhase; }

    // Status Effects
    void updateStatusEffects(qreal deltaTime);
    void applyBurn(int damagePerSecond, qreal duration);
    void applySlow(qreal factor, qreal duration);

    bool isSporeFogActive() const { return m_isSporeFogActive; }
    void setIsSporeFogActive(bool active);
    void activateSporeFog(int durationMs = 2000);

signals:
    void xChanged();
    void yChanged();
    void hpChanged();
    void maxHpChanged();
    void typeChanged();
    void facingRightChanged();
    void died(Enemy* enemy);
    void isBlockingChanged();
    void sporeFogChanged();
    void summonedByBossChanged();

private slots:
    void onBranchGunnerShoot();
    void onMoveTimeout();
    void onShootTimeout();
    void onSummonTimeout();

private:
    qreal m_x = 0;
    qreal m_y = 0;
    int m_hp = 1;
    int m_maxHp = 1;
    qreal m_speed = 100;
    int m_damage = 10;
    int m_expReward = 10;
    bool m_facingRight = true;
    EnemyType m_type = EnemyType::MossBall;
    qreal m_size = 30.0;
    Skill* m_skill = nullptr;
    qreal m_skillTimer = 0;
    qreal m_skillCooldown = 3.0;
    qreal m_explosionRadius = 60.0;         
    int m_explosionDamage = 15;             
    int m_smallBerryCount = 3;              
    qreal m_dirX = 1.0, m_dirY = 0.0;

    // Status Effects
    bool m_isSporeFogActive = false;
    QTimer* m_fogTimer = nullptr;
    qreal m_speedMultiplier = 1.0;
    qreal m_slowTimer = 0.0;
    int m_burnDamagePerSecond = 0;
    qreal m_burnTimer = 0.0;

    bool m_isBlocking = false;
    bool m_isBlockCooldown = false;
    QTimer* m_blockTimer = nullptr;
    QTimer* m_cooldownTimer = nullptr;
    QTimer* m_branchShootTimer = nullptr;

    // Boss Related
    int m_bossPhase = 1;
    bool m_isMoving = true;
    QPointF m_moveDirection;
    qreal m_baseSpeed = 0;  // Save base move speed
    GameManager* m_gameManager = nullptr;
    void scheduleNextShoot();
    void scheduleNextMove();
    void scheduleNextSummon();

    bool m_bossLogicActive = false;   // Control whether recursion continues
    bool m_initialDirectionSet = false;  // Ensure direction is initialized only once
    int m_stuckCounter = 0;
    bool m_isSummonedByBoss = false;
};