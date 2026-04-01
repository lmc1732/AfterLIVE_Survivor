#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "GameManager.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    GameManager gameManager;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("gameManager", &gameManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}