#include "features/Scoring.hpp"

#include <algorithm>

namespace tetris {

namespace {

int pointsForLines(int lineCount) {
    switch (lineCount) {
        case 1:
            return 100;
        case 2:
            return 300;
        case 3:
            return 500;
        default:
            return 800;  // Tetris: 4+ rows in one lock.
    }
}

}  // namespace

void Scoring::reset() {
    score_ = 0;
    level_ = 1;
    totalLines_ = 0;
}

void Scoring::addLines(int lineCount) {
    if (lineCount <= 0) {
        return;
    }
    score_ += pointsForLines(lineCount) * level_;
    totalLines_ += lineCount;
    level_ = 1 + totalLines_ / LINES_PER_LEVEL;
}

void Scoring::setLevel(int level) {
    level_ = std::max(1, level);
}

int Scoring::getScore() const {
    return score_;
}

int Scoring::getLevel() const {
    return level_;
}

int Scoring::getTotalLines() const {
    return totalLines_;
}

int Scoring::getDropIntervalMs() const {
    const int interval = BASE_DROP_INTERVAL_MS - (level_ - 1) * DROP_STEP_MS;
    return std::max(MIN_DROP_INTERVAL_MS, interval);
}

}  // namespace tetris
