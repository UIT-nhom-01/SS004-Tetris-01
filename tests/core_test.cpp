#include "core/Game.hpp"
#include "core/GameBoard.hpp"
#include "core/Input.hpp"
#include "core/Types.hpp"

#include <cassert>
#include <iostream>

namespace {

void testBoardDimensions() {
    const tetris::GameBoard board;
    assert(board.width() == 10);
    assert(board.height() == 20);
}

void testBoardCellsAndReset() {
    tetris::GameBoard board;
    board.setCell(0, 0, tetris::CellState::Filled);
    board.setCell(9, 19, tetris::CellState::Filled);

    assert(board.getCell(0, 0) == tetris::CellState::Filled);
    assert(board.getCell(9, 19) == tetris::CellState::Filled);

    board.reset();
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            assert(board.getCell(x, y) == tetris::CellState::Empty);
        }
    }
}

void testBoardBoundaries() {
    const tetris::GameBoard board;
    assert(board.isInside(0, 0));
    assert(board.isInside(9, 19));
    assert(!board.isInside(-1, 0));
    assert(!board.isInside(0, -1));
    assert(!board.isInside(10, 0));
    assert(!board.isInside(0, 20));
}

void testInputMapping() {
    assert(tetris::Input::fromCharacter('a') == tetris::InputAction::MoveLeft);
    assert(tetris::Input::fromCharacter('D') == tetris::InputAction::MoveRight);
    assert(tetris::Input::fromCharacter('s') == tetris::InputAction::MoveDown);
    assert(tetris::Input::fromCharacter('W') == tetris::InputAction::Rotate);
    assert(tetris::Input::fromCharacter('q') == tetris::InputAction::Quit);
    assert(tetris::Input::fromCharacter('x') == tetris::InputAction::None);
}

void testTemporaryPieceMovement() {
    tetris::Game game;
    assert(game.activePiece().blocks.size() == 4);

    assert(game.moveCurrentPiece(-4, 0));
    assert(game.activePiece().blocks[0].x == 0);
    assert(!game.moveCurrentPiece(-1, 0));
    assert(game.activePiece().blocks[0].x == 0);

    assert(game.moveCurrentPiece(8, 0));
    assert(game.activePiece().blocks[1].x == 9);
    assert(!game.moveCurrentPiece(1, 0));
    assert(game.activePiece().blocks[1].x == 9);

    assert(game.moveCurrentPiece(0, 18));
    assert(game.activePiece().blocks[3].y == 19);
    assert(!game.moveCurrentPiece(0, 1));
    assert(game.activePiece().blocks[3].y == 19);
}

}  // namespace

int main() {
    testBoardDimensions();
    testBoardCellsAndReset();
    testBoardBoundaries();
    testInputMapping();
    testTemporaryPieceMovement();

    std::cout << "All core tests passed.\n";
    return 0;
}
