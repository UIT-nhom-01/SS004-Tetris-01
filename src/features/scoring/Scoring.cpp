#include "features/Scoring.hpp"

#include <stdexcept>

namespace tetris {

// Reset the score for a new game session.
void Scoring::reset() {
    score_ = 0;
}

// Add points according to the number of lines cleared by one piece lock.
void Scoring::addLines(int lineCount) {
    switch (lineCount) {
        case 0:
            break;
        case 1:
            score_ += 100;
            break;
        case 2:
            score_ += 300;
            break;
        case 3:
            score_ += 500;
            break;
        case 4:
            score_ += 800;
            break;
        default:
            throw std::invalid_argument("lineCount must be between 0 and 4");
    }
}

// Return the current score without changing it.
int Scoring::getScore() const {
    return score_;
}

}  // namespace tetris
