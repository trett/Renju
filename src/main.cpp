#include "dot.h"
#include "game.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QPointer>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<Dot>("renju.core.dot", 1, 0, "Dot");
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    QList<QObject*> objects = engine.rootObjects();
    if (objects.isEmpty())
        return -1;

    auto boardItem = objects.first()->findChild<QObject*>("board");
    new Game(new GameBoard(boardItem));
    return app.exec();
}
