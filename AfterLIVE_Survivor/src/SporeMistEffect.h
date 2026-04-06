#pragma once
#include <QObject>

class SporeMistEffect : public QObject
{
    Q_OBJECT
        Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
        Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
        Q_PROPERTY(qreal radius READ radius CONSTANT)

public:
    explicit SporeMistEffect(QObject* parent = nullptr);
    void setPosition(qreal x, qreal y);
    void update(qreal deltaTime);
    bool isExpired() const { return m_lifeTime >= m_duration; }
    qreal x() const { return m_x; }
    qreal y() const { return m_y; }
    qreal radius() const { return m_radius; }
    void setX(qreal x);
    void setY(qreal y);

signals:
    void xChanged();
    void yChanged();

private:
    qreal m_x = 0;
    qreal m_y = 0;
    qreal m_radius = 60.0;
    qreal m_lifeTime = 0;
    qreal m_duration = 3.0;
};