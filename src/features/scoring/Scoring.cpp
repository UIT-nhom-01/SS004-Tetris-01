#include "features/Scoring.hpp"

#include <algorithm>
#include <stdexcept>

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
        case 4:
            return 800;
        default:
            throw std::invalid_argument("lineCount must be between 1 and 4");
    }
}

}  // namespace

// Reset the score and level for a new game session.
void Scoring::reset() {
    score_ = 0;
    level_ = 1;
    totalLines_ = 0;
}

// Add points according to the number of lines cleared by one piece lock,
// multiplied by the level reached before these rows are counted.
void Scoring::addLines(int lineCount) {
    if (lineCount == 0) {
        return;
    }
    score_ += pointsForLines(lineCount) * level_;
    totalLines_ += lineCount;
    level_ = 1 + totalLines_ / LINES_PER_LEVEL;
}

void Scoring::setLevel(int level) {
    level_ = std::max(1, level);
}

// Return the current score without changing it.
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
