============================================================
   SNAKE GAME — C++ with OOP & Data Structures
   Version 1.0  |  Academic Project
============================================================

TEAM MEMBERS:
  - Ibrar Ahmed   → Implementation
  - M. Sanan      → Documentation
  - Hanzala Baqir → Testing & Analysis

------------------------------------------------------------
FILES IN THIS PACKAGE:
------------------------------------------------------------
  snake_game.cpp               → Main C++ source code
  Snake_Game_Documentation.docx → Full project report (DOCX)
  README.txt                   → This file

------------------------------------------------------------
HOW TO COMPILE & RUN:
------------------------------------------------------------

  WINDOWS (MinGW / g++):
    1. Open Command Prompt in this folder
    2. Run:  g++ -o snake snake_game.cpp
    3. Run:  snake.exe

  LINUX / macOS:
    1. Open terminal in this folder
    2. Run:  g++ -o snake snake_game.cpp
    3. Run:  ./snake

------------------------------------------------------------
CONTROLS:
------------------------------------------------------------
  ENTER   →  Start game
  W       →  Move UP
  S       →  Move DOWN
  A       →  Move LEFT
  D       →  Move RIGHT
  Q       →  Quit game

------------------------------------------------------------
GAMEPLAY:
------------------------------------------------------------
  - Control the snake to eat food ($)
  - Each food eaten = +10 points
  - Snake grows after eating food
  - Game over if snake hits a wall or itself
  - High score is saved to highscore.txt automatically
  - Press Y to play again, N to exit after game over

------------------------------------------------------------
CONCEPTS COVERED:
------------------------------------------------------------
  OOP       : Classes, Encapsulation, Composition
  DS        : vector<Point> as dynamic deque, 2D char array
  PF        : Loops, Conditionals, Functions, Enums, Structs
  Algorithms: Grid Traversal, Collision Detection,
              Dynamic Body Update

------------------------------------------------------------
COMPLEXITY:
------------------------------------------------------------
  Time per game tick  : O(n)  — n = snake length
  Space (snake body)  : O(n)
  Wall collision check: O(1)
  Self collision check: O(n)

============================================================
