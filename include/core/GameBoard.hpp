#pragma once

#include <array>

#include "core/Types.hpp"

namespace tetris {

/// Fixed-size grid containing only blocks that have already been locked.
/// The falling ActivePiece is stored separately by Game.
class GameBoard {
public:
    static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 20;

    GameBoard();

    /// Clears every cell and restores the initial empty board.
    void reset();

    /// Returns whether `(x, y)` is a valid board coordinate.
    [[nodiscard]] bool isInside(int x, int y) const;

    /// Reads a cell and throws std::out_of_range for invalid coordinates.
    [[nodiscard]] CellState getCell(int x, int y) const;

    /// Writes a cell and throws std::out_of_range for invalid coordinates.
    void setCell(int x, int y, CellState value);

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

private:
    using Row = std::array<CellState, WIDTH>;
    std::array<Row, HEIGHT> cells_{};
};

}  // namespace tetris
