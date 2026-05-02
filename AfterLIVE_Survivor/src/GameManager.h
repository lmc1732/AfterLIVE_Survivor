#pragma once
#include <QObject>
#include <QList>
#include <QTimer>
#include <QMap>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"
#include "ExpBall.h"
#include "UpgradeItem.h"
#include "EnemyType.h"

class SporeMistEffect;

class GameManager : public QObject
{
    Q_OBJECT
        Q_PROPERTY(Player* player READ player NOTIFY playerChanged)
        Q_PROPERTY(int selectedCharacter READ selectedCharacter WRITE setSelectedCharacter NOTIFY selectedCharacterChanged)
        Q_PROPERTY(QList<QObject*> enemies READ enemies NOTIFY enemiesChanged)
        Q_PROPERTY(QList<QObject*> bullets READ bullets NOTIFY bulletsChanged)
        Q_PROPERTY(QList<QObject*> explosions READ explosions NOTIFY explosionsChanged)
        Q_PROPERTY(QList<QObject*> expBalls READ expBalls NOTIFY expBallsChanged)
        Q_PROPERTY(QList<QObject*> sporeMistEffects READ sporeMistEffects NOTIFY sporeMistEffectsChanged)
        Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOverChanged)
        Q_PROPERTY(bool isGamePaused READ isGamePaused NOTIFY gamePausedChanged)
        Q_PROPERTY(qreal gameTime READ gameTime NOTIFY gameTimeChanged)

public:
    explicit GameManager(QObject* parent = nullptr);

    Player* player() const { return m_player; }
    QList<QObject*> enemies() const;
    QList<QObject*> bullets() const;
    QList<QObject*> explosions() const;
    QList<QObject*> expBalls() const;
    QList<QObject*> sporeMistEffects() const;
    bool isGameOver() const { return m_gameOver; }
    bool isGamePaused() const { return m_gamePaused; }
    qreal gameTime() const { return m_gameTime; }

    Q_INVOKABLE void selectCharacter(int type);
    Q_INVOKABLE void update(qreal deltaTime, qreal playerX, qreal playerY);
    Q_INVOKABLE void restartGame();
    Q_INVOKABLE void setSpawnTimerEnabled(bool enabled);
    Q_INVOKABLE void selectUpgrade(int index);
    Q_INVOKABLE void refreshEnemies();

    void addExplosion(Explosion* explosion);
    void addExpBall(ExpBall* ball);
    void addBullet(Bullet* bullet);
    void addSporeMistEffect(SporeMistEffect* effect);
    void setPlayerPosition(qreal x, qreal y);
    void addEnemyDirect(Enemy* enemy);
    qreal playerX() const { return m_playerX; }
    qreal playerY() const { return m_playerY; }
    Enemy* spawnEnemyAt(EnemyType type, qreal x, qreal y);

    void resetGameData();
    void setIsGamePaused(bool paused);
    void levelUp();

    int selectedCharacter() const { return m_selectedCharacter; }
    void setSelectedCharacter(int character);

signals:
    void selectedCharacterChanged();
    void enemiesChanged();
    void bulletsChanged();
    void explosionsChanged();
    void expBallsChanged();
    void sporeMistEffectsChanged();
    void playerDied();
    void gameWon();
    void gameOverChanged();
    void gamePausedChanged();
    void gameTimeChanged();
    void showUpgradeOptions(const QStringList& names, const QStringList& descriptions, const QStringList& icons);
    void playerChanged();

private slots:
    void spawnEnemy();
    void onEnemyDied(Enemy* enemy);

private:
    int m_selectedCharacter = 0;  // 0 means CHERRY£¬1 means GANGANJI

    void checkCollisions();
    void cleanupDeadObjects();
    void checkBulletEnemyCollision();
    void checkPlayerEnemyCollision();
    void checkPlayerRootCollision();
    void checkPlayerExpBallCollision();
    QList<UpgradeItem*> generateRandomUpgrades();

    Player* m_player;
    QList<Enemy*> m_enemies;
    QList<Bullet*> m_bullets;
    QList<Explosion*> m_explosions;
    QList<ExpBall*> m_expBalls;
    QList<UpgradeItem*> m_currentUpgrades;
    QList<SporeMistEffect*> m_sporeMistEffects;

    QMap<Enemy*, qreal> m_skillTimers;

    qreal m_regenTimer = 0;
    qreal m_playerX = 400;
    qreal m_playerY = 300;
    qreal m_gameTime = 0;

    bool m_gameOver = false;
    bool m_gamePaused = false;
    bool m_isWaitingForUpgrade = false;
    bool m_gameWinHandled = false;
    bool m_playerInDamageMist = false;
    bool m_prevInDamageMist = false;

    QTimer* m_spawnTimer = nullptr;
    qreal m_spawnInterval = 0.8;
    bool m_bossSpawned = false;
    Enemy* m_boss = nullptr;

    const qreal WAVE1_END = 30.0;
    const qreal WAVE2_END = 60.0;
    const qreal WAVE3_END = 90.0;
    const qreal WAVE4_END = 120.0;
};