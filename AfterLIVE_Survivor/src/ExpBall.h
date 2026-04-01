#pragma once
#include <QObject>

class ExpBall : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(int expValue READ expValue WRITE setExpValue NOTIFY expValueChanged)

public:
    explicit ExpBall(QObject* parent = nullptr);

    qreal x() const { return m_x; }
    void setX(qreal x);

    qreal y() const { return m_y; }
    void setY(qreal y);

    int expValue() const { return m_expValue; }
    void setExpValue(int expValue);

    void update(qreal deltaTime, const QPointF& playerPos, qreal attractRadius, qreal attractSpeed);
    bool isCollected() const { return m_collected; }

signals:
    void xChanged();
    void yChanged();
    void expValueChanged();

private:
    qreal m_x = 0;
    qreal m_y = 0;
    int m_expValue = 10;
    bool m_collected = false;
};