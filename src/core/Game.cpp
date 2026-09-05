#include "core/Game.hpp"

#include <iostream>
#include <string>

namespace tetris {

Game::Game()
    : activePiece_{{{4, 0}, {5, 0}, {4, 1}, {5, 1}}}, running_(true) {
    // TEMPORARY:
    // This O-shaped piece is used only to test the core game loop.
    // It will be replaced by Huy's Tetromino module.
}

void Game::run() {
    render();

    while (running_) {
        std::cout << "Action: " << std::flush;
        const InputAction action = input_.readAction();
        handleInput(action);

        if (running_ && action != InputAction::None) {
            update();
            render();
        }
    }

    std::cout << "Game closed.\n";
}

bool Game::moveCurrentPiece(int dx, int dy) {
    // Boundary-only validation keeps the core demonstration working.
    // TODO(Tu): delegate full movement validation to Collision after integration.
    for (const Position& block : activePiece_.blocks) {
        if (!board_.isInside(block.x + dx, block.y + dy)) {
            return false;
        }
    }

    for (Position& block : activePiece_.blocks) {
        block.x += dx;
        block.y += dy;
    }

    return true;
}

const GameBoard& Game::board() const {
    return board_;
}

const ActivePiece& Game::activePiece() const {
    return activePiece_;
}

void Game::handleInput(InputAction action) {
    switch (action) {
        case InputAction::MoveLeft:
            moveCurrentPiece(-1, 0);
            break;
        case InputAction::MoveRight:
            moveCurrentPiece(1, 0);
            break;
        case InputAction::MoveDown:
            moveCurrentPiece(0, 1);
            break;
        case InputAction::Rotate:
            // TODO(Huy): integrate Tetromino rotation.
            break;
        case InputAction::Quit:
            running_ = false;
            break;
        case InputAction::None:
            break;
    }
}

void Game::update() {
    // TODO(Tu): integrate collision, piece locking, and line clearing.
    // TODO(Gam): integrate Scoring.
    // TODO(Khanh): integrate Game Over detection and Restart.
}

void Game::render() const {
    std::cout << '\n' << '+' << std::string(GameBoard::WIDTH, '-') << "+\n";

    for (int y = 0; y < board_.height(); ++y) {
        std::cout << '|';
        for (int x = 0; x < board_.width(); ++x) {
            bool activeBlock = false;
            for (const Position& block : activePiece_.blocks) {
                if (block.x == x && block.y == y) {
                    activeBlock = true;
                    break;
                }
            }

            const bool filledCell = board_.getCell(x, y) == CellState::Filled;
            std::cout << ((activeBlock || filledCell) ? '#' : '.');
        }
        std::cout << "|\n";
    }

    std::cout << '+' << std::string(GameBoard::WIDTH, '-') << "+\n";
    std::cout << "Score: 0\n";  // TODO(Gam): integrate Scoring.
    std::cout << "Controls: A left, D right, S down, W rotate, Q quit\n";
}

}  // namespace tetris
