#pragma once
#include <QObject>

class Particle : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(qreal vx READ vx WRITE setVx NOTIFY vxChanged)
        Q_PROPERTY(qreal vy READ vy WRITE setVy NOTIFY vyChanged)
        Q_PROPERTY(int colorR READ colorR WRITE setColorR NOTIFY colorRChanged)
        Q_PROPERTY(int colorG READ colorG WRITE setColorG NOTIFY colorGChanged)
        Q_PROPERTY(int colorB READ colorB WRITE setColorB NOTIFY colorBChanged)
        Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
        Q_PROPERTY(qreal life READ life WRITE setLife NOTIFY lifeChanged)

public:
    explicit Particle(QObject* parent = nullptr);

    qreal x() const { return m_x; }
    void setX(qreal x);
    qreal y() const { return m_y; }
    void setY(qreal y);
    qreal vx() const { return m_vx; }
    void setVx(qreal vx);
    qreal vy() const { return m_vy; }
    void setVy(qreal vy);
    int colorR() const { return m_colorR; }
    void setColorR(int r);
    int colorG() const { return m_colorG; }
    void setColorG(int g);
    int colorB() const { return m_colorB; }
    void setColorB(int b);
    qreal size() const { return m_size; }
    void setSize(qreal size);
    qreal life() const { return m_life; }
    void setLife(qreal life);

    void update(qreal deltaTime);
    bool isDead() const { return m_life <= 0; }

signals:
    void xChanged();
    void yChanged();
    void vxChanged();
    void vyChanged();
    void colorRChanged();
    void colorGChanged();
    void colorBChanged();
    void sizeChanged();
    void lifeChanged();

private:
    qreal m_x = 0;
    qreal m_y = 0;
    qreal m_vx = 0;
    qreal m_vy = 0;
    int m_colorR = 255;
    int m_colorG = 200;
    int m_colorB = 100;
    qreal m_size = 5;
    qreal m_life = 0.5;
    qreal m_maxLife = 0.5;
};

class Explosion : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QList<QObject*> particles READ particles NOTIFY particlesChanged)

public:
    explicit Explosion(QObject* parent = nullptr);
    ~Explosion();

    void setCenter(qreal x, qreal y);
    void update(qreal deltaTime);
    bool isFinished() const;

    QList<QObject*> particles() const;

signals:
    void particlesChanged();

private:
    QList<Particle*> m_particles;
    qreal m_centerX = 0;
    qreal m_centerY = 0;
    bool m_initialized = false;
    void generateParticles();
};