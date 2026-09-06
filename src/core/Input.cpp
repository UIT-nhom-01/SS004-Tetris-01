#include "core/Input.hpp"

#include <cctype>
#include <memory>

#if defined(_WIN32)
#include <conio.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace tetris {

struct Input::TerminalState {
#if !defined(_WIN32)
    termios originalSettings{};
    bool shouldRestore{false};

    TerminalState() {
        if (isatty(STDIN_FILENO) == 0 ||
            tcgetattr(STDIN_FILENO, &originalSettings) != 0) {
            return;
        }

        termios immediateSettings = originalSettings;
        immediateSettings.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));
        immediateSettings.c_cc[VMIN] = 0;
        immediateSettings.c_cc[VTIME] = 0;

        shouldRestore =
            tcsetattr(STDIN_FILENO, TCSANOW, &immediateSettings) == 0;
    }

    ~TerminalState() {
        if (shouldRestore) {
            tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
        }
    }
#endif
};

Input::Input() : terminalState_(std::make_unique<TerminalState>()) {}

Input::~Input() = default;

InputAction Input::pollAction() const {
#if defined(_WIN32)
    if (_kbhit() == 0) {
        return InputAction::None;
    }

    const int key = _getch();
    if (key == 0 || key == 224) {
        const int extendedKey = _getch();
        switch (extendedKey) {
            case 72:
                return InputAction::Rotate;
            case 75:
                return InputAction::MoveLeft;
            case 77:
                return InputAction::MoveRight;
            case 80:
                return InputAction::MoveDown;
            default:
                return InputAction::None;
        }
    }

    return fromCharacter(static_cast<char>(key));
#else
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(STDIN_FILENO, &readSet);

    // A zero timeout turns select() into a readiness poll instead of blocking.
    timeval timeout{};
    const int ready = select(STDIN_FILENO + 1, &readSet, nullptr, nullptr, &timeout);
    if (ready <= 0) {
        return InputAction::None;
    }

    unsigned char byte = 0;
    const ssize_t bytesRead = read(STDIN_FILENO, &byte, 1);
    if (bytesRead == 0) {
        return InputAction::Quit;
    }
    if (bytesRead < 0) {
        return InputAction::None;
    }
    return consumeInputByte(byte);
#endif
}

InputAction Input::fromCharacter(char character) {
    const auto normalized = static_cast<char>(
        std::tolower(static_cast<unsigned char>(character)));

    switch (normalized) {
        case 'a':
            return InputAction::MoveLeft;
        case 'd':
            return InputAction::MoveRight;
        case 's':
            return InputAction::MoveDown;
        case 'w':
            return InputAction::Rotate;
        case 'r':
            return InputAction::Restart;
        case 'q':
            return InputAction::Quit;
        default:
            return InputAction::None;
    }
}

InputAction Input::consumeInputByte(unsigned char byte) const {
    switch (escapeSequenceState_) {
        case EscapeSequenceState::None:
            if (byte == 0x1B) {
                escapeSequenceState_ = EscapeSequenceState::Escape;
                return InputAction::None;
            }
            return fromCharacter(static_cast<char>(byte));

        case EscapeSequenceState::Escape:
            if (byte == '[' || byte == 'O') {
                escapeSequenceState_ = EscapeSequenceState::ControlSequence;
                return InputAction::None;
            }
            escapeSequenceState_ = EscapeSequenceState::None;
            return fromCharacter(static_cast<char>(byte));

        case EscapeSequenceState::ControlSequence:
            escapeSequenceState_ = EscapeSequenceState::None;
            switch (byte) {
                case 'A':
                    return InputAction::Rotate;
                case 'B':
                    return InputAction::MoveDown;
                case 'C':
                    return InputAction::MoveRight;
                case 'D':
                    return InputAction::MoveLeft;
                default:
                    return InputAction::None;
            }
    }

    return InputAction::None;
}

}  // namespace tetris
