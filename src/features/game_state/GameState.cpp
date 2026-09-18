#include "features/GameState.hpp"

namespace tetris {

// Report whether the current session has ended.
bool GameState::isGameOver() const {
    return gameOver_;
}

// A blocked spawn ends the session; a valid spawn keeps it running.
void GameState::updateAfterSpawn(bool spawnPositionValid) {
    gameOver_ = !spawnPositionValid;
}

// Return to the running state for a new session.
void GameState::reset() {
    gameOver_ = false;
}

}  // namespace tetris
