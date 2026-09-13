#include "core/Game.hpp"
#include "core/GameBoard.hpp"
#include "core/Types.hpp"
#include "features/Collision.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

constexpr int MAX_TICKS_UNTIL_GAME_OVER = 1000;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

bool piecesEqual(
    const tetris::ActivePiece& lhs,
    const tetris::ActivePiece& rhs) {
    return lhs.type == rhs.type && lhs.rotation == rhs.rotation &&
        lhs.origin == rhs.origin && lhs.blocks == rhs.blocks;
}

int occupiedCellCount(const tetris::GameBoard& board) {
    int count = 0;
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            if (tetris::isOccupied(board.getCell(x, y))) {
                ++count;
            }
        }
    }
    return count;
}

tetris::ActivePiece findLandingPiece(
    const tetris::GameBoard& board,
    const tetris::ActivePiece& spawnPiece,
    const tetris::Collision& collision) {
    tetris::ActivePiece landingPiece = spawnPiece;
    while (collision.canPlace(board, tetris::translated(landingPiece, 0, 1))) {
        landingPiece = tetris::translated(landingPiece, 0, 1);
    }
    return landingPiece;
}

void testSpawnMoveRotateFallLockAndPromote() {
    tetris::Game game;
    tetris::Collision collision;

    expect(collision.canPlace(game.board(), game.activePiece()),
           "the generated active piece must spawn on the empty board");
    expect(game.activePiece().rotation == tetris::RotationState::Spawn,
           "the active piece must start in spawn orientation");
    expect(game.nextPiece().rotation == tetris::RotationState::Spawn,
           "the preview piece must start in spawn orientation");

    expect(game.moveCurrentPiece(-1, 0),
           "the active piece must accept valid horizontal movement");
    expect(game.moveCurrentPiece(1, 0),
           "the active piece must move back after horizontal movement");
    expect(game.rotateCurrentPiece(),
           "the active piece must accept a valid clockwise rotation");

    const tetris::ActivePiece expectedNext = game.nextPiece();
    for (int tick = 0;
         tick < tetris::GameBoard::HEIGHT && occupiedCellCount(game.board()) == 0;
         ++tick) {
        expect(game.tick(), "gravity must advance or lock a running piece");
    }

    expect(occupiedCellCount(game.board()) == 4,
           "the falling piece must lock exactly four cells on an empty board");
    expect(piecesEqual(game.activePiece(), expectedNext),
           "the preview piece must become active after locking");
    expect(game.score() == 0,
           "locking without a completed line must not change score");
    expect(!game.isGameOver(),
           "the next piece must continue the session on an empty spawn area");
}

void testLockLineClearAndScoreFlow() {
    tetris::Game game;
    tetris::Collision collision;
    auto& board = const_cast<tetris::GameBoard&>(game.board());

    const tetris::ActivePiece landingPiece =
        findLandingPiece(board, game.activePiece(), collision);
    bool landingColumns[tetris::GameBoard::WIDTH]{};
    int blocksInClearedRow = 0;
    for (const tetris::Position& block : landingPiece.blocks) {
        if (block.y == board.height() - 1) {
            landingColumns[block.x] = true;
            ++blocksInClearedRow;
        }
    }

    for (int x = 0; x < board.width(); ++x) {
        if (!landingColumns[x]) {
            board.setCell(x, board.height() - 1, tetris::CellState::J);
        }
    }

    const tetris::ActivePiece expectedNext = game.nextPiece();
    const int landingDistance =
        landingPiece.origin.y - game.activePiece().origin.y;
    expect(game.moveCurrentPiece(0, landingDistance),
           "the active piece must reach its prepared landing position");
    expect(game.tick(),
           "a blocked fall must lock, clear, score, and promote the next piece");

    expect(game.score() == 100,
           "clearing one line through Game must award 100 points");
    expect(occupiedCellCount(board) == 4 - blocksInClearedRow,
           "line clearing must remove the completed row and shift remaining blocks");
    expect(piecesEqual(game.activePiece(), expectedNext),
           "line clearing must finish before the preview piece is promoted");
}

void testGameOverStopsFlowAndRestartResetsSession() {
    tetris::Game game;
    tetris::Collision collision;

    for (int tick = 0;
         tick < MAX_TICKS_UNTIL_GAME_OVER && !game.isGameOver();
         ++tick) {
        game.tick();
    }

    expect(game.isGameOver(),
           "stacking pieces in the spawn columns must eventually cause Game Over");
    const int occupiedBeforeStoppedTick = occupiedCellCount(game.board());
    expect(!game.tick(), "gravity must stop after Game Over");
    expect(occupiedCellCount(game.board()) == occupiedBeforeStoppedTick,
           "the board must not change after Game Over");

    game.restart();

    expect(!game.isGameOver(), "restart must resume gameplay");
    expect(game.score() == 0, "restart must reset score");
    expect(occupiedCellCount(game.board()) == 0,
           "restart must clear every locked block");
    expect(collision.canPlace(game.board(), game.activePiece()),
           "restart must generate a placeable active piece");
    expect(game.tick(), "gravity must resume in the restarted session");
}

}  // namespace

int main() {
    try {
        testSpawnMoveRotateFallLockAndPromote();
        testLockLineClearAndScoreFlow();
        testGameOverStopsFlowAndRestartResetsSession();

        std::cout << "integration_test: all passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "integration_test failed: " << error.what() << '\n';
        return 1;
    }
}
