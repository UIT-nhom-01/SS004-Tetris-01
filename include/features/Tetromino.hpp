#pragma once

#include "core/Types.hpp"

namespace tetris {

/// Creates Tetromino shapes and rotation candidates without accessing the board.
class Tetromino {
public:
    /// Creates a random piece at its spawn position.
    [[nodiscard]] ActivePiece createPiece();

    /// Creates a selected type at its spawn position for tests and previews.
    [[nodiscard]] ActivePiece createPiece(TetrominoType type) const;

    /// Returns a clockwise-rotated candidate without mutating the input piece.
    /// Game asks Collision whether the candidate is valid before applying it.
    [[nodiscard]] ActivePiece getRotated(const ActivePiece& piece) const;
};

// TODO(Huy): implement Tetromino generation and rotation in
// src/features/tetromino/.

}  // namespace tetris
