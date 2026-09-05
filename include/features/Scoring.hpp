#pragma once

namespace tetris {

class Scoring {
public:
    void reset();
    void addLines(int lineCount);
    [[nodiscard]] int getScore() const;
};

// TODO(Gam): implement the scoring system in src/features/scoring/.

}  // namespace tetris
