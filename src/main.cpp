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
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QList<QObject*> objects = engine.rootObjects();
    if (objects.isEmpty())
        return -1;

    auto root = objects.first();
    auto boardItem = root->findChild<QObject*>("board");
    GameBoard *gameBoard = new GameBoard(boardItem);
    Game *game = new Game(gameBoard);
    QObject::connect(boardItem, SIGNAL(mouseClicked(QVariant)), game, SLOT(nextMove(QVariant)));
    return app.exec();
}
