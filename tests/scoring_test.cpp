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

// A new scoring session starts at zero.
void testInitialScore() {
    tetris::Scoring scoring;

    expect(scoring.getScore() == 0, "initial score must be 0");
}

// Clearing one to four lines awards points from the agreed scoring table.
void testScoreTable() {
    {
        tetris::Scoring scoring;
        scoring.addLines(1);

        expect(scoring.getScore() == 100,
               "clearing 1 line must add 100 points");
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(2);

        expect(scoring.getScore() == 300,
               "clearing 2 lines must add 300 points");
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(3);

        expect(scoring.getScore() == 500,
               "clearing 3 lines must add 500 points");
    }

    {
        tetris::Scoring scoring;
        scoring.addLines(4);

        expect(scoring.getScore() == 800,
               "clearing 4 lines must add 800 points");
    }
}

// Points accumulate across multiple line-clear events.
void testAccumulatedScore() {
    tetris::Scoring scoring;

    scoring.addLines(1);  // +100
    scoring.addLines(2);  // +300
    scoring.addLines(4);  // +800

    expect(scoring.getScore() == 1200,
           "score must accumulate across multiple calls");
}

// Reset starts a new scoring session at zero.
void testReset() {
    tetris::Scoring scoring;

    scoring.addLines(4);
    scoring.reset();

    expect(scoring.getScore() == 0, "reset must restore score to 0");
}

// Clearing no lines leaves the current score unchanged.
void testZeroLines() {
    tetris::Scoring scoring;

    scoring.addLines(2);
    const int scoreBefore = scoring.getScore();

    scoring.addLines(0);

    expect(scoring.getScore() == scoreBefore,
           "clearing 0 lines must not change score");
}

// Counts outside the supported range are rejected.
void testInvalidLineCount() {
    {
        tetris::Scoring scoring;
        bool threw = false;

        try {
            scoring.addLines(-1);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        expect(threw, "negative lineCount must throw std::invalid_argument");
    }

    {
        tetris::Scoring scoring;
        bool threw = false;

        try {
            scoring.addLines(5);
        } catch (const std::invalid_argument&) {
            threw = true;
        }

        expect(threw,
               "lineCount greater than 4 must throw std::invalid_argument");
    }
}

void testInvalidLineCountPreservesAccumulatedScore() {
    tetris::Scoring scoring;
    scoring.addLines(1);
    scoring.addLines(2);
    const int scoreBefore = scoring.getScore();

    for (const int invalidCount : {-1, 5}) {
        bool threw = false;
        try {
            scoring.addLines(invalidCount);
        } catch (const std::invalid_argument&) {
            threw = true;
        }
        expect(threw, "invalid line count must be rejected");
        expect(scoring.getScore() == scoreBefore,
               "rejected scoring input must preserve earned points");
    }
}

}  // namespace

int main() {
    try {
        testInitialScore();
        testScoreTable();
        testAccumulatedScore();
        testReset();
        testZeroLines();
        testInvalidLineCount();
        testInvalidLineCountPreservesAccumulatedScore();

        std::cout << "scoring_test: all passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "scoring_test failed: " << error.what() << '\n';
        return 1;
    }
}
