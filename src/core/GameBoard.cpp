#include "core/GameBoard.hpp"

#include <stdexcept>

namespace tetris {

GameBoard::GameBoard() {
    reset();
}

void GameBoard::reset() {
    for (auto& row : cells_) {
        row.fill(CellState::Empty);
    }
}

bool GameBoard::isInside(int x, int y) const {
    return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

CellState GameBoard::getCell(int x, int y) const {
    if (!isInside(x, y)) {
        throw std::out_of_range("GameBoard coordinates are outside the board");
    }

    return cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)];
}

void GameBoard::setCell(int x, int y, CellState value) {
    if (!isInside(x, y)) {
        throw std::out_of_range("GameBoard coordinates are outside the board");
    }

    cells_[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = value;
}

int GameBoard::width() const {
    return WIDTH;
}

int GameBoard::height() const {
    return HEIGHT;
}

}  // namespace tetris
