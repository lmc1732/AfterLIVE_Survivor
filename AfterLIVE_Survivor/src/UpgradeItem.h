#pragma once
#include <QObject>
#include <QString>

class Player;

class UpgradeItem : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString name READ name CONSTANT)
        Q_PROPERTY(QString description READ description CONSTANT)
        Q_PROPERTY(QString iconPath READ iconPath CONSTANT)

public:
    explicit UpgradeItem(QObject* parent = nullptr);
    virtual ~UpgradeItem();

    virtual QString name() const = 0;
    virtual QString description() const = 0;
    virtual QString iconPath() const = 0;   // Image path (reserved)
    virtual void apply(Player* player) = 0;
};