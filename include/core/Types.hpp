#pragma once

#include <vector>

namespace tetris {

struct Position {
    int x;
    int y;
};

enum class CellState {
    Empty,
    Filled
};

enum class InputAction {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    Rotate,
    Quit
};

// Shared representation only. Tetromino shapes and behavior belong to Huy's
// feature module.
struct ActivePiece {
    std::vector<Position> blocks;
};

}  // namespace tetris
