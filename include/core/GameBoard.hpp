#pragma once

#include <array>

#include "core/Types.hpp"

namespace tetris {

class GameBoard {
public:
    static constexpr int WIDTH = 10;
    static constexpr int HEIGHT = 20;

    GameBoard();

    void reset();

    [[nodiscard]] bool isInside(int x, int y) const;
    [[nodiscard]] CellState getCell(int x, int y) const;
    void setCell(int x, int y, CellState value);

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

private:
    using Row = std::array<CellState, WIDTH>;
    std::array<Row, HEIGHT> cells_{};
};

}  // namespace tetris
