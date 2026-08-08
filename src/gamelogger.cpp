#include "gamelogger.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

QFile *GameLogger::m_file = nullptr;

static const char *colorName(Renju::DOT_COLOR color)
{
    switch (color) {
    case Renju::BLACK: return "BLACK";
    case Renju::WHITE: return "WHITE";
    default: return "NONE";
    }
}

void GameLogger::startGame(Renju::DOT_COLOR humanColor, Renju::DOT_COLOR aiColor)
{
    // Games can be restarted without a clean end (e.g. a new game started
    // mid-way); make sure the previous file is always closed off first.
    abortGame();

    QDir dir(QCoreApplication::applicationDirPath() + "/logs");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString fileName = "game_" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".log";
    m_file = new QFile(dir.filePath(fileName));
    if (!m_file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        delete m_file;
        m_file = nullptr;
        return;
    }

    QTextStream out(m_file);
    out << "GAME START human=" << colorName(humanColor) << " ai=" << colorName(aiColor) << "\n";
    out.flush();
}

void GameLogger::logMove(int moveNumber, Renju::DOT_COLOR color, int x, int y, bool isAi,
                          qint64 elapsedMs, int depthReached, int score)
{
    if (!m_file) {
        return;
    }

    QTextStream out(m_file);
    out << "MOVE #" << moveNumber << " " << (isAi ? "AI" : "HUMAN")
        << " color=" << colorName(color) << " pos=(" << x << "," << y << ")";
    if (isAi) {
        out << " depth=" << depthReached << " time=" << elapsedMs << "ms score=" << score;
    }
    out << "\n";
    // Left buffered (no flush): this runs once per move on the GUI thread,
    // and the game-end/abort path below always flushes+closes, so nothing
    // is lost on a normal or aborted game end - only on a hard crash.
}

void GameLogger::endGame(Renju::DOT_COLOR winner)
{
    closeFile("GAME END winner=" + QString(colorName(winner)));
}

void GameLogger::abortGame()
{
    closeFile("GAME ABORTED");
}

void GameLogger::closeFile(const QString &finalLine)
{
    if (!m_file) {
        return;
    }

    QTextStream out(m_file);
    out << finalLine << "\n";
    out.flush();

    m_file->close();
    delete m_file;
    m_file = nullptr;
}
