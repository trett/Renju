#ifndef GAMELOGGER_H
#define GAMELOGGER_H

#include "renju.h"

#include <QString>
#include <QtGlobal>

class QFile;

/**
 * @brief Writes a plain-text move log per game to disk (one file per game,
 * under <applicationDirPath>/logs), so games can be replayed/analyzed later
 * to spot weaknesses in the AI's strategy.
 */
class GameLogger
{
public:
    GameLogger() = delete;

    static void startGame(Renju::DOT_COLOR humanColor, Renju::DOT_COLOR aiColor);
    static void logMove(int moveNumber, Renju::DOT_COLOR color, int x, int y, bool isAi,
                         qint64 elapsedMs = -1, int depthReached = -1, int score = 0);
    static void endGame(Renju::DOT_COLOR winner);
    // No-op if the game already ended cleanly via endGame(); otherwise
    // records that the log stopped short of a result (new game started, or
    // the app quit, mid-game).
    static void abortGame();

private:
    static void closeFile(const QString &finalLine);

    static QFile *m_file;
};
#endif // GAMELOGGER_H
