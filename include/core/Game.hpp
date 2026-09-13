#pragma once

#include "core/ConsoleRenderer.hpp"
#include "features/Collision.hpp"
#include "core/GameBoard.hpp"
#include "core/Input.hpp"
#include "core/Types.hpp"
#include "features/GameState.hpp"
#include "features/Scoring.hpp"

namespace tetris {

/// Owns the core game loop and orchestrates board, input, and feature modules.
class Game {
public:
    static constexpr int FALL_INTERVAL_MS = 500;
    static constexpr int LOOP_SLEEP_MS = 16;

    Game();

    /// Runs until the player quits. Gravity advances independently of input.
    void run();

    /// Applies a translation only when every candidate block is placeable.
    /// Occupied-cell validation is delegated to Collision.
    /// Movement and rotation are rejected after Game Over.
    bool moveCurrentPiece(int dx, int dy);
    bool rotateCurrentPiece();

    /// Advances gravity by one row; returns true when game state changed.
    /// Returns false without changing state after Game Over.
    bool tick();

    /// Resets board, score, and Game Over state, then spawns new pieces
    /// without restarting the process.
    void restart();

    /// Exposes read-only state for feature integration and tests.
    [[nodiscard]] const GameBoard& board() const;
    [[nodiscard]] const ActivePiece& activePiece() const;
    [[nodiscard]] const ActivePiece& nextPiece() const;
    [[nodiscard]] int score() const;
    [[nodiscard]] bool isGameOver() const;

private:
    bool handleInput(InputAction action);
    void render() const;

    GameBoard board_;
    ActivePiece activePiece_;
    ActivePiece nextPiece_;
    Collision collision_;
    Scoring scoring_;
    GameState gameState_;
    Input input_;
    ConsoleRenderer renderer_;
    bool running_{true};
};

}  // namespace tetris
