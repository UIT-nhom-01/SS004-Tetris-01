#include "features/Scoring.hpp"
#include <stdexcept>

namespace tetris {

// Đưa điểm về 0
void Scoring::reset() {
    score_ = 0;
}

// Cộng điểm dựa theo số hàng được xóa
void Scoring::addLines(int lineCount) {
    switch (lineCount) {
        case 0:
            break;

        case 1:
            score_ += 100;
            break;

        case 2:
            score_ += 300;
            break;

        case 3:
            score_ += 500;
            break;

        case 4:
            score_ += 800;
            break;

        default:
            throw std::invalid_argument(
                "lineCount must be between 0 and 4"
            );
    }
}

// Trả về điểm hiện tại
int Scoring::getScore() const {
    return score_;
}

} // namespace tetris
