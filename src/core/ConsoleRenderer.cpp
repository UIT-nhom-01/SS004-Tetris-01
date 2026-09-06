#include "core/ConsoleRenderer.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>
#include <utility>

namespace tetris {

namespace {

constexpr int CELL_DISPLAY_WIDTH = 2;
constexpr int PANEL_INNER_WIDTH = 32;
constexpr int PREVIEW_GRID_SIZE = 4;
constexpr std::string_view ANSI_RESET = "\x1B[0m";

[[nodiscard]] std::string_view colorFor(TetrominoType type) {
    switch (type) {
        case TetrominoType::I:
            return "\x1B[46m";  // Cyan
        case TetrominoType::O:
            return "\x1B[43m";  // Yellow
        case TetrominoType::T:
            return "\x1B[45m";  // Magenta
        case TetrominoType::S:
            return "\x1B[42m";  // Green
        case TetrominoType::Z:
            return "\x1B[41m";  // Red
        case TetrominoType::J:
            return "\x1B[44m";  // Blue
        case TetrominoType::L:
            return "\x1B[48;5;208m";  // Orange in the ANSI 256-color palette
    }

    return {};
}

[[nodiscard]] std::string_view colorFor(CellState state) {
    switch (state) {
        case CellState::I:
            return colorFor(TetrominoType::I);
        case CellState::O:
            return colorFor(TetrominoType::O);
        case CellState::T:
            return colorFor(TetrominoType::T);
        case CellState::S:
            return colorFor(TetrominoType::S);
        case CellState::Z:
            return colorFor(TetrominoType::Z);
        case CellState::J:
            return colorFor(TetrominoType::J);
        case CellState::L:
            return colorFor(TetrominoType::L);
        case CellState::Empty:
            return {};
    }

    return {};
}

[[nodiscard]] std::string renderBlock(
    std::string_view color,
    bool useColor) {
    if (!useColor) {
        return "[]";
    }

    return std::string{color} + "  " + std::string{ANSI_RESET};
}

[[nodiscard]] std::string panelBorder() {
    return '+' + std::string(PANEL_INNER_WIDTH, '-') + '+';
}

[[nodiscard]] std::string panelLine(const std::string& content = {}) {
    const std::string visibleContent =
        content.substr(0, static_cast<std::size_t>(PANEL_INNER_WIDTH));
    return '|' + visibleContent +
        std::string(PANEL_INNER_WIDTH - visibleContent.size(), ' ') + '|';
}

[[nodiscard]] std::string centeredPanelLine(const std::string& content) {
    if (content.size() >= static_cast<std::size_t>(PANEL_INNER_WIDTH)) {
        return panelLine(content);
    }

    const std::size_t leftPadding =
        (static_cast<std::size_t>(PANEL_INNER_WIDTH) - content.size()) / 2;
    return panelLine(std::string(leftPadding, ' ') + content);
}

[[nodiscard]] std::array<std::string, GameBoard::HEIGHT + 2> buildBoardLines(
    const GameBoard& board,
    const ActivePiece& activePiece,
    bool useColor) {
    std::array<std::string, GameBoard::HEIGHT + 2> lines;
    const std::string border =
        '+' + std::string(GameBoard::WIDTH * CELL_DISPLAY_WIDTH, '-') + '+';
    lines.front() = border;

    for (int y = 0; y < board.height(); ++y) {
        std::string line{"|"};
        for (int x = 0; x < board.width(); ++x) {
            const auto activeBlock = std::find(
                activePiece.blocks.begin(),
                activePiece.blocks.end(),
                Position{x, y});

            if (activeBlock != activePiece.blocks.end()) {
                line += renderBlock(colorFor(activePiece.type), useColor);
                continue;
            }

            const CellState cell = board.getCell(x, y);
            line += isOccupied(cell)
                ? renderBlock(colorFor(cell), useColor)
                : std::string(CELL_DISPLAY_WIDTH, ' ');
        }
        line += '|';
        lines[static_cast<std::size_t>(y + 1)] = std::move(line);
    }

    lines.back() = border;
    return lines;
}

[[nodiscard]] std::array<std::string, PREVIEW_GRID_SIZE> buildPreviewLines(
    const ActivePiece& nextPiece,
    bool useColor) {
    std::array<std::array<bool, PREVIEW_GRID_SIZE>, PREVIEW_GRID_SIZE> cells{};

    int minimumX = nextPiece.blocks.front().x;
    int minimumY = nextPiece.blocks.front().y;
    for (const Position& block : nextPiece.blocks) {
        minimumX = std::min(minimumX, block.x);
        minimumY = std::min(minimumY, block.y);
    }

    for (const Position& block : nextPiece.blocks) {
        const int previewX = block.x - minimumX;
        const int previewY = block.y - minimumY;
        if (previewX >= 0 && previewX < PREVIEW_GRID_SIZE &&
            previewY >= 0 && previewY < PREVIEW_GRID_SIZE) {
            cells[static_cast<std::size_t>(previewY)]
                 [static_cast<std::size_t>(previewX)] = true;
        }
    }

    std::array<std::string, PREVIEW_GRID_SIZE> lines;
    const int previewWidth = PREVIEW_GRID_SIZE * CELL_DISPLAY_WIDTH;
    const int leftPadding = (PANEL_INNER_WIDTH - previewWidth) / 2;
    const int rightPadding = PANEL_INNER_WIDTH - previewWidth - leftPadding;

    for (int y = 0; y < PREVIEW_GRID_SIZE; ++y) {
        std::string line = '|' + std::string(leftPadding, ' ');
        for (int x = 0; x < PREVIEW_GRID_SIZE; ++x) {
            line += cells[static_cast<std::size_t>(y)]
                         [static_cast<std::size_t>(x)]
                ? renderBlock(colorFor(nextPiece.type), useColor)
                : std::string(CELL_DISPLAY_WIDTH, ' ');
        }
        line += std::string(rightPadding, ' ') + '|';
        lines[static_cast<std::size_t>(y)] = std::move(line);
    }

    return lines;
}

[[nodiscard]] std::array<std::string, GameBoard::HEIGHT + 2> buildPanelLines(
    const ActivePiece& nextPiece,
    int score,
    bool gameOver,
    bool useColor) {
    std::array<std::string, GameBoard::HEIGHT + 2> lines;
    const auto previewLines = buildPreviewLines(nextPiece, useColor);

    lines[0] = panelBorder();
    lines[1] = centeredPanelLine("SS004 TETRIS");
    lines[2] = panelBorder();
    lines[3] = panelLine(" SCORE");
    lines[4] = panelLine(" " + std::to_string(score));
    lines[5] = panelBorder();
    lines[6] = panelLine(" NEXT PIECE");
    lines[7] = panelLine();
    lines[8] = previewLines[0];
    lines[9] = previewLines[1];
    lines[10] = previewLines[2];
    lines[11] = previewLines[3];
    lines[12] = gameOver ? centeredPanelLine("GAME OVER") : panelLine();
    lines[13] = panelBorder();
    lines[14] = panelLine(" CONTROLS");
    lines[15] = panelLine(" Left/Right : arrows or A/D");
    lines[16] = panelLine(" Soft drop  : down arrow or S");
    lines[17] = panelLine(" Rotate     : up arrow or W");
    lines[18] = panelLine(" Restart    : R");
    lines[19] = panelLine(" Quit       : Q");
    lines[20] = panelLine();
    lines[21] = panelBorder();
    return lines;
}

}  // namespace

std::string ConsoleRenderer::buildFrame(
    const GameBoard& board,
    const ActivePiece& activePiece,
    const ActivePiece& nextPiece,
    int score,
    bool gameOver,
    bool useColor) const {
    const auto boardLines = buildBoardLines(board, activePiece, useColor);
    const auto panelLines =
        buildPanelLines(nextPiece, score, gameOver, useColor);

    std::string frame;
    for (std::size_t line = 0; line < boardLines.size(); ++line) {
        frame += boardLines[line] + "   " + panelLines[line] + '\n';
    }
    return frame;
}

}  // namespace tetris
