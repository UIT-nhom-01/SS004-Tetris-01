#pragma once

namespace tetris {

/// Owns the score and level for one game session.
///
/// Points follow the guideline table (100/300/500/800 for a
/// single/double/triple/Tetris) multiplied by the level reached before the
/// rows are counted. Every LINES_PER_LEVEL cleared rows raise the level by
/// one, which shortens the gravity interval from getDropIntervalMs().
class Scoring {
public:
    /// Cleared rows required to advance one level.
    static constexpr int LINES_PER_LEVEL = 10;
    /// Gravity interval in milliseconds at level 1.
    static constexpr int BASE_DROP_INTERVAL_MS = 500;
    /// Interval shrink per level in milliseconds.
    static constexpr int DROP_STEP_MS = 40;
    /// Fastest allowed gravity interval in milliseconds.
    static constexpr int MIN_DROP_INTERVAL_MS = 50;

    /// Resets score, level, and cleared-row count for a new or restarted session.
    void reset();

    /// Adds points for clearing `lineCount` rows in one lock operation.
    /// Single/double/triple/Tetris awards 100/300/500/800 points multiplied
    /// by the level reached before these rows are counted.
    /// Throws std::invalid_argument when `lineCount` is outside 1-4.
    /// A zero count is accepted and changes nothing.
    void addLines(int lineCount);

    /// Overrides the derived level (clamped to 1 or higher).
    void setLevel(int level);

    /// Returns the current score without changing it.
    [[nodiscard]] int getScore() const;

    /// Returns the current 1-based level.
    [[nodiscard]] int getLevel() const;

    /// Returns how many rows have been counted so far.
    [[nodiscard]] int getTotalLines() const;

    /// Returns the gravity interval matching the current level.
    [[nodiscard]] int getDropIntervalMs() const;

private:
    int score_{0};
    int level_{1};
    int totalLines_{0};
};

}  // namespace tetris
