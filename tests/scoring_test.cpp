#include "features/Scoring.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// 1. Giá trị ban đầu phải bằng 0
void testInitialScore() {
    tetris::Scoring scoring;

    expect(
        scoring.getScore() == 0,
        "initial score must be 0"
    );
}

// 2. Xóa 1-4 hàng phải cộng đúng điểm
void testScoreTable() {
    {
        tetris::Scoring scoring;
        scoring.addLines(1);

        expect(
            scoring.getScore() == 100,
            "clearing 1 line must add 100 points"
        );
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(2);

        expect(
            scoring.getScore() == 300,
            "clearing 2 lines must add 300 points"
        );
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(3);

        expect(
            scoring.getScore() == 500,
            "clearing 3 lines must add 500 points"
        );
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(4);

        expect(
            scoring.getScore() == 800,
            "clearing 4 lines must add 800 points"
        );
    }
}

// 3. Gọi nhiều lần thì điểm phải được cộng dồn
void testAccumulatedScore() {
    tetris::Scoring scoring;

    scoring.addLines(1); // +100
    scoring.addLines(2); // +300
    scoring.addLines(4); // +800

    expect(
        scoring.getScore() == 1200,
        "score must accumulate across multiple calls"
    );
}

// 4. reset() phải đưa score về 0
void testReset() {
    tetris::Scoring scoring;

    scoring.addLines(4);
    scoring.reset();

    expect(
        scoring.getScore() == 0,
        "reset must restore score to 0"
    );
}

// 5. lineCount == 0 thì score không đổi
void testZeroLines() {
    tetris::Scoring scoring;

    scoring.addLines(2);
    const int scoreBefore = scoring.getScore();

    scoring.addLines(0);

    expect(
        scoring.getScore() == scoreBefore,
        "clearing 0 lines must not change score"
    );
}

// 6. Giá trị ngoài 0-4 phải ném std::invalid_argument
void testInvalidLineCount() {
    {
        tetris::Scoring scoring;
        bool threw = false;

        try {
            scoring.addLines(-1);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        expect(
            threw,
            "negative lineCount must throw std::invalid_argument"
        );
    }

    {
        tetris::Scoring scoring;
        bool threw = false;

        try {
            scoring.addLines(5);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        expect(
            threw,
            "lineCount greater than 4 must throw std::invalid_argument"
        );
    }
}

} // namespace

int main() {
    try {
        testInitialScore();
        testScoreTable();
        testAccumulatedScore();
        testReset();
        testZeroLines();
        testInvalidLineCount();

        std::cout << "scoring_test: all passed\n";
        return 0;

    } catch (const std::exception& error) {
        std::cerr
            << "scoring_test failed: "
            << error.what()
            << '\n';

        return 1;
    }
} 