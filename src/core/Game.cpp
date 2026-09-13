#include "core/Game.hpp"

#include <chrono>
#include <cstdio>
#include <iostream>
#include <thread>

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

namespace tetris {

namespace {

bool standardOutputIsTerminal() {
#if defined(_WIN32)
    return _isatty(_fileno(stdout)) != 0;
#else
    return isatty(STDOUT_FILENO) != 0;
#endif
}

/// Uses a separate terminal screen so rendered frames never enter scrollback.
class TerminalScreen {
public:
    TerminalScreen() : active_(standardOutputIsTerminal()) {
        if (active_) {
            std::cout << "\x1B[?1049h\x1B[?25l" << std::flush;
        }
    }

    ~TerminalScreen() {
        restore();
    }

    TerminalScreen(const TerminalScreen&) = delete;
    TerminalScreen& operator=(const TerminalScreen&) = delete;

    void restore() {
        if (active_) {
            std::cout << "\x1B[?25h\x1B[?1049l" << std::flush;
            active_ = false;
        }
    }

private:
    bool active_;
};

}  // namespace

Game::Game() {
    restart();
}

void Game::run() {
    using Clock = std::chrono::steady_clock;

    TerminalScreen terminalScreen;
    render();
    // steady_clock prevents system clock changes from affecting gravity timing.
    auto nextFall = Clock::now() + std::chrono::milliseconds(FALL_INTERVAL_MS);

    while (running_) {
        bool stateChanged = false;
        const InputAction action = input_.pollAction();
        if (action != InputAction::None) {
            stateChanged = handleInput(action);
        }

        const auto now = Clock::now();
        if (action == InputAction::Restart) {
            // A restarted piece always receives a complete first fall interval.
            nextFall = now + std::chrono::milliseconds(FALL_INTERVAL_MS);
        } else if (running_ && now >= nextFall) {
            stateChanged = tick() || stateChanged;
            nextFall = now + std::chrono::milliseconds(FALL_INTERVAL_MS);
        }

        if (running_ && stateChanged) {
            render();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(LOOP_SLEEP_MS));
    }

    terminalScreen.restore();
    std::cout << "Game closed.\n";
}

bool Game::moveCurrentPiece(int dx, int dy) {
    if (gameState_.isGameOver()) {
        return false;
    }

    const ActivePiece candidate = translated(activePiece_, dx, dy);

    if (!collision_.canPlace(board_, candidate)) {
        return false;
    }

    activePiece_ = candidate;
    return true;
}

bool Game::rotateCurrentPiece() {
    if (gameState_.isGameOver()) {
        return false;
    }

    const ActivePiece candidate = tetromino_.getRotated(activePiece_);

    if (!collision_.canPlace(board_, candidate)) {
        return false;
    }

    activePiece_ = candidate;
    return true;
}

bool Game::tick() {
    if (gameState_.isGameOver()) {
        return false;
    }

    if (moveCurrentPiece(0, 1)) {
        return true;
    }

    collision_.lockPiece(board_, activePiece_);
    scoring_.addLines(collision_.clearCompletedLines(board_));
    activePiece_ = nextPiece_;
    nextPiece_ = tetromino_.createPiece();
    // Game Over when the promoted piece overlaps the stack at its spawn position.
    gameState_.updateAfterSpawn(collision_.canPlace(board_, activePiece_));
    return true;
}

void Game::restart() {
    board_.reset();
    scoring_.reset();
    gameState_.reset();
    activePiece_ = tetromino_.createPiece();
    nextPiece_ = tetromino_.createPiece();
    gameState_.updateAfterSpawn(collision_.canPlace(board_, activePiece_));
    running_ = true;
}

const GameBoard& Game::board() const {
    return board_;
}

const ActivePiece& Game::activePiece() const {
    return activePiece_;
}

const ActivePiece& Game::nextPiece() const {
    return nextPiece_;
}

int Game::score() const {
    return scoring_.getScore();
}

bool Game::isGameOver() const {
    return gameState_.isGameOver();
}

bool Game::handleInput(InputAction action) {
    switch (action) {
        case InputAction::MoveLeft:
            return moveCurrentPiece(-1, 0);
        case InputAction::MoveRight:
            return moveCurrentPiece(1, 0);
        case InputAction::MoveDown:
            // Soft drop follows the same collision/locking flow as gravity.
            return tick();
        case InputAction::Rotate:
            return rotateCurrentPiece();
        case InputAction::Restart:
            restart();
            return true;
        case InputAction::Quit:
            running_ = false;
            return false;
        case InputAction::None:
            return false;
    }

    return false;
}

void Game::render() const {
    // Redraw in place within the alternate screen owned by run().
    const bool useTerminalFeatures = standardOutputIsTerminal();
    if (useTerminalFeatures) {
        std::cout << "\x1B[2J\x1B[H";
    }

    std::cout << renderer_.buildFrame(
        board_,
        activePiece_,
        nextPiece_,
        scoring_.getScore(),
        gameState_.isGameOver(),
        useTerminalFeatures)
              << std::flush;
}

}  // namespace tetris
