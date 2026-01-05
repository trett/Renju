#include "simpleai.h"

#include <QThread>
#include <QMap>
#include <QRandomGenerator>

SimpleAi::SimpleAi(QObject *parent) : IPlayer(parent), m_model(BOARD_SIZE, QVector<int>(BOARD_SIZE, 0))
{
}

int SimpleAi::generate(DOT_COLOR color, int depth, int alpha, int beta) {
    DOT_COLOR opColor = color ? BLACK : WHITE;
    if (depth == 0) {
        return calculate();
    }
    auto moves = getAllMoves(color);
    
    // If no moves available, evaluate position
    if (moves.isEmpty()) {
        return calculate();
    }
    
    foreach (QSharedPointer<Dot> move, moves) {
        // make move
        m_model[move->y()][move->x()] = color;
        int tmp = -generate(opColor, depth - 1, -beta, -alpha);
        // unmake move
        m_model[move->y()][move->x()] = NONE;
        
        if (tmp > alpha) {
            alpha = tmp;
            // Only update best move at root level
            if (depth == DEPTH) {
                m_tmpDot = QSharedPointer<Dot>(new Dot);
                m_tmpDot->setX(move->x());
                m_tmpDot->setY(move->y());
            }
        }
        
        // Beta cutoff: prune when alpha >= beta (current player's best score meets/exceeds
        // the opponent's threshold). This is critical for alpha-beta pruning efficiency.
        if (alpha >= beta) {
            break;
        }
    }
    return alpha;
}

QVector<QSharedPointer<Dot> > SimpleAi::getAllMoves(DOT_COLOR color)
{
    auto getRate = [this](DOT_COLOR color, int x, int y, QMap<int, QVector<QSharedPointer<Dot>>> &map){
        QSharedPointer<Dot> dot(new Dot);
        dot->setX(x);
        dot->setY(y);
        dot->setColor(color);
        int rate = -1;
        int patternScore = 0;
        int blockScore = 0;
        
        // First check if this position blocks an existing threat (without placing the dot)
        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            if (blocksExistingFour(x, y, color, direction)) {
                blockScore = std::max(blockScore, 5000);
            }
        }
        
        // Temporarily place the dot to check other patterns
        m_model[y][x] = color;
        
        for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
            rate = std::max(rate, Table::getDotCountInRow(dot.data(), direction, m_model));
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
        
        // Remove the dot
        m_model[y][x] = NONE;
        
        // Combine scores - blocking existing threats is most critical
        int finalScore = std::max(blockScore, patternScore);
        
        // Boost rate based on danger patterns
        if (finalScore >= 5000) {
            rate = 15;  // Blocks existing four - MUST DO!
        } else if (finalScore >= 3000) {
            rate = 10;  // Open four - critical priority
        } else if (finalScore >= 500) {
            rate = std::max(rate, 7);  // Open three - high priority
        } else if (finalScore >= 200) {
            rate = std::max(rate, 5);  // Split three - medium priority
        }
        
        map[rate].append(dot);
    };
    QMap<int, QVector<QSharedPointer<Dot>>> dotsMap;
    QMap<int, QVector<QSharedPointer<Dot>>> opDotsMap;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_model.at(y).at(x) != 0) {
                continue;
            }
            getRate(color, x, y, dotsMap);
            getRate(color == WHITE ? BLACK : WHITE, x, y, opDotsMap);
        }
    }

    QVector<QSharedPointer<Dot>> res;
    // Check if maps are not empty before accessing lastKey()
    // This prevents crashes when board is full or no valid moves exist
    if (dotsMap.isEmpty() || opDotsMap.isEmpty()) {
        return res;
    }
    
    auto m = color == m_color ?
                dotsMap.lastKey() >= opDotsMap.lastKey() ?
                    &dotsMap : &opDotsMap : dotsMap.lastKey() > opDotsMap.lastKey() ?
                        &dotsMap : &opDotsMap;
    if (m->lastKey() < 3 || m->lastKey() >= 5) {
        res << m->last();
    } else {
        QMapIterator<int, QVector<QSharedPointer<Dot>>> it(*m);
        while(it.hasNext()) {
            auto val = it.next();
            if (val.key() > 2) {
                res << val.value();
            }
        }
    }
    return res;
}

Dot *SimpleAi::nextMove()
{
    Dot *dot = new Dot;
    debug("AI moving");
    // first move
    if (Table::history.empty()) {
        dot->setX(7);
        dot->setY(7);
        dot->setColor(m_color);
        return dot;
    }
    // copy Table to model
    std::copy(Table::table.constBegin(), Table::table.constEnd(), m_model.begin());
    generate(m_color, DEPTH, -10000, 10000);
    // if no move was selected during generate(), avoid dereferencing a null pointer
    if (!m_tmpDot) {
        // clear model
        std::for_each(m_model.begin(), m_model.end(), [](auto &it) { it.fill(0); } );
        delete dot;
        return nullptr;
    }
    dot->setX(m_tmpDot->x());
    dot->setY(m_tmpDot->y());
    dot->setColor(m_color);
    // clear model
    std::for_each(m_model.begin(), m_model.end(), [](auto &it) { it.fill(0); } );
    // humanize
    QThread::msleep(QRandomGenerator::global()->bounded(200) + 300);
    return dot;
}

bool SimpleAi::isOpenThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Pattern: _XXX_ (three in a row with both ends open)
    Dot dot;
    dot.setX(x);
    dot.setY(y);
    dot.setColor(color);
    
    if (Table::getDotCountInRow(&dot, direction, m_model) != 3) {
        return false;
    }
    
    // Check both ends are open
    int openEnds = 0;
    for (const auto &pair : direction) {
        int dx = 0, dy = 0;
        switch (pair.first) {
            case '-': dx = -1; break;
            case '+': dx = 1; break;
        }
        switch (pair.second) {
            case '-': dy = -1; break;
            case '+': dy = 1; break;
        }
        
        // Find the end of the sequence
        int nx = x, ny = y;
        while (nx + dx >= 0 && nx + dx < BOARD_SIZE && ny + dy >= 0 && ny + dy < BOARD_SIZE 
               && m_model[ny + dy][nx + dx] == color) {
            nx += dx;
            ny += dy;
        }
        
        // Check if next position is empty
        nx += dx;
        ny += dy;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_model[ny][nx] == NONE) {
            openEnds++;
        }
    }
    
    return openEnds == 2;
}

bool SimpleAi::isOpenFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Pattern: _XXXX_ or XXXX_ (four in a row with at least one end open)
    Dot dot;
    dot.setX(x);
    dot.setY(y);
    dot.setColor(color);
    
    if (Table::getDotCountInRow(&dot, direction, m_model) != 4) {
        return false;
    }
    
    // Check if at least one end is open
    for (const auto &pair : direction) {
        int dx = 0, dy = 0;
        switch (pair.first) {
            case '-': dx = -1; break;
            case '+': dx = 1; break;
        }
        switch (pair.second) {
            case '-': dy = -1; break;
            case '+': dy = 1; break;
        }
        
        // Find the end of the sequence
        int nx = x, ny = y;
        while (nx + dx >= 0 && nx + dx < BOARD_SIZE && ny + dy >= 0 && ny + dy < BOARD_SIZE 
               && m_model[ny + dy][nx + dx] == color) {
            nx += dx;
            ny += dy;
        }
        
        // Check if next position is empty
        nx += dx;
        ny += dy;
        if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && m_model[ny][nx] == NONE) {
            return true;
        }
    }
    
    return false;
}

bool SimpleAi::blocksExistingFour(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Check if placing at (x,y) blocks an existing four in a row
    // This checks if there are already 4 opponent stones in a row with this position as an open end
    if (m_model[y][x] != NONE) {
        return false;
    }
    
    // For each direction (which has two pairs for opposite directions)
    // Count total stones in the line that passes through this position
    int totalCount = 0;
    
    for (const auto &pair : direction) {
        int dx = 0, dy = 0;
        switch (pair.first) {
            case '-': dx = -1; break;
            case '+': dx = 1; break;
        }
        switch (pair.second) {
            case '-': dy = -1; break;
            case '+': dy = 1; break;
        }
        
        // Count consecutive opponent stones in this direction
        int nx = x + dx, ny = y + dy;
        while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE 
               && m_model[ny][nx] == color) {
            totalCount++;
            nx += dx;
            ny += dy;
        }
    }
    
    // If total count across both directions is 4 or more, blocking here is critical
    return totalCount >= 4;
}

bool SimpleAi::isSplitThree(int x, int y, DOT_COLOR color, const Table::Direction &direction)
{
    // Pattern: XX_X or X_XX (three with one gap)
    for (const auto &pair : direction) {
        int dx = 0, dy = 0;
        switch (pair.first) {
            case '-': dx = -1; break;
            case '+': dx = 1; break;
        }
        switch (pair.second) {
            case '-': dy = -1; break;
            case '+': dy = 1; break;
        }
        
        int count = 0;
        int gaps = 0;
        int nx = x + dx, ny = y + dy;
        
        // Check pattern in one direction
        for (int i = 0; i < 4 && nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE; i++) {
            if (m_model[ny][nx] == color) {
                count++;
            } else if (m_model[ny][nx] == NONE) {
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
    
    // Check if this blocks an existing four - CRITICAL!
    if (blocksExistingFour(x, y, color, direction)) {
        score += 5000;  // Must block immediately!
    } else if (isOpenFour(x, y, color, direction)) {
        score += 3000;  // Critical: creates open four
    } else if (isOpenThree(x, y, color, direction)) {
        score += 500;   // Very dangerous
    } else if (isSplitThree(x, y, color, direction)) {
        score += 200;   // Dangerous
    }
    
    return score;
}

int SimpleAi::countPatterns(DOT_COLOR color)
{
    int totalScore = 0;
    
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_model[y][x] != NONE) {
                continue;
            }
            
            // Check if this position blocks an existing four FIRST (before placing)
            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (blocksExistingFour(x, y, color, direction)) {
                    totalScore += 5000;
                }
            }
            
            // Temporarily place the dot to check other patterns
            m_model[y][x] = color;
            
            for (const Table::Direction &direction : QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                if (isOpenFour(x, y, color, direction)) {
                    totalScore += 3000;
                } else if (isOpenThree(x, y, color, direction)) {
                    totalScore += 500;
                } else if (isSplitThree(x, y, color, direction)) {
                    totalScore += 200;
                }
            }
            
            // Remove the dot
            m_model[y][x] = NONE;
        }
    }
    
    return totalScore;
}

int SimpleAi::calculate()
{
    // calculate how much possibilities to increase continuous dots
    QMap<int, int> opMap;
    QMap<int, int> selfMap;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (m_model.at(y).at(x) != 0) {
                continue;
            }
            Dot dot;
            dot.setX(x);
            dot.setY(y);
            // opponent
            dot.setColor(m_color == WHITE ? BLACK : WHITE);
            int tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                tmp = std::max(tmp, Table::getDotCountInRow(&dot, direction, m_model));
            }
            opMap[tmp]++;
            // self
            dot.setColor(m_color);
            tmp = 0;
            for (const Table::Direction &direction: QList<Table::Direction> { Table::X, Table::Y, Table::XY, Table::YX }) {
                tmp = std::max(tmp, Table::getDotCountInRow(&dot, direction, m_model));
            }
            selfMap[tmp]++;
        }
    }
    int opScore = opMap.lastKey() * 1000 + opMap.last();
    int selfScore = selfMap.lastKey() * 1000 + selfMap.last();
    
    // Add danger pattern evaluation
    DOT_COLOR opColor = m_color == WHITE ? BLACK : WHITE;
    int opPatternScore = countPatterns(opColor);
    int selfPatternScore = countPatterns(m_color);
    
    // Prioritize blocking opponent's existing four (critical threat!)
    if (opPatternScore >= 5000) {
        return -MAX + 50;  // Must block existing four immediately!
    }
    
    // Prioritize blocking opponent's open four
    if (opPatternScore >= 3000) {
        return -MAX + 100;  // Must block!
    }
    
    if (opScore >= 5000) {
        return -MAX;
    }
    if (selfScore >= 5000) {
        return MAX;
    }
    
    // Add pattern scores to evaluation
    selfScore += selfPatternScore;
    opScore += opPatternScore;
    
    if (selfScore >= opScore) {
        return selfScore - opScore;
    }
    return opScore - selfScore;
}
