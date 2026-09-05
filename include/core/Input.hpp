#pragma once

#include "core/Types.hpp"

namespace tetris {

class Input {
public:
    [[nodiscard]] InputAction readAction() const;
    [[nodiscard]] static InputAction fromCharacter(char character);
};

}  // namespace tetris
