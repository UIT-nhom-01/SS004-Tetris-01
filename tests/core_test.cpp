#include "core/ConsoleRenderer.hpp"
#include "core/Game.hpp"
#include "core/GameBoard.hpp"
#include "core/Input.hpp"
#include "core/Types.hpp"
#include "features/Collision.hpp"
#include "features/GameState.hpp"
#include "features/Scoring.hpp"
#include "features/Tetromino.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void testBoardDimensions() {
    const tetris::GameBoard board;
    expect(board.width() == 10, "board width must be 10");
    expect(board.height() == 20, "board height must be 20");
}

void testBoardCellsAndReset() {
    tetris::GameBoard board;
    board.setCell(0, 0, tetris::CellState::T);
    board.setCell(9, 19, tetris::CellState::L);

    expect(
        board.getCell(0, 0) == tetris::CellState::T,
        "top-left cell must retain its Tetromino type");
    expect(
        board.getCell(9, 19) == tetris::CellState::L,
        "bottom-right cell must retain its Tetromino type");

    board.reset();
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            expect(
                board.getCell(x, y) == tetris::CellState::Empty,
                "reset must empty every cell");
        }
    }
}

void testCellStateContract() {
    expect(
        !tetris::isOccupied(tetris::CellState::Empty),
        "an empty cell must not be occupied");
    expect(
        tetris::isOccupied(tetris::CellState::Z),
        "a typed Tetromino cell must be occupied");
    expect(
        tetris::cellStateFor(tetris::TetrominoType::I) ==
            tetris::CellState::I,
        "Tetromino type must map to the matching board state");
    expect(
        tetris::cellStateFor(tetris::TetrominoType::L) ==
            tetris::CellState::L,
        "every Tetromino type must have a storable board state");
}

void testBoardBoundaries() {
    const tetris::GameBoard board;
    expect(board.isInside(0, 0), "top-left position must be inside");
    expect(board.isInside(9, 19), "bottom-right position must be inside");
    expect(!board.isInside(-1, 0), "negative x must be outside");
    expect(!board.isInside(0, -1), "negative y must be outside");
    expect(!board.isInside(10, 0), "x at width must be outside");
    expect(!board.isInside(0, 20), "y at height must be outside");

    bool threw = false;
    try {
        static_cast<void>(board.getCell(-1, 0));
    } catch (const std::out_of_range&) {
        threw = true;
    }
    expect(threw, "reading outside the board must throw");
}

void testInputMapping() {
    expect(
        tetris::Input::fromCharacter('a') == tetris::InputAction::MoveLeft,
        "A must map to MoveLeft");
    expect(
        tetris::Input::fromCharacter('D') == tetris::InputAction::MoveRight,
        "D must map to MoveRight");
    expect(
        tetris::Input::fromCharacter('s') == tetris::InputAction::MoveDown,
        "S must map to MoveDown");
    expect(
        tetris::Input::fromCharacter('W') == tetris::InputAction::Rotate,
        "W must map to Rotate");
    expect(
        tetris::Input::fromCharacter('R') == tetris::InputAction::Restart,
        "R must map to Restart");
    expect(
        tetris::Input::fromCharacter('q') == tetris::InputAction::Quit,
        "Q must map to Quit");
    expect(
        tetris::Input::fromCharacter('x') == tetris::InputAction::None,
        "unknown keys must map to None");
}

void testSharedPieceModel() {
    const tetris::ActivePiece piece{
        tetris::TetrominoType::T,
        tetris::RotationState::Spawn,
        {4, 1},
        {{{3, 1}, {4, 1}, {5, 1}, {4, 2}}}};

    const tetris::ActivePiece moved = tetris::translated(piece, 2, 3);
    expect(piece.origin == tetris::Position{4, 1}, "translation must not mutate input");
    expect(moved.origin == tetris::Position{6, 4}, "translation must move origin");
    expect(
        moved.blocks[0] == tetris::Position{5, 4},
        "translation must move every block");
    expect(moved.type == piece.type, "translation must preserve piece type");
    expect(
        moved.rotation == piece.rotation,
        "translation must preserve rotation state");
}

void testTemporaryPieceMovement() {
    tetris::Game game;
    expect(game.activePiece().blocks.size() == 4, "a piece must contain four blocks");

    expect(game.moveCurrentPiece(-4, 0), "piece must move to the left edge");
    expect(game.activePiece().blocks[0].x == 0, "piece must reach x=0");
    expect(!game.moveCurrentPiece(-1, 0), "piece must not cross the left edge");
    expect(game.activePiece().blocks[0].x == 0, "rejected move must be atomic");

    expect(game.moveCurrentPiece(8, 0), "piece must move to the right edge");
    expect(game.activePiece().blocks[1].x == 9, "piece must reach x=9");
    expect(!game.moveCurrentPiece(1, 0), "piece must not cross the right edge");
    expect(game.activePiece().blocks[1].x == 9, "rejected move must be atomic");

    expect(game.moveCurrentPiece(0, 18), "piece must move to the bottom edge");
    expect(game.activePiece().blocks[3].y == 19, "piece must reach y=19");
    expect(!game.moveCurrentPiece(0, 1), "piece must not cross the bottom edge");
    expect(game.activePiece().blocks[3].y == 19, "rejected move must be atomic");
}

void testTickAndRestart() {
    tetris::Game game;
    const tetris::Position initialOrigin = game.activePiece().origin;

    expect(game.tick(), "gravity tick must move a placeable piece down");
    expect(
        game.activePiece().origin ==
            tetris::Position{initialOrigin.x, initialOrigin.y + 1},
        "gravity tick must update piece origin");

    expect(game.moveCurrentPiece(-2, 3), "test setup move must succeed");
    game.restart();
    expect(
        game.activePiece().origin == initialOrigin,
        "restart must restore the spawn origin");
    expect(
        game.activePiece().type == tetris::TetrominoType::O,
        "baseline restart must restore the temporary piece");
    expect(
        game.nextPiece().type == tetris::TetrominoType::T,
        "baseline restart must restore the temporary preview piece");
}

void testConsoleRendererLayoutAndColors() {
    tetris::GameBoard board;
    board.setCell(0, 19, tetris::CellState::I);
    board.setCell(1, 19, tetris::CellState::O);
    board.setCell(2, 19, tetris::CellState::T);
    board.setCell(3, 19, tetris::CellState::S);
    board.setCell(4, 19, tetris::CellState::Z);
    board.setCell(5, 19, tetris::CellState::J);
    board.setCell(6, 19, tetris::CellState::L);

    const tetris::ActivePiece activePiece{
        tetris::TetrominoType::O,
        tetris::RotationState::Spawn,
        {4, 0},
        {{{4, 0}, {5, 0}, {4, 1}, {5, 1}}}};
    const tetris::ActivePiece nextPiece{
        tetris::TetrominoType::T,
        tetris::RotationState::Spawn,
        {4, 1},
        {{{3, 1}, {4, 1}, {5, 1}, {4, 2}}}};

    const tetris::ConsoleRenderer renderer;
    const std::string plainFrame = renderer.buildFrame(
        board, activePiece, nextPiece, 1250, false, false);

    expect(
        std::count(plainFrame.begin(), plainFrame.end(), '\n') == 22,
        "a frame must match the board's 20 rows plus two borders");
    expect(
        plainFrame.find('\n') == 59,
        "board and side panel must have a stable one-line width");
    expect(
        plainFrame.find("SCORE") != std::string::npos &&
            plainFrame.find("1250") != std::string::npos,
        "side panel must show the provided score");
    expect(
        plainFrame.find("NEXT PIECE") != std::string::npos &&
            plainFrame.find("CONTROLS") != std::string::npos,
        "side panel must include preview and controls sections");
    expect(
        plainFrame.find("\x1B[") == std::string::npos,
        "plain rendering must not contain ANSI escape codes");

    const std::string coloredFrame = renderer.buildFrame(
        board, activePiece, nextPiece, 1250, true, true);
    expect(
        coloredFrame.find("\x1B[46m") != std::string::npos,
        "the I piece must use the cyan ANSI background");
    expect(
        coloredFrame.find("\x1B[43m") != std::string::npos,
        "the O piece must use the yellow ANSI background");
    expect(
        coloredFrame.find("\x1B[45m") != std::string::npos,
        "the T preview must use the magenta ANSI background");
    expect(
        coloredFrame.find("\x1B[42m") != std::string::npos,
        "the S piece must use the green ANSI background");
    expect(
        coloredFrame.find("\x1B[41m") != std::string::npos,
        "the Z piece must use the red ANSI background");
    expect(
        coloredFrame.find("\x1B[44m") != std::string::npos,
        "a locked J block must retain its blue ANSI background");
    expect(
        coloredFrame.find("\x1B[48;5;208m") != std::string::npos,
        "the L piece must use the orange ANSI background");
    expect(
        coloredFrame.find("GAME OVER") != std::string::npos,
        "the side panel must support the future Game Over state");
}

void testFeatureHeadersCompileAsContracts() {
    static_assert(std::is_default_constructible_v<tetris::Tetromino>);
    static_assert(std::is_default_constructible_v<tetris::Collision>);
    static_assert(std::is_default_constructible_v<tetris::Scoring>);
    static_assert(std::is_default_constructible_v<tetris::GameState>);
}

}  // namespace

int main() {
    try {
        testBoardDimensions();
        testBoardCellsAndReset();
        testCellStateContract();
        testBoardBoundaries();
        testInputMapping();
        testSharedPieceModel();
        testTemporaryPieceMovement();
        testTickAndRestart();
        testConsoleRendererLayoutAndColors();
        testFeatureHeadersCompileAsContracts();
    } catch (const std::exception& error) {
        std::cerr << "Core test failed: " << error.what() << '\n';
        return 1;
    }

    std::cout << "All core tests passed.\n";
    return 0;
}
