#include "dot.h"
#include "controller.h"
#include "gameboard.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<Dot>("renju.core.dot", 1, 0, "Dot");
    qmlRegisterSingletonType<Controller>("renju.core.controller", 1, 0, "Controller", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        auto *controller = new Controller();
        return controller;
    });
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    QList<QObject*> objects = engine.rootObjects();
    if (objects.isEmpty())
        return -1;

    auto boardItem = objects.first()->findChild<QObject*>("board");
    new GameBoard(boardItem);
    return app.exec();
}
