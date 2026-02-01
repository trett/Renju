#include "simpleai.h"

#include <algorithm>
#include <QThread>
#include <QMap>
#include <QRandomGenerator>
#include <cstring>

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
    
    foreach (QSharedPointer<Dot> move, moves) {
        m_board[move->y()][move->x()] = color;
        int tmp = -generate(opColor, depth - 1, -beta, -alpha);
        m_board[move->y()][move->x()] = 0;
        
        if (tmp > alpha) {
            alpha = tmp;
            if (depth == DEPTH) {
                m_tmpDot = QSharedPointer<Dot>(new Dot);
                m_tmpDot->setX(move->x());
                m_tmpDot->setY(move->y());
            }
        }
        
        if (alpha >= beta) {
            break;
        }
    }
    return alpha;
}

QVector<QSharedPointer<Dot> > SimpleAi::getAllMoves(DOT_COLOR color)
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

    auto getRate = [this](DOT_COLOR color, int x, int y, QMap<int, QVector<QSharedPointer<Dot>>> &map){
        QSharedPointer<Dot> dot(new Dot);
        dot->setX(x);
        dot->setY(y);
        dot->setColor(color);
        int rate = 0;
        int patternScore = 0;
        int blockScore = 0;
        
        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            if (blocksExistingFour(x, y, color, direction)) {
                blockScore = std::max(blockScore, 5000);
            }
        }
        
        m_board[y][x] = color;
        
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
            if (isOpenFour(x, y, color, direction)) {
                score = 3000;
            } else if (isOpenThree(x, y, color, direction)) {
                score = 500;
            } else if (isSplitThree(x, y, color, direction)) {
                score = 200;
            }
            patternScore = std::max(patternScore, score);
        }
        
        m_board[y][x] = 0;
        
        int finalScore = std::max(blockScore, patternScore);
        
        if (finalScore >= 5000) rate = 15;
        else if (finalScore >= 3000) rate = 10;
        else if (finalScore >= 500) rate = std::max(rate, 7);
        else if (finalScore >= 200) rate = std::max(rate, 5);
        
        map[rate].append(dot);
    };

    QMap<int, QVector<QSharedPointer<Dot>>> dotsMap;
    QMap<int, QVector<QSharedPointer<Dot>>> opDotsMap;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (candidates[y][x]) {
                getRate(color, x, y, dotsMap);
                getRate(color == WHITE ? BLACK : WHITE, x, y, opDotsMap);
            }
        }
    }

    QVector<QSharedPointer<Dot>> res;
    if (dotsMap.isEmpty() && opDotsMap.isEmpty()) {
        return res;
    }
    
    // Choose which map to prioritize
    // If it's AI turn (color == m_color): prefer attacking if good moves exist, else defend
    // If it's Opponent turn (recursive): same logic
    auto m = color == m_color ?
                (dotsMap.lastKey() >= opDotsMap.lastKey() ? &dotsMap : &opDotsMap) : 
                (dotsMap.lastKey() > opDotsMap.lastKey() ? &dotsMap : &opDotsMap);
                
    if (m->isEmpty()) return res;

    if (m->lastKey() < 3 || m->lastKey() >= 5) {
        res << m->last();
    } else {
        QMapIterator<int, QVector<QSharedPointer<Dot>>> it(*m);
        it.toBack();
        while(it.hasPrevious()) {
            it.previous();
            if (it.key() > 2) {
                res << it.value();
            }
        }
    }
    return res;
}

Dot *SimpleAi::nextMove()
{
    Dot *dot = new Dot;
    debug("AI moving");
    if (Table::history.empty()) {
        dot->setX(7);
        dot->setY(7);
        dot->setColor(m_color);
        return dot;
    }

    for(int y=0; y<BOARD_SIZE; ++y)
        for(int x=0; x<BOARD_SIZE; ++x)
            m_board[y][x] = Table::table[y][x];

    generate(m_color, DEPTH, -10000, 10000);

    if (!m_tmpDot) {
        std::memset(m_board, 0, sizeof(m_board));
        delete dot;
        return nullptr;
    }
    dot->setX(m_tmpDot->x());
    dot->setY(m_tmpDot->y());
    dot->setColor(m_color);
    
    std::memset(m_board, 0, sizeof(m_board));
    QThread::msleep(QRandomGenerator::global()->bounded(200) + 300);
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

bool SimpleAi::isOpenThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Check if total count is 3
    int totalCount = 1;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        totalCount += countConsecutive(x, y, dx, dy, color);
    }
    
    if (totalCount != 3) return false;
    
    // Check open ends
    int openEnds = 0;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        
        int nx = x, ny = y;
        while (nx + dx >= 0 && nx + dx < BOARD_SIZE && ny + dy >= 0 && ny + dy < BOARD_SIZE 
               && m_board[ny + dy][nx + dx] == color) {
            nx += dx;
            ny += dy;
        }
        
        nx += dx;
        ny += dy;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_board[ny][nx] == 0) {
            openEnds++;
        }
    }
    
    return openEnds == 2;
}

bool SimpleAi::isOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    int totalCount = 1;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        totalCount += countConsecutive(x, y, dx, dy, color);
    }
    
    if (totalCount != 4) return false;
    
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        
        int nx = x, ny = y;
        while (nx + dx >= 0 && nx + dx < BOARD_SIZE && ny + dy >= 0 && ny + dy < BOARD_SIZE 
               && m_board[ny + dy][nx + dx] == color) {
            nx += dx;
            ny += dy;
        }
        
        nx += dx;
        ny += dy;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_board[ny][nx] == 0) {
            return true;
        }
    }
    
    return false;
}

bool SimpleAi::blocksExistingFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    if (m_board[y][x] != 0) return false;
    
    int totalCount = 0;
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        
        // Count neighbors in this direction
        totalCount += countConsecutive(x, y, dx, dy, color);
    }
    
    return totalCount >= 4;
}

bool SimpleAi::isSplitThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    for (const auto &pair : direction) {
        int dx=0, dy=0;
        if(pair.first == '-') dx = -1; else if(pair.first == '+') dx = 1;
        if(pair.second == '-') dy = -1; else if(pair.second == '+') dy = 1;
        
        int count = 0;
        int gaps = 0;
        int nx = x + dx, ny = y + dy;
        
        for (int i = 0; i < 4 && nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE; i++) {
            if (m_board[ny][nx] == color) {
                count++;
            } else if (m_board[ny][nx] == 0) {
                gaps++;
                if (gaps > 1) break;
            } else {
                break;
            }
            nx += dx;
            ny += dy;
        }
        
        if (count == 2 && gaps == 1) {
            return true;
        }
    }
    return false;
}

int SimpleAi::evaluatePattern(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    int score = 0;
    if (blocksExistingFour(x, y, color, direction)) {
        score += 5000;
    } else if (isOpenFour(x, y, color, direction)) {
        score += 3000;
    } else if (isOpenThree(x, y, color, direction)) {
        score += 500;
    } else if (isSplitThree(x, y, color, direction)) {
        score += 200;
    }
    return score;
}

int SimpleAi::countPatterns(DOT_COLOR color)
{
    int totalScore = 0;
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_board[y][x] != 0) continue;
            
            // Block check (existing four)
            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (blocksExistingFour(x, y, color, direction)) {
                    totalScore += 5000;
                }
            }
            
            m_board[y][x] = color;
            
            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (isOpenFour(x, y, color, direction)) {
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
    // Optimization: we can reduce loop overhead by combining or simplifying
    // But for now, using m_board and simple loops is much faster than QVector
    
    QMap<int, int> opMap;
    QMap<int, int> selfMap;
    
    // For calculate, we need "self" and "op" relative to the AI's identity?
    // The original code calculated 'self' as m_color (AI) and 'op' as !m_color.
    // m_color is constant.
    // So 'selfMap' is always AI's lines. 'opMap' is always Human's lines.
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_board[y][x] != 0) continue;
            
            // Check opponent (Human) potential
            int opColor = m_color == WHITE ? BLACK : WHITE;
            int tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                 // Inline count
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
            
            // Check self (AI) potential
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
    
    // Critical Threat Logic:
    // If Human has existing four (opPatternScore >= 5000), we are dead if we don't block.
    // But calculate() is a static evaluation of a leaf node.
    // If we are at leaf node, and Human has 5000+ score (threat), 
    // it implies the previous move (by AI or Human) led to this.
    // High opPatternScore means Human HAS threats. Bad for AI.
    
    // Adjust scores
    if (opPatternScore >= 5000) return -MAX + 50;
    if (opPatternScore >= 3000) return -MAX + 100;
    if (opScore >= 5000) return -MAX;
    if (selfScore >= 5000) return MAX;
    
    selfScore += selfPatternScore;
    opScore += opPatternScore;
    
    // RETURN SIGNED SCORE
    // Positive if AI is winning, Negative if Human is winning.
    // Negamax expects: (Score for player_to_move).
    // If 'color' is AI: return (AI - Human).
    // If 'color' is Human: return (Human - AI) = -(AI - Human).
    
    int val = selfScore - opScore;
    if (color == m_color) return val;
    else return -val;
}