#include "core/Game.hpp"
#include "core/GameBoard.hpp"
#include "core/Types.hpp"
#include "features/Collision.hpp"
#include "features/Tetromino.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}

int maxBlockY(const tetris::ActivePiece& piece) {
    return std::max_element(
               piece.blocks.begin(),
               piece.blocks.end(),
               [](const auto& lhs, const auto& rhs) { return lhs.y < rhs.y; })
        ->y;
}

bool boardHasLockedBlocks(const tetris::GameBoard& board) {
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            if (tetris::isOccupied(board.getCell(x, y))) {
                return true;
            }
        }
    }
    return false;
}

void testCanPlaceOnEmptyBoard() {
    tetris::Tetromino factory;
    tetris::Collision collision;
    tetris::GameBoard board;

    const tetris::TetrominoType types[] = {
        tetris::TetrominoType::I,
        tetris::TetrominoType::O,
        tetris::TetrominoType::T,
        tetris::TetrominoType::S,
        tetris::TetrominoType::Z,
        tetris::TetrominoType::J,
        tetris::TetrominoType::L,
    };

    for (auto type : types) {
        expect(collision.canPlace(board, factory.createPiece(type)),
               "spawn piece must fit an empty board");
    }
}

void testWallAndFloorCollisionRejected() {
    tetris::Tetromino factory;
    tetris::Collision collision;
    tetris::GameBoard board;
    const auto piece = factory.createPiece(tetris::TetrominoType::I);

    expect(!collision.canPlace(board, translated(piece, -4, 0)),
           "piece must collide with the left wall");
    expect(!collision.canPlace(board, translated(piece, 4, 0)),
           "piece must collide with the right wall");
    expect(!collision.canPlace(board, translated(piece, 0, 19)),
           "piece must collide with the floor");
    expect(!collision.canPlace(board, translated(piece, 0, -2)),
           "piece must collide with the ceiling");

    expect(collision.canPlace(board, translated(piece, -3, 0)),
           "piece flush with the left wall is valid");
    expect(collision.canPlace(board, translated(piece, 3, 0)),
           "piece flush with the right wall is valid");
    expect(collision.canPlace(board, translated(piece, 0, 18)),
           "piece resting on the floor is valid");
}

void testBlockCollisionRejected() {
    tetris::Tetromino factory;
    tetris::Collision collision;
    tetris::GameBoard board;
    board.setCell(4, 2, tetris::CellState::I);

    const auto piece = factory.createPiece(tetris::TetrominoType::T);
    expect(!collision.canPlace(board, piece),
           "piece overlapping an occupied cell must be rejected");
    expect(collision.canPlace(board, translated(piece, -1, 0)),
           "piece beside the occupied cell must be accepted");
}

void testLockPieceStoresTypeAndColor() {
    tetris::Tetromino factory;
    tetris::Collision collision;
    tetris::GameBoard board;

    const auto piece = translated(
        factory.createPiece(tetris::TetrominoType::J), 0, 18);
    collision.lockPiece(board, piece);

    int occupiedCells = 0;
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            if (tetris::isOccupied(board.getCell(x, y))) {
                ++occupiedCells;
                expect(board.getCell(x, y) == tetris::cellStateFor(piece.type),
                       "locked cell must store the piece type");
            }
        }
    }
    expect(occupiedCells == 4, "locking must write exactly four cells");
}

void testLockedPieceCompletesAndClearsLine() {
    tetris::Tetromino factory;
    tetris::Collision collision;
    tetris::GameBoard board;

    for (int x = 0; x < 8; ++x) {
        board.setCell(x, 19, tetris::CellState::J);
    }

    const auto piece = translated(
        factory.createPiece(tetris::TetrominoType::O), 4, 18);
    collision.lockPiece(board, piece);

    for (int x = 0; x < board.width(); ++x) {
        expect(tetris::isOccupied(board.getCell(x, 19)),
               "locking must complete the bottom row");
    }

    const int clearedLines = collision.clearCompletedLines(board);
    expect(clearedLines == 1,
           "completed row after locking must return one cleared line");
    expect(board.getCell(8, 19) == tetris::CellState::O &&
               board.getCell(9, 19) == tetris::CellState::O,
           "blocks above the cleared row must shift down");
    for (int x = 0; x < 8; ++x) {
        expect(board.getCell(x, 19) == tetris::CellState::Empty,
               "cleared cells must be replaced by the row above");
    }
}

void testClearWithoutFullLinesKeepsBoard() {
    tetris::Collision collision;
    tetris::GameBoard board;
    board.setCell(0, 19, tetris::CellState::Z);
    board.setCell(9, 15, tetris::CellState::T);

    expect(collision.clearCompletedLines(board) == 0,
           "no full row must clear nothing");
    expect(board.getCell(0, 19) == tetris::CellState::Z,
           "partial row must stay untouched");
    expect(board.getCell(9, 15) == tetris::CellState::T,
           "isolated block must stay untouched");
}

void testNoLineClearPreservesEveryCell() {
    tetris::Collision collision;
    tetris::GameBoard board;
    for (int y = 0; y < board.height(); ++y) {
        board.setCell(y % board.width(), y, tetris::CellState::S);
        board.setCell((y + 3) % board.width(), y, tetris::CellState::L);
    }
    const tetris::GameBoard before = board;

    expect(collision.clearCompletedLines(board) == 0,
           "partial rows must not be cleared");
    for (int y = 0; y < board.height(); ++y) {
        for (int x = 0; x < board.width(); ++x) {
            expect(board.getCell(x, y) == before.getCell(x, y),
                   "a no-op line clear must preserve every typed cell");
        }
    }
}

void testClearSingleLineShiftsRowsDown() {
    tetris::Collision collision;
    tetris::GameBoard board;

    for (int x = 0; x < board.width(); ++x) {
        board.setCell(x, 19, tetris::CellState::J);
    }
    board.setCell(0, 18, tetris::CellState::Z);

    expect(collision.clearCompletedLines(board) == 1,
           "one full row must be cleared");
    expect(board.getCell(0, 19) == tetris::CellState::Z,
           "row above must fall down and keep its color");
    expect(board.getCell(5, 19) == tetris::CellState::Empty,
           "rest of the fallen row must be empty");
    expect(board.getCell(0, 18) == tetris::CellState::Empty,
           "row above the cleared row must become empty");
}

void testClearMultipleConsecutiveLines() {
    tetris::Collision collision;
    tetris::GameBoard board;

    for (int x = 0; x < board.width(); ++x) {
        board.setCell(x, 16, tetris::CellState::J);
        board.setCell(x, 17, tetris::CellState::S);
        board.setCell(x, 18, tetris::CellState::T);
        board.setCell(x, 19, tetris::CellState::Z);
    }
    board.setCell(0, 15, tetris::CellState::I);
    board.setCell(9, 15, tetris::CellState::L);

    expect(collision.clearCompletedLines(board) == 4,
           "four consecutive full rows must be cleared");
    expect(board.getCell(0, 19) == tetris::CellState::I &&
               board.getCell(9, 19) == tetris::CellState::L,
           "surviving blocks must keep their colors after falling");
    expect(board.getCell(1, 19) == tetris::CellState::Empty,
           "fallen row must be empty beside surviving blocks");
    for (int y = 15; y <= 18; ++y) {
        for (int x = 0; x < board.width(); ++x) {
            expect(board.getCell(x, y) == tetris::CellState::Empty,
                   "cleared region must be empty");
        }
    }
}

void testClearTopLineEmptiesReplacementCells() {
    tetris::Collision collision;
    tetris::GameBoard board;

    for (int x = 0; x < board.width(); ++x) {
        board.setCell(x, 0, tetris::CellState::L);
    }
    board.setCell(0, 1, tetris::CellState::T);

    expect(collision.clearCompletedLines(board) == 1,
           "top row must be cleared");
    expect(board.getCell(0, 1) == tetris::CellState::T,
           "row below the cleared top row must not shift");
    expect(board.getCell(5, 0) == tetris::CellState::Empty,
           "replacement cells with nothing above must be empty");
}

void testGameTickLocksPieceAndPromotesNextPiece() {
    tetris::Game game;
    const tetris::ActivePiece initialPiece = game.activePiece();
    const tetris::ActivePiece expectedNext = game.nextPiece();
    tetris::ActivePiece lockedPiece = initialPiece;
    bool pieceWasLocked = false;

    for (int tick = 0; tick < tetris::GameBoard::HEIGHT; ++tick) {
        const tetris::ActivePiece pieceBeforeTick = game.activePiece();
        game.tick();
        if (boardHasLockedBlocks(game.board())) {
            lockedPiece = pieceBeforeTick;
            pieceWasLocked = true;
            break;
        }
    }

    expect(pieceWasLocked, "gravity must eventually lock the active piece");
    expect(game.activePiece().type == expectedNext.type,
           "blocked piece must switch to the next piece");
    expect(game.activePiece().rotation == expectedNext.rotation,
           "promoted piece must preserve its rotation");
    expect(game.activePiece().origin == expectedNext.origin,
           "promoted piece must preserve its origin");
    expect(game.activePiece().blocks == expectedNext.blocks,
           "promoted piece must preserve its blocks");

    for (const tetris::Position& block : lockedPiece.blocks) {
        expect(game.board().getCell(block.x, block.y) ==
                   tetris::cellStateFor(initialPiece.type),
               "previous piece must lock before the next piece is promoted");
    }
}

void testGameMovementRejectsBlockedCandidates() {
    tetris::Game game;
    const int distanceToFloor =
        tetris::GameBoard::HEIGHT - 1 - maxBlockY(game.activePiece());
    expect(game.moveCurrentPiece(0, distanceToFloor),
           "piece must move to the bottom for test setup");

    const auto beforeBlocks = game.activePiece().blocks;
    expect(!game.moveCurrentPiece(0, 1),
           "piece must not move below the floor");
    expect(game.activePiece().blocks == beforeBlocks,
           "rejected candidate must not mutate the piece");
}

void testGameRotationAppliesTetrominoCandidate() {
    tetris::Game game;
    tetris::Tetromino tetromino;
    const tetris::ActivePiece original = game.activePiece();
    const tetris::ActivePiece expected = tetromino.getRotated(original);

    expect(game.rotateCurrentPiece(),
           "rotation input must apply a placeable candidate");
    expect(game.activePiece().rotation == expected.rotation,
           "rotation input must update rotation state");
    expect(game.activePiece().blocks == expected.blocks,
           "rotation input must use Tetromino block coordinates");
}

}  // namespace

int main() {
    try {
        testCanPlaceOnEmptyBoard();
        testWallAndFloorCollisionRejected();
        testBlockCollisionRejected();
        testLockPieceStoresTypeAndColor();
        testLockedPieceCompletesAndClearsLine();
        testClearWithoutFullLinesKeepsBoard();
        testNoLineClearPreservesEveryCell();
        testClearSingleLineShiftsRowsDown();
        testClearMultipleConsecutiveLines();
        testClearTopLineEmptiesReplacementCells();
        testGameTickLocksPieceAndPromotesNextPiece();
        testGameMovementRejectsBlockedCandidates();
        testGameRotationAppliesTetrominoCandidate();
        std::cout << "collision_test: all passed\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "collision_test failed: " << ex.what() << '\n';
        return 1;
    }
}
