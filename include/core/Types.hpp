#pragma once

#include <array>

namespace tetris {

/// Zero-based absolute coordinate on the game board.
struct Position {
    int x{0};
    int y{0};
};

[[nodiscard]] constexpr bool operator==(Position lhs, Position rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

[[nodiscard]] constexpr bool operator!=(Position lhs, Position rhs) {
    return !(lhs == rhs);
}

/// Logical player action, independent of the physical input device.
enum class InputAction {
    None,
    MoveLeft,
    MoveRight,
    MoveDown,
    Rotate,
    Restart,
    Quit
};

/// Identity of one of the seven standard Tetromino shapes.
enum class TetrominoType {
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

/// Stored state of a locked board cell.
///
/// Occupied cells retain their Tetromino type so the renderer can preserve the
/// piece color after it has been locked into the board.
enum class CellState {
    Empty,
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

/// Returns whether a board cell contains a locked Tetromino block.
[[nodiscard]] constexpr bool isOccupied(CellState state) {
    return state != CellState::Empty;
}

/// Converts a Tetromino identity into the value stored by GameBoard.
[[nodiscard]] constexpr CellState cellStateFor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I:
            return CellState::I;
        case TetrominoType::O:
            return CellState::O;
        case TetrominoType::T:
            return CellState::T;
        case TetrominoType::S:
            return CellState::S;
        case TetrominoType::Z:
            return CellState::Z;
        case TetrominoType::J:
            return CellState::J;
        case TetrominoType::L:
            return CellState::L;
    }

    return CellState::Empty;
}

/// Clockwise orientation relative to a piece's spawn orientation.
enum class RotationState {
    Spawn,
    Right,
    Reverse,
    Left
};

/// Shared representation of the currently falling Tetromino.
///
/// `origin` is the rotation reference point and every entry in `blocks` is an
/// absolute board coordinate. Tetromino owns shape/rotation rules; core and
/// Collision consume the generated representation.
struct ActivePiece {
    TetrominoType type{TetrominoType::O};
    RotationState rotation{RotationState::Spawn};
    Position origin{};
    std::array<Position, 4> blocks{};
};

/// Returns a translated copy while leaving the source piece unchanged.
[[nodiscard]] inline ActivePiece translated(
    const ActivePiece& piece,
    int dx,
    int dy) {
    ActivePiece result = piece;
    result.origin.x += dx;
    result.origin.y += dy;

    for (Position& block : result.blocks) {
        block.x += dx;
        block.y += dy;
    }

    return result;
}

}  // namespace tetris
