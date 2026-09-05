# SS004 – Tetris Final Project

Repository: `SS004-Tetris-01`

## Project Overview

Basic console Tetris game developed for the SS004 final project. The project
uses C++17 and CMake, with a small core separated from independently owned
feature modules.

## Current Status

The core project structure, board, console loop, input mapping, temporary test
piece, and core tests are complete. Feature development has not started yet.

The current O-shaped piece is temporary and exists only to demonstrate core
movement. It is not a Tetromino feature implementation.

## Build

Requirements: a C++17 compiler and CMake 3.16 or newer.

```sh
cmake -S . -B build
cmake --build build
```

No external libraries are required.

## Run

```sh
./build/tetris
```

Enter a control key and press Enter when using a line-buffered terminal.

## Controls

- `A` - move left
- `D` - move right
- `S` - move down
- `W` - rotate (integration placeholder only)
- `Q` - quit

## Test

```sh
ctest --test-dir build --output-on-failure
```

## Team

- **Nguyễn Mạnh Hùng** – Team Leader / Core / Integration
- **Lê Hoàng Huy** – Tetromino / Rotation
- **Nguyễn Quý Tứ** – Collision / Line Clearing
- **Nguyễn Thị Hồng Gấm** – Scoring
- **Nguyễn Gia Khánh** – Game Over / Restart

## Project Structure

```text
include/core/                 Shared types and core public APIs
include/features/             Feature API boundaries (declarations only)
src/core/                     Board, input, and game orchestration
src/features/tetromino/       Huy's future implementation
src/features/collision/       Tứ's future implementation
src/features/scoring/         Gấm's future implementation
src/features/game_state/      Khánh's future implementation
tests/                        Core-only tests
```

Feature `.cpp` files added below `src/features/` are picked up automatically by
CMake. Core integration remains the Team Leader's responsibility.

## Development Workflow

The core baseline is merged into `main` first. Each member then creates a
feature branch from the same baseline and works mainly in the assigned feature
header and source folder. Hùng reviews and integrates the completed features.

Suggested branches:

- `main`
- `feature/game-board`
- `feature/tetromino`
- `feature/collision`
- `feature/scoring`
- `feature/game-over`

Feature ownership:

| Member | Header | Source folder | TODO |
| --- | --- | --- | --- |
| Huy | `include/features/Tetromino.hpp` | `src/features/tetromino/` | Piece generation and rotation |
| Tứ | `include/features/Collision.hpp` | `src/features/collision/` | Collision and line clearing |
| Gấm | `include/features/Scoring.hpp` | `src/features/scoring/` | Scoring system |
| Khánh | `include/features/GameState.hpp` | `src/features/game_state/` | Game Over and Restart state |
