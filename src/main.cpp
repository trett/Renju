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
    Game *game = new Game();
    game->m_gameBoard->data()->m_board = root->findChild<QObject*>("board");
    QObject::connect(game->m_gameBoard->data()->m_board, SIGNAL(mouseClicked(QVariant)), game, SLOT(nextMove(QVariant)));
    return app.exec();
}
