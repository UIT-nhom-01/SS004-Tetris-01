#include "core/Game.hpp"
#include "core/GameBoard.hpp"
#include "core/Types.hpp"
#include "features/Collision.hpp"
#include "features/GameState.hpp"

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

bool boardsEqual(const tetris::GameBoard& lhs, const tetris::GameBoard& rhs) {
    for (int y = 0; y < lhs.height(); ++y) {
        for (int x = 0; x < lhs.width(); ++x) {
            if (lhs.getCell(x, y) != rhs.getCell(x, y)) {
                return false;
            }
        }
    }
    return true;
}

bool piecesEqual(
    const tetris::ActivePiece& lhs,
    const tetris::ActivePiece& rhs) {
    return lhs.type == rhs.type && lhs.rotation == rhs.rotation &&
        lhs.origin == rhs.origin && lhs.blocks == rhs.blocks;
}

// Drops every piece straight down from its spawn position. Spawned pieces never
// reach column 0, so no row can clear and the stack must eventually block the
// spawn area.
void playUntilGameOver(tetris::Game& game) {
    tetris::Collision collision;

    for (int tick = 0;
         tick < MAX_TICKS_UNTIL_GAME_OVER && !game.isGameOver();
         ++tick) {
        expect(collision.canPlace(game.board(), game.activePiece()),
               "active piece must be placeable while the game is running");
        game.tick();
    }

    expect(game.isGameOver(), "stacking pieces must eventually end the game");
}

// A new session starts in the playing state.
void testInitialStateIsPlaying() {
    tetris::GameState state;

    expect(!state.isGameOver(), "a new game state must not be Game Over");
}

// A valid spawn keeps the session running.
void testValidSpawnKeepsPlaying() {
    tetris::GameState state;
    state.updateAfterSpawn(true);

    expect(!state.isGameOver(), "a valid spawn must not end the game");
}

// A blocked spawn ends the session.
void testInvalidSpawnSetsGameOver() {
    tetris::GameState state;
    state.updateAfterSpawn(false);

    expect(state.isGameOver(), "an invalid spawn must set Game Over");
}

// A later valid spawn returns the state to playing.
void testValidSpawnAfterGameOverResumesPlaying() {
    tetris::GameState state;
    state.updateAfterSpawn(false);
    state.updateAfterSpawn(true);

    expect(!state.isGameOver(),
           "a valid spawn must restore the playing state");
}

// Reset starts a new session after Game Over.
void testResetAfterGameOver() {
    tetris::GameState state;
    state.updateAfterSpawn(false);
    state.reset();

    expect(!state.isGameOver(), "reset must restore the playing state");
}

// Reading the state does not change it.
void testGetterDoesNotChangeState() {
    tetris::GameState state;
    state.updateAfterSpawn(false);

    for (int call = 0; call < 3; ++call) {
        expect(state.isGameOver(), "isGameOver must not change the state");
    }
}

// Game Over is reported only when the promoted piece cannot spawn.
void testGameOverDetectedWhenSpawnBlocked() {
    tetris::Game game;
    tetris::Collision collision;
    expect(!game.isGameOver(), "a new game must not start in Game Over");

    playUntilGameOver(game);

    expect(!collision.canPlace(game.board(), game.activePiece()),
           "Game Over must be set when the spawned piece overlaps the stack");
}

// Gravity, movement, and rotation are ignored after Game Over.
void testGameplayStopsAfterGameOver() {
    tetris::Game game;
    playUntilGameOver(game);

    const tetris::GameBoard boardBefore = game.board();
    const tetris::ActivePiece activeBefore = game.activePiece();
    const tetris::ActivePiece nextBefore = game.nextPiece();

    expect(!game.tick(), "gravity must not advance after Game Over");
    expect(!game.moveCurrentPiece(-1, 0),
           "left movement must be ignored after Game Over");
    expect(!game.moveCurrentPiece(1, 0),
           "right movement must be ignored after Game Over");
    expect(!game.moveCurrentPiece(0, 1),
           "soft drop must be ignored after Game Over");
    expect(!game.rotateCurrentPiece(),
           "rotation must be ignored after Game Over");

    expect(game.isGameOver(), "ignored actions must keep the Game Over state");
    expect(boardsEqual(game.board(), boardBefore),
           "board must not change after Game Over");
    expect(piecesEqual(game.activePiece(), activeBefore),
           "active piece must not change after Game Over");
    expect(piecesEqual(game.nextPiece(), nextBefore),
           "next piece must not change after Game Over");
}

// Restart clears the board, score, and Game Over state, then spawns new pieces.
void testRestartResetsBoardScoreAndState() {
    tetris::Game game;
    tetris::Collision collision;

    // Seed the bottom row so the initial O piece completes one line. Game only
    // exposes a read-only board because production mutation belongs to
    // Collision; this controlled test setup removes constness from a non-const
    // Game instance without changing the production API.
    auto& board = const_cast<tetris::GameBoard&>(game.board());
    for (int x = 0; x < 8; ++x) {
        board.setCell(x, 19, tetris::CellState::J);
    }
    expect(game.moveCurrentPiece(4, 18),
           "score reset setup must move the O piece into the final gap");
    expect(game.tick(),
           "locking the O piece must complete and clear the bottom row");
    expect(game.score() == 100,
           "score reset setup must create a non-zero score");

    playUntilGameOver(game);
    expect(game.score() == 100,
           "stacking after the setup must preserve the earned score");

    game.restart();

    expect(!game.isGameOver(), "restart must clear Game Over");
    expect(game.score() == 0, "restart must reset the score");
    for (int y = 0; y < game.board().height(); ++y) {
        for (int x = 0; x < game.board().width(); ++x) {
            expect(game.board().getCell(x, y) == tetris::CellState::Empty,
                   "restart must empty every board cell");
        }
    }
    expect(game.activePiece().type == tetris::TetrominoType::O &&
               game.activePiece().origin == tetris::Position{4, 0},
           "restart must spawn a fresh active piece");
    expect(collision.canPlace(game.board(), game.activePiece()),
           "restarted active piece must be placeable");
    expect(game.nextPiece().type == tetris::TetrominoType::T,
           "restart must refill the next piece");
}

// A restarted session accepts gameplay and can reach Game Over again.
void testNewSessionPlaysAfterRestart() {
    tetris::Game game;
    playUntilGameOver(game);
    game.restart();

    const tetris::Position spawnOrigin = game.activePiece().origin;
    expect(game.tick(), "gravity must resume after restart");
    expect(game.activePiece().origin ==
               tetris::Position{spawnOrigin.x, spawnOrigin.y + 1},
           "gravity must move the restarted piece down");
    expect(game.moveCurrentPiece(-1, 0),
           "movement must resume after restart");
    expect(game.rotateCurrentPiece(), "rotation must resume after restart");

    playUntilGameOver(game);
}

}  // namespace

int main() {
    try {
        testInitialStateIsPlaying();
        testValidSpawnKeepsPlaying();
        testInvalidSpawnSetsGameOver();
        testValidSpawnAfterGameOverResumesPlaying();
        testResetAfterGameOver();
        testGetterDoesNotChangeState();
        testGameOverDetectedWhenSpawnBlocked();
        testGameplayStopsAfterGameOver();
        testRestartResetsBoardScoreAndState();
        testNewSessionPlaysAfterRestart();

        std::cout << "game_state_test: all passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "game_state_test failed: " << error.what() << '\n';
        return 1;
    }
}
