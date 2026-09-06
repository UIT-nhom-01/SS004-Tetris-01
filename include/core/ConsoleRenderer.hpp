#pragma once

#include <string>

#include "core/GameBoard.hpp"
#include "core/Types.hpp"

namespace tetris {

/// Builds complete terminal frames without changing game state.
class ConsoleRenderer {
public:
    /// Returns a board-and-sidebar frame ready to be written to the terminal.
    /// ANSI colors are omitted when `useColor` is false.
    [[nodiscard]] std::string buildFrame(
        const GameBoard& board,
        const ActivePiece& activePiece,
        const ActivePiece& nextPiece,
        int score,
        bool gameOver,
        bool useColor) const;
};

}  // namespace tetris
