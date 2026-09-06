#pragma once

namespace tetris {

/// Owns the score for one game session.
class Scoring {
public:
    /// Restores the score to zero.
    void reset();

    /// Adds points for clearing `lineCount` rows in one lock operation.
    /// TODO(Gam): define and validate the scoring table for 1-4 rows.
    void addLines(int lineCount);

    /// Returns the current score without changing it.
    [[nodiscard]] int getScore() const;

private:
    int score_{0};
};

// TODO(Gam): implement the scoring system in src/features/scoring/.

}  // namespace tetris
