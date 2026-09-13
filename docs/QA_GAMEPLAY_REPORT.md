# Gameplay Testing Report

## Test information

| Field | Value |
| --- | --- |
| Task | No.12: Gameplay Testing & Bug Report |
| Branch | `feature/integration` |
| Commit | `e98799c` |
| Test date | 2026-09-13 |
| Tester role | Gameplay QA |
| Operating system | macOS 15.6.1, Apple Silicon |
| Compiler | Apple Clang 17.0.0 |
| Build system | CMake 4.4.3, Debug build |
| Interactive surface | ANSI-compatible terminal |

## Test approach

The integrated game was tested through three complementary paths:

1. A clean CMake build and the complete CTest suite.
2. A sanitized build using AddressSanitizer and UndefinedBehaviorSanitizer.
3. Interactive terminal gameplay using both letter keys and arrow-key escape
   sequences.

Random-dependent regression tests were also repeated ten times to exercise
different Tetromino sequences.

## Functional results

| ID | Feature | Test performed | Expected result | Result |
| --- | --- | --- | --- | --- |
| GT-01 | Game startup | Start the executable in an interactive terminal | A 10 x 20 board, score 0, an active piece, a next-piece preview, and controls are displayed | PASS |
| GT-02 | Tetromino spawning | Exercise all explicit shape factories and observe repeated random spawns/restarts | I, O, T, S, Z, J, and L use valid spawn layouts; random results remain within the seven types | PASS |
| GT-03 | Falling | Start a session and wait without input | The active piece moves down at the gravity interval | PASS |
| GT-04 | Left/right movement | Use A/D and left/right arrows; exercise both board edges in regression tests | Valid movement is applied and movement beyond either wall is rejected atomically | PASS |
| GT-05 | Soft drop | Use S and the down arrow repeatedly | The piece moves down; a blocked downward action locks it and promotes the preview piece | PASS |
| GT-06 | Rotation | Use W and the up arrow; rotate test pieces four times | Valid clockwise rotation is applied, O keeps its shape, and four rotations restore the original state | PASS |
| GT-07 | Collision | Attempt placement beyond walls/floor and over locked cells | Invalid candidates are rejected without changing the active piece or board | PASS |
| GT-08 | Piece locking | Soft-drop pieces to the floor and onto the stack | Exactly four typed cells are stored and the next piece becomes active | PASS |
| GT-09 | Line detection/clear | Complete one row in the integrated Game test | The completed row is detected, removed, and cells above it move down | PASS |
| GT-10 | Multiple-line clear | Execute collision regression cases for consecutive full rows and four rows | Every full row is removed in one call and the returned count is correct | PASS |
| GT-11 | Scoring | Clear 0-4 rows through scoring tests and one row through Game integration | Awards are 0, 100, 300, 500, and 800; score accumulates and is exposed to the renderer | PASS |
| GT-12 | Game Over | Repeatedly soft-drop pieces in the spawn columns until the stack blocks spawn | `GAME OVER` is displayed when the promoted piece cannot spawn | PASS |
| GT-13 | Stopped gameplay | Send A, W, and S after Game Over | Movement, rotation, and falling no longer change the game | PASS |
| GT-14 | Restart | Press R after Game Over | Board is emptied, score becomes 0, Game Over clears, and new active/next pieces are generated | PASS |
| GT-15 | Quit/terminal cleanup | Press Q and compare terminal settings before and after execution | The game exits normally, restores the alternate screen/cursor, and preserves the original terminal settings | PASS |

## Automated verification

Clean Debug build and CTest result:

```text
100% tests passed, 0 tests failed out of 6

core_test          PASS
tetromino_test     PASS
collision_test     PASS
scoring_test       PASS
game_state_test    PASS
integration_test   PASS
```

The six tests passed for ten consecutive runs. The sanitized build also passed
all six tests without an AddressSanitizer or UndefinedBehaviorSanitizer report.

## Bug report

No reproducible functional bugs were found on the tested macOS environment.
Therefore, no feature-owner bug card is required for this test round. If a bug
is found on another environment, create a separate Trello card containing the
commit, environment, preconditions, reproduction steps, expected result,
actual result, reproducibility, severity, evidence, related feature, and owner.

## Coverage limitation

Windows-specific input handling and Windows terminal rendering were not
executed in this round because the available test host is macOS. This is an
environment coverage gap, not a confirmed product bug. A Windows smoke test is
recommended before the final release.

## Conclusion

The integrated build satisfies the tested gameplay flow:

```text
Spawn -> Fall/Move/Rotate -> Collision -> Lock -> Line Clear -> Score
      -> Next Piece -> Game Over -> Restart
```

The build is ready for the next testing or release-review stage on macOS.
