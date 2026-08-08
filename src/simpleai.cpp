#include "simpleai.h"

#include <algorithm>
#include <QElapsedTimer>
#include <QThread>
#include <QMap>
#include <QRandomGenerator>
#include <cstring>
#include <utility>

SimpleAi::SimpleAi(QObject *parent) : IPlayer(parent)
{
    std::memset(m_board, 0, sizeof(m_board));
}

int SimpleAi::generate(DOT_COLOR color, int depth, int alpha, int beta) {
    DOT_COLOR opColor = color == BLACK ? WHITE : BLACK;
    if (depth == 0) {
        return calculate(color);
    }
    auto moves = getAllMoves(color);

    if (moves.isEmpty()) {
        return calculate(color);
    }

    for (const Move &move : moves) {
        if (m_timer.hasExpired(TIME_BUDGET_MS)) {
            // Out of time: stop exploring further candidates at this node
            // so an over-budget iteration unwinds quickly instead of
            // running to completion regardless of how long that takes.
            break;
        }

        m_board[move.y][move.x] = color;

        int tmp;
        if (isWinningMove(move.x, move.y, color)) {
            // This move completes five in a row and ends the game right
            // here; no need to search further. Prefer faster wins (and,
            // symmetrically via negation, delay unavoidable losses) by
            // scoring shallower wins higher.
            tmp = MAX + depth;
        } else {
            tmp = -generate(opColor, depth - 1, -beta, -alpha);
        }

        m_board[move.y][move.x] = 0;

        if (tmp > alpha) {
            alpha = tmp;
            if (depth == m_rootDepth) {
                m_bestMove = move;
                m_hasBestMove = true;
            }
        }

        if (alpha >= beta) {
            break;
        }
    }
    return alpha;
}

QVector<SimpleAi::Move> SimpleAi::getAllMoves(DOT_COLOR color)
{
    // Neighborhood pruning: only consider cells within radius 2 of existing stones
    bool candidates[BOARD_SIZE][BOARD_SIZE];
    std::memset(candidates, 0, sizeof(candidates));
    bool emptyBoard = true;

    for(int y=0; y<BOARD_SIZE; ++y) {
        for(int x=0; x<BOARD_SIZE; ++x) {
            if(m_board[y][x] != 0) {
                emptyBoard = false;
                for(int dy=-2; dy<=2; ++dy) {
                    for(int dx=-2; dx<=2; ++dx) {
                        int ny = y + dy;
                        int nx = x + dx;
                        if(nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_board[ny][nx] == 0) {
                            candidates[ny][nx] = true;
                        }
                    }
                }
            }
        }
    }

    if(emptyBoard) {
        candidates[7][7] = true;
    }

    auto getRate = [this](DOT_COLOR color, int x, int y, QMap<int, QVector<Move>> &map){
        int rate = 0;
        int blockScore = 0;

        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            if (completesFive(x, y, color, direction)) {
                blockScore = std::max(blockScore, 5000);
            }
        }

        m_board[y][x] = color;

        // Track the two strongest simultaneous threats created by this
        // move. A second threat alongside the best one means this move is
        // a fork (e.g. a double three) the opponent cannot fully answer
        // with a single reply, so it earns a bonus on top of the best
        // individual pattern instead of being scored the same as a move
        // with only one threat.
        int bestScore = 0;
        int secondBestScore = 0;

        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            // Count total dots in row including this one
            int totalDots = 1;
            for(auto pair : direction) {
                int dx=0, dy=0;
                if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
                if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
                totalDots += countConsecutive(x, y, dx, dy, color);
            }
            rate = std::max(rate, totalDots);

            int score = 0;
            if (isDoubleOpenFour(x, y, color, direction)) {
                score = 5000; // both ends open: unblockable, as decisive as an outright win
            } else if (isOpenFour(x, y, color, direction)) {
                score = 3000;
            } else if (isOpenThree(x, y, color, direction)) {
                score = 500;
            } else if (isSplitThree(x, y, color, direction)) {
                score = 200;
            }
            if (score > bestScore) {
                secondBestScore = bestScore;
                bestScore = score;
            } else if (score > secondBestScore) {
                secondBestScore = score;
            }
        }

        m_board[y][x] = 0;

        int patternScore = bestScore;
        if (secondBestScore >= 200) {
            patternScore += secondBestScore + 500;
        }

        int finalScore = std::max(blockScore, patternScore);

        if (finalScore >= 5000) rate = 15;
        else if (finalScore >= 3000) rate = 10;
        else if (finalScore >= 500) rate = std::max(rate, 7);
        else if (finalScore >= 200) rate = std::max(rate, 5);

        map[rate].append(Move{x, y});
    };

    QMap<int, QVector<Move>> dotsMap;
    QMap<int, QVector<Move>> opDotsMap;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (candidates[y][x]) {
                getRate(color, x, y, dotsMap);
                getRate(color == WHITE ? BLACK : WHITE, x, y, opDotsMap);
            }
        }
    }

    QVector<Move> res;
    if (dotsMap.isEmpty() && opDotsMap.isEmpty()) {
        return res;
    }

    constexpr int WINNING_RATE = 15;
    constexpr int MAX_CANDIDATES = 8;

    if (!dotsMap.isEmpty() && dotsMap.lastKey() >= WINNING_RATE) {
        // The mover has a move that wins outright (or creates an
        // unstoppable double threat) - nothing else is worth considering.
        return dotsMap.last();
    }
    if (!opDotsMap.isEmpty() && opDotsMap.lastKey() >= WINNING_RATE) {
        // The opponent has an immediate winning/unstoppable move available;
        // it must be occupied.
        return opDotsMap.last();
    }

    // No forced win or must-block: gather a bounded set of the best-rated
    // candidates from both maps so the tree search actually has
    // alternatives to compare, instead of committing to a single guess.
    QVector<std::pair<int, Move>> ranked;
    for (auto it = dotsMap.constBegin(); it != dotsMap.constEnd(); ++it) {
        for (const Move &move : it.value()) {
            ranked.append({it.key(), move});
        }
    }
    for (auto it = opDotsMap.constBegin(); it != opDotsMap.constEnd(); ++it) {
        for (const Move &move : it.value()) {
            ranked.append({it.key(), move});
        }
    }
    std::sort(ranked.begin(), ranked.end(), [](const std::pair<int, Move> &a, const std::pair<int, Move> &b) {
        return a.first > b.first;
    });

    bool seen[BOARD_SIZE][BOARD_SIZE];
    std::memset(seen, 0, sizeof(seen));
    for (const auto &entry : ranked) {
        const Move &move = entry.second;
        if (seen[move.y][move.x]) continue;
        seen[move.y][move.x] = true;
        res << move;
        if (res.size() >= MAX_CANDIDATES) break;
    }
    return res;
}

Dot *SimpleAi::nextMove()
{
    debug("AI moving");
    if (Table::history.empty()) {
        Dot *dot = new Dot;
        dot->setX(7);
        dot->setY(7);
        dot->setColor(m_color);
        m_lastElapsedMs = 0;
        m_lastDepthReached = 0;
        m_lastScore = 0;
        return dot;
    }

    for(int y=0; y<BOARD_SIZE; ++y)
        for(int x=0; x<BOARD_SIZE; ++x)
            m_board[y][x] = Table::table[y][x];

    Move overallBest{};
    bool haveOverallBest = false;

    m_timer.start();
    // Iterative deepening: search as deep as the time budget allows.
    // Forced/narrow positions finish well within budget and get several
    // extra plies almost for free; wide, quiet positions simply stop at
    // whatever depth was reached and return the best move found so far.
    // generate() itself also checks m_timer on every node, so a single
    // pass that turns out to be too expensive gets unwound quickly
    // instead of running to completion regardless of cost.
    for (int depth = 2; depth <= MAX_SEARCH_DEPTH; depth += 2) {
        m_rootDepth = depth;
        m_hasBestMove = false;
        int score = generate(m_color, depth, -10000, 10000);
        if (m_hasBestMove) {
            overallBest = m_bestMove;
            haveOverallBest = true;
            m_lastDepthReached = depth;
            m_lastScore = score;
        }
        if (m_timer.hasExpired(TIME_BUDGET_MS)) {
            break;
        }
    }

    std::memset(m_board, 0, sizeof(m_board));

    if (!haveOverallBest) {
        return nullptr;
    }

    Dot *dot = new Dot;
    dot->setX(overallBest.x);
    dot->setY(overallBest.y);
    dot->setColor(m_color);

    qint64 elapsed = m_timer.elapsed();
    m_lastElapsedMs = elapsed;
    if (elapsed < MIN_THINK_MS) {
        QThread::msleep(MIN_THINK_MS - elapsed);
    }
    return dot;
}

int SimpleAi::countConsecutive(int x, int y, int dx, int dy, int color)
{
    int count = 0;
    int nx = x + dx;
    int ny = y + dy;
    while(nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_board[ny][nx] == color) {
        count++;
        nx += dx;
        ny += dy;
    }
    return count;
}

bool SimpleAi::isWinningMove(int x, int y, DOT_COLOR color)
{
    // Direct scan for an actual completed five-in-a-row through (x, y),
    // which must already contain `color`. Unlike the empty-cell-potential
    // heuristics used elsewhere (calculate()), this works even when the
    // resulting line ends up fully boxed in on both ends.
    for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
        int total = 1;
        for (const auto &pair : direction) {
            int dx=0, dy=0;
            if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
            if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
            total += countConsecutive(x, y, dx, dy, color);
        }
        if (total >= 5) return true;
    }
    return false;
}

void SimpleAi::lineStats(int x, int y, DOT_COLOR color, const Table::Direction &direction, int &totalCount, int &openEnds)
{
    totalCount = 1;
    openEnds = 0;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;

        int nx = x, ny = y;
        while (nx + dx >= 0 && nx + dx < BOARD_SIZE && ny + dy >= 0 && ny + dy < BOARD_SIZE
               && m_board[ny + dy][nx + dx] == color) {
            nx += dx;
            ny += dy;
            totalCount++;
        }

        nx += dx;
        ny += dy;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_board[ny][nx] == 0) {
            openEnds++;
        }
    }
}

bool SimpleAi::isOpenThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    int totalCount, openEnds;
    lineStats(x, y, color, direction, totalCount, openEnds);
    return totalCount == 3 && openEnds == 2;
}

bool SimpleAi::isOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // A four with at least one open end: the opponent must block that
    // specific cell or lose next move. Forcing, but (unlike
    // isDoubleOpenFour) still blockable.
    int totalCount, openEnds;
    lineStats(x, y, color, direction, totalCount, openEnds);
    return totalCount == 4 && openEnds >= 1;
}

bool SimpleAi::isDoubleOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // A four with BOTH ends open: unblockable, since the opponent can only
    // occupy one of the two completing cells. As decisive as an outright
    // win.
    int totalCount, openEnds;
    lineStats(x, y, color, direction, totalCount, openEnds);
    return totalCount == 4 && openEnds == 2;
}

bool SimpleAi::completesFive(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Does placing `color` at this empty cell complete a five-in-a-row?
    // Sums the existing run on both sides of the gap; if that combined
    // count is already >= 4, filling the gap reaches >= 5.
    if (m_board[y][x] != 0) return false;

    int totalCount = 0;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;

        totalCount += countConsecutive(x, y, dx, dy, color);
    }

    return totalCount >= 4;
}

bool SimpleAi::isSplitThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    int dx = 0, dy = 0;
    const auto &pair = direction.first();
    if (pair.first == '-') dx = -1; else if (pair.first == '+') dx = 1;
    if (pair.second == '-') dy = -1; else if (pair.second == '+') dy = 1;

    // Slide every 4-cell window along this axis that includes (x, y),
    // looking for exactly 3 same-color stones and exactly 1 gap (a broken
    // three such as X.XX or XX.X) - including windows that straddle both
    // sides of the newly played stone, which a purely one-directional scan
    // would miss.
    for (int offset = -3; offset <= 0; offset++) {
        int stones = 0, gaps = 0;
        bool valid = true;
        for (int i = 0; i < 4; i++) {
            int nx = x + (offset + i) * dx;
            int ny = y + (offset + i) * dy;
            if (nx < 0 || nx >= BOARD_SIZE || ny < 0 || ny >= BOARD_SIZE) { valid = false; break; }
            if (m_board[ny][nx] == color) stones++;
            else if (m_board[ny][nx] == 0) gaps++;
            else { valid = false; break; }
        }
        if (valid && stones == 3 && gaps == 1) {
            return true;
        }
    }
    return false;
}

int SimpleAi::countPatterns(DOT_COLOR color)
{
    int totalScore = 0;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_board[y][x] != 0) continue;

            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (completesFive(x, y, color, direction)) {
                    totalScore += 5000;
                }
            }

            m_board[y][x] = color;

            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (isDoubleOpenFour(x, y, color, direction)) {
                    totalScore += 5000;
                } else if (isOpenFour(x, y, color, direction)) {
                    totalScore += 3000;
                } else if (isOpenThree(x, y, color, direction)) {
                    totalScore += 500;
                } else if (isSplitThree(x, y, color, direction)) {
                    totalScore += 200;
                }
            }

            m_board[y][x] = 0;
        }
    }

    return totalScore;
}

int SimpleAi::calculate(DOT_COLOR color)
{
    QMap<int, int> opMap;
    QMap<int, int> selfMap;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_board[y][x] != 0) continue;

            // Opponent (Human) potential
            int opColor = m_color == WHITE ? BLACK : WHITE;
            int tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                 int current = 1;
                 for(auto pair : direction) {
                     int dx=0, dy=0;
                     if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
                     if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
                     current += countConsecutive(x, y, dx, dy, opColor);
                 }
                 tmp = std::max(tmp, current);
            }
            opMap[tmp]++;

            // Self (AI) potential
            tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                 int current = 1;
                 for(auto pair : direction) {
                     int dx=0, dy=0;
                     if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
                     if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
                     current += countConsecutive(x, y, dx, dy, m_color);
                 }
                 tmp = std::max(tmp, current);
            }
            selfMap[tmp]++;
        }
    }

    int opScore = opMap.lastKey() * 1000 + opMap.last();
    int selfScore = selfMap.lastKey() * 1000 + selfMap.last();

    DOT_COLOR humanColor = m_color == WHITE ? BLACK : WHITE;
    int opPatternScore = countPatterns(humanColor);
    int selfPatternScore = countPatterns(m_color);

    // High opPatternScore/selfPatternScore means one side has an existing
    // threat that wasn't already caught by isWinningMove() in generate()
    // (e.g. it's still one move away, or is a fork the other side cannot
    // fully answer). Treat it as close to decisive - and, importantly,
    // return early rather than folding it into selfScore/opScore below:
    // countPatterns() sums over every empty cell and direction with no
    // cap, so without this early return a genuine multi-threat position
    // can push the combined score past the +-10000 alpha-beta window used
    // throughout the search, which silently breaks alpha-beta's pruning
    // correctness.
    if (opPatternScore >= 5000) return -MAX + 50;
    if (opPatternScore >= 3000) return -MAX + 100;
    if (selfPatternScore >= 5000) return MAX - 50;
    if (selfPatternScore >= 3000) return MAX - 100;
    if (opScore >= 5000) return -MAX;
    if (selfScore >= 5000) return MAX;

    selfScore += selfPatternScore;
    opScore += opPatternScore;

    // Positive if AI is winning, negative if Human is winning; negamax
    // expects the score from the perspective of `color` (the player to
    // move at this node).
    int val = selfScore - opScore;
    // Defensive bound: keep the leaf value comfortably inside the
    // +-10000 alpha-beta window used throughout the search, regardless of
    // how selfScore/opScore were assembled above.
    val = std::clamp(val, -9000, 9000);
    if (color == m_color) return val;
    else return -val;
}
