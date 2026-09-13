#pragma once

namespace tetris {

/// Owns the score for one game session.
class Scoring {
public:
    /// Clears the accumulated score when restarting the game.
    void reset();

    /// Adds points for clearing `lineCount` rows in one lock operation.
    /// Throws std::invalid_argument when `lineCount` is outside 0-4.
    void addLines(int lineCount);

    /// Returns the current score without changing it.
    [[nodiscard]] int getScore() const;

private:
    int score_{0};
};

}  // namespace tetris
