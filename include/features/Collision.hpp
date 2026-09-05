#pragma once

#include "core/GameBoard.hpp"
#include "core/Types.hpp"

namespace tetris {

class Collision {
public:
    [[nodiscard]] bool canPlace(
        const GameBoard& board,
        const ActivePiece& piece) const;

    int clearCompletedLines(GameBoard& board);
};

// TODO(Tu): implement collision and line clearing in
// src/features/collision/.

}  // namespace tetris
