#pragma once

#include "core/GameBoard.hpp"
#include "core/Types.hpp"

namespace tetris {

class GameState {
public:
    [[nodiscard]] bool isGameOver(
        const GameBoard& board,
        const ActivePiece& piece) const;

    void reset();
};

// TODO(Khanh): implement Game Over and Restart state in
// src/features/game_state/.

}  // namespace tetris
