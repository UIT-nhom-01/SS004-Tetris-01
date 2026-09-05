#include "core/Input.hpp"

#include <cctype>
#include <iostream>

namespace tetris {

InputAction Input::readAction() const {
    char character = '\0';
    if (!(std::cin >> character)) {
        return InputAction::Quit;
    }

    return fromCharacter(character);
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
        case 'q':
            return InputAction::Quit;
        default:
            return InputAction::None;
    }
}

}  // namespace tetris
