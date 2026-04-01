#pragma once
#include <QObject>
#include <QList>
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Explosion.h"
#include "ExpBall.h"
#include "UpgradeItem.h"

class GameManager : public QObject
{
    Q_OBJECT
        Q_PROPERTY(Player* player READ player CONSTANT)
        Q_PROPERTY(QList<QObject*> enemies READ enemies NOTIFY enemiesChanged)
        Q_PROPERTY(QList<QObject*> bullets READ bullets NOTIFY bulletsChanged)
        Q_PROPERTY(QList<QObject*> explosions READ explosions NOTIFY explosionsChanged)
        Q_PROPERTY(QList<QObject*> expBalls READ expBalls NOTIFY expBallsChanged)
        Q_PROPERTY(bool isGameOver READ isGameOver NOTIFY gameOverChanged)
        Q_PROPERTY(bool isGamePaused READ isGamePaused NOTIFY gamePausedChanged)
        Q_PROPERTY(qreal gameTime READ gameTime NOTIFY gameTimeChanged)  // Game Time

public:
    explicit GameManager(QObject* parent = nullptr);

    Player* player() const { return m_player; }
    QList<QObject*> enemies() const;
    QList<QObject*> bullets() const;
    QList<QObject*> explosions() const;
    QList<QObject*> expBalls() const;
    bool isGameOver() const { return m_gameOver; }
    bool isGamePaused() const { return m_gamePaused; }
    qreal gameTime() const { return m_gameTime; }

    Q_INVOKABLE void selectCharacter(int type);
    Q_INVOKABLE void update(qreal deltaTime, qreal playerX, qreal playerY);
    Q_INVOKABLE void restartGame();
    Q_INVOKABLE void selectUpgrade(int index);

    void addExplosion(Explosion* explosion);
    void addExpBall(ExpBall* ball);
    void levelUp();

signals:
    void enemiesChanged();
    void bulletsChanged();
    void explosionsChanged();
    void expBallsChanged();
    void playerDied();
    void gameWon();
    void gameOverChanged();
    void gamePausedChanged();
    void gameTimeChanged();
    void showUpgradeOptions(const QStringList& names, const QStringList& descriptions, const QStringList& icons);

private:
    void spawnEnemy();
    void checkCollisions();
    void cleanupDeadObjects();
    void checkBulletEnemyCollision();
    void checkPlayerEnemyCollision();
    void checkPlayerExpBallCollision();
    QList<UpgradeItem*> generateRandomUpgrades();

    Player* m_player;
    QList<Enemy*> m_enemies;
    QList<Bullet*> m_bullets;
    QList<Explosion*> m_explosions;
    QList<ExpBall*> m_expBalls;
    QList<UpgradeItem*> m_currentUpgrades;
    qreal m_spawnTimer = 0;
    qreal m_spawnInterval = 1.0;
    qreal m_regenTimer = 0;
    qreal m_playerX = 400;
    qreal m_playerY = 300;
    qreal m_gameTime = 0;  // Game Timer
    bool m_gameOver = false;
    bool m_gamePaused = false;
    bool m_isWaitingForUpgrade = false;
};