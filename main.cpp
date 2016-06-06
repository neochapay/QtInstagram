#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickItem>

#include "src/api/instagram.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<Instagram>("instagram", 1, 0, "Instagram");

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
