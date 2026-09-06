#pragma once

namespace tetris {

/// Owns the Game Over state for one game session.
class GameState {
public:
    /// Returns whether gameplay input and gravity should be paused.
    [[nodiscard]] bool isGameOver() const;

    /// Updates Game Over after Game validates a newly spawned piece.
    void updateAfterSpawn(bool spawnPositionValid);

    /// Restores the running state for a new session.
    void reset();

private:
    bool gameOver_{false};
};

// TODO(Khanh): implement Game Over and Restart state in
// src/features/game_state/.

}  // namespace tetris
