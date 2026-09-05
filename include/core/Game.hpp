#pragma once

#include "core/GameBoard.hpp"
#include "core/Input.hpp"
#include "core/Types.hpp"

namespace tetris {

class Game {
public:
    Game();

    void run();

    // Basic core movement. Full validation will later be delegated to the
    // Collision feature.
    bool moveCurrentPiece(int dx, int dy);

    [[nodiscard]] const GameBoard& board() const;
    [[nodiscard]] const ActivePiece& activePiece() const;

private:
    void handleInput(InputAction action);
    void update();
    void render() const;

    GameBoard board_;
    ActivePiece activePiece_;
    Input input_;
    bool running_;
};

}  // namespace tetris
