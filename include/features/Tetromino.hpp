#pragma once

#include "core/Types.hpp"

namespace tetris {

class Tetromino {
public:
    [[nodiscard]] ActivePiece createPiece();
    [[nodiscard]] ActivePiece getRotated(const ActivePiece& piece) const;
};

// TODO(Huy): implement Tetromino generation and rotation in
// src/features/tetromino/.

}  // namespace tetris
