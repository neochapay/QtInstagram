#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "src/api/instagram.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Instagram *InstagramClient = new Instagram("","");
    InstagramClient->login();

    return app.exec();
}
