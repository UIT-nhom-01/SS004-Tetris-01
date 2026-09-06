#pragma once

#include <memory>

#include "core/Types.hpp"

namespace tetris {

/// Converts keyboard input into logical actions without owning game state.
class Input {
public:
    /// Enables immediate terminal input when standard input is an interactive TTY.
    Input();

    /// Restores the terminal settings that were active before construction.
    ~Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;
    Input(Input&&) = delete;
    Input& operator=(Input&&) = delete;

    /// Polls without blocking the game loop.
    [[nodiscard]] InputAction pollAction() const;

    /// Maps a character case-insensitively; unknown characters map to None.
    [[nodiscard]] static InputAction fromCharacter(char character);

private:
    enum class EscapeSequenceState {
        None,
        Escape,
        ControlSequence
    };

    struct TerminalState;

    /// Decodes one byte of a POSIX terminal escape sequence.
    [[nodiscard]] InputAction consumeInputByte(unsigned char byte) const;

    std::unique_ptr<TerminalState> terminalState_;
    mutable EscapeSequenceState escapeSequenceState_{EscapeSequenceState::None};
};

}  // namespace tetris
