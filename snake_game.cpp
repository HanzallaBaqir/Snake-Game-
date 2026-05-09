/*
 * ============================================================
 *   SNAKE GAME — C++ with OOP & Data Structures
 * ============================================================
 *  Project : Design and Analysis of a Snake Game using C++
 *  Course  : OOP | Data Structures | Programming Fundamentals
 * ============================================================
 *  Team Members:
 *   - Ibrar Ahmed   → Implementation (game logic, movement,
 *                     collision detection, scoring system)
 *   - M. Sanan      → Documentation (report, pseudocode,
 *                     algorithm explanations)
 *   - Hanzala Baqir → Testing & Analysis (test cases,
 *                     complexity observations)
 * ============================================================
 *
 *  COMPILATION:
 *   Windows (MinGW) : g++ -o snake snake_game.cpp
 *   Linux / macOS   : g++ -o snake snake_game.cpp
 *   Then run        : snake.exe  (Windows)  or  ./snake
 *
 *  CONTROLS : W = Up  |  S = Down  |  A = Left  |  D = Right
 *             ENTER   = Start Game
 *             Q       = Quit
 * ============================================================
 *
 *  OOP CONCEPTS USED:
 *   - Classes & Objects  : Point, Food, Snake, ScoreManager,
 *                          SnakeGame
 *   - Encapsulation      : Private data members, public methods
 *   - Abstraction        : Each class hides its internal logic
 *
 *  DATA STRUCTURES USED:
 *   - Point body[]       : Fixed-size array to store snake body
 *   - Array-shift movement: shift right (grow/slide)
 *   - 2D char array      : Grid buffer for rendering
 *
 *  ALGORITHMS IMPLEMENTED:
 *   - Grid Traversal     : Snake moves cell-by-cell on 2D grid
 *   - Collision Detection: Wall (O1) and Self (On) checks
 *   - Dynamic Body Update: Grow on food, slide otherwise
 * ============================================================
 */

// ─────────────────────────────────────────────────────────────
//  HEADERS  (only what is actually needed)
// ─────────────────────────────────────────────────────────────
#include <iostream>   // cout, cin
#include <cstdlib>    // rand(), srand(), system()
#include <ctime>      // time() — random seed

// ── Platform-specific headers for real-time input ────────────
#ifdef _WIN32
    #include <conio.h>    // _kbhit(), _getch()
    #include <windows.h>  // Sleep(), system("cls")
    #define CLEAR        "cls"
    #define SLEEP(ms)    Sleep(ms)
    #define KEY_HIT      _kbhit()
    #define GET_KEY      _getch()
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
    #define CLEAR        "clear"
    #define SLEEP(ms)    usleep((ms) * 1000)

    static struct termios _orig_termios;
    static bool           _termios_saved = false;

    inline void enableRawMode() {
        if (!_termios_saved) {
            tcgetattr(STDIN_FILENO, &_orig_termios);
            _termios_saved = true;
        }
        struct termios raw = _orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }

    inline void disableRawMode() {
        if (_termios_saved)
            tcsetattr(STDIN_FILENO, TCSANOW, &_orig_termios);
    }

    inline int kbhit_posix() {
        char ch;
        int n = read(STDIN_FILENO, &ch, 1);
        if (n > 0) { ungetc(ch, stdin); return 1; }
        return 0;
    }

    inline char getch_posix() {
        char ch = 0;
        read(STDIN_FILENO, &ch, 1);
        return ch;
    }

    #define KEY_HIT  kbhit_posix()
    #define GET_KEY  getch_posix()
#endif

using namespace std;

// ─────────────────────────────────────────────────────────────
//  CONSTANTS  (Programming Fundamentals: named constants)
// ─────────────────────────────────────────────────────────────
const int  WIDTH     = 40;       // grid columns
const int  HEIGHT    = 20;       // grid rows
const int  MAX_LEN   = WIDTH * HEIGHT; // maximum possible snake length
const int  SPEED_MS  = 120;      // milliseconds per frame
const char SNAKE_HEAD = 'O';     // head symbol
const char SNAKE_BODY = 'o';     // body symbol
const char FOOD_SYMBOL = '$';    // food symbol
const char WALL_H     = '-';     // horizontal wall
const char WALL_V     = '|';     // vertical wall

// ─────────────────────────────────────────────────────────────
//  ENUM — Direction  (PF: enumeration type)
// ─────────────────────────────────────────────────────────────
enum Direction { STOP = 0, UP, DOWN, LEFT, RIGHT };

// ─────────────────────────────────────────────────────────────
//  STRUCT — Point
//  Represents a single (x, y) cell on the 2D grid
// ─────────────────────────────────────────────────────────────
struct Point {
    int x, y;

    Point(int x = 0, int y = 0) : x(x), y(y) {}

    // Overloaded == operator — used in collision detection
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

// ─────────────────────────────────────────────────────────────
//  CLASS — Food
//  Encapsulates food position and random respawn logic
// ─────────────────────────────────────────────────────────────
class Food {
private:
    Point position;

public:
    Food() { respawn(); }

    /*
     * respawn() — Grid Traversal Algorithm
     * Randomly places food at a valid (unoccupied) cell
     * Time Complexity: O(n)  — checks all snake segments
     */
    void respawn(const Point* occupied = nullptr, int count = 0) {
        bool valid = false;
        while (!valid) {
            position.x = rand() % (WIDTH  - 2) + 1;
            position.y = rand() % (HEIGHT - 2) + 1;
            valid = true;
            for (int i = 0; i < count; i++) {
                if (occupied[i] == position) { valid = false; break; }
            }
        }
    }

    Point getPosition() const { return position; }
};

// ─────────────────────────────────────────────────────────────
//  CLASS — ScoreManager
//  Manages current score and all-time high score
// ─────────────────────────────────────────────────────────────
class ScoreManager {
private:
    int score;
    int highScore;

public:
    ScoreManager() : score(0), highScore(0) {}

    void addPoints(int pts = 10) {
        score += pts;
        if (score > highScore) highScore = score;
    }

    void reset()          { score = 0; }
    int  getScore() const { return score; }
    int  getHigh()  const { return highScore; }
};

// ─────────────────────────────────────────────────────────────
//  CLASS — Snake
//  Core Data Structure: fixed-size Point array with shift movement
//
//  MOVEMENT MODEL:
//    body[0]        = HEAD
//    body[length-1] = TAIL
//    Grow  → increment length, shift all right, insert new head
//    Slide → shift all right, insert new head (length unchanged)
// ─────────────────────────────────────────────────────────────
class Snake {
private:
    Point     body[MAX_LEN];  // DS: fixed-size array for all segments
    int       length;         // current number of segments
    Direction dir;            // current movement direction

public:
    Snake() { reset(); }

    /*
     * reset() — initialise snake at grid centre, length = 3
     * Time Complexity: O(1)
     */
    void reset() {
        int cx = WIDTH  / 2;
        int cy = HEIGHT / 2;
        body[0] = Point(cx,     cy);   // head
        body[1] = Point(cx - 1, cy);   // body
        body[2] = Point(cx - 2, cy);   // tail
        length = 3;
        dir = RIGHT;
    }

    /*
     * move() — Dynamic Body Update Algorithm
     * Steps:
     *   1. If growing, increment length first
     *   2. Shift all segments one position to the right
     *   3. Compute new head from direction and place at body[0]
     * Time Complexity: O(n) — shifting n elements
     */
    void move(bool grew) {
        if (grew) length++;            // grow: make room for new head

        // Shift entire body one slot to the right
        for (int i = length - 1; i > 0; i--)
            body[i] = body[i - 1];

        // Compute and place new head
        Point newHead = body[1];      // old head (now at index 1)
        switch (dir) {
            case UP:    newHead.y--; break;
            case DOWN:  newHead.y++; break;
            case LEFT:  newHead.x--; break;
            case RIGHT: newHead.x++; break;
            default:    break;
        }
        body[0] = newHead;
    }

    /*
     * hitWall() — Wall Collision Detection
     * Time Complexity: O(1)
     */
    bool hitWall() const {
        const Point& h = body[0];
        return (h.x <= 0 || h.x >= WIDTH - 1 ||
                h.y <= 0 || h.y >= HEIGHT - 1);
    }

    /*
     * hitSelf() — Self Collision Detection
     * Time Complexity: O(n) — linear scan of body
     */
    bool hitSelf() const {
        for (int i = 1; i < length; i++) {
            if (body[i] == body[0]) return true;
        }
        return false;
    }

    /*
     * ateFood() — Check if head overlaps food position
     * Time Complexity: O(1)
     */
    bool ateFood(const Point& food) const {
        return body[0] == food;
    }

    /*
     * setDirection() — Prevent 180-degree reversal
     */
    void setDirection(Direction d) {
        if ((d == UP    && dir != DOWN)  ||
            (d == DOWN  && dir != UP)    ||
            (d == LEFT  && dir != RIGHT) ||
            (d == RIGHT && dir != LEFT))
            dir = d;
    }

    Direction        getDirection() const { return dir; }
    const Point&     getHead()      const { return body[0]; }
    const Point*     getBody()      const { return body; }
    int              getLength()    const { return length; }
};

// ─────────────────────────────────────────────────────────────
//  CLASS — SnakeGame  (Main Controller / Aggregator)
//  Aggregates Snake + Food + ScoreManager via composition
//  Game loop pattern: setup() → [input() → logic() → draw()]
// ─────────────────────────────────────────────────────────────
class SnakeGame {
private:
    Snake        snake;
    Food         food;
    ScoreManager scores;
    bool         gameOver;
    bool         quit;

    char grid[HEIGHT][WIDTH + 1];

    /*
     * buildGrid() — Grid Traversal Algorithm
     * Fills 2D char array with walls, food, and snake
     * Time Complexity: O(WIDTH * HEIGHT + n)
     */
    void buildGrid() {
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                if (y == 0 || y == HEIGHT - 1)
                    grid[y][x] = WALL_H;
                else if (x == 0 || x == WIDTH - 1)
                    grid[y][x] = WALL_V;
                else
                    grid[y][x] = ' ';
            }
            grid[y][WIDTH] = '\0';
        }

        Point fp = food.getPosition();
        grid[fp.y][fp.x] = FOOD_SYMBOL;

        const Point* body = snake.getBody();
        int len = snake.getLength();
        for (int i = 1; i < len; i++)
            grid[body[i].y][body[i].x] = SNAKE_BODY;

        Point hp = snake.getHead();
        grid[hp.y][hp.x] = SNAKE_HEAD;
    }

public:
    SnakeGame() : gameOver(false), quit(false) {
        srand((unsigned)time(nullptr));
    }

    void setup() {
        snake.reset();
        food.respawn(snake.getBody(), snake.getLength());
        scores.reset();
        gameOver = false;
        quit     = false;
    }

    void draw() {
        system(CLEAR);
        buildGrid();

        for (int y = 0; y < HEIGHT; y++)
            cout << grid[y] << "\n";

        cout << "\n";
        cout << "  Score     : " << scores.getScore() << "\n";
        cout << "  High Score: " << scores.getHigh()  << "\n";
        cout << "  Length    : " << snake.getLength()  << "\n";
        cout << "\n  Controls  : W=Up  S=Down  A=Left  D=Right  Q=Quit\n";
    }

    void input() {
        if (!KEY_HIT) return;
        char key = (char)GET_KEY;

        switch (tolower(key)) {
            case 'w': snake.setDirection(UP);    break;
            case 's': snake.setDirection(DOWN);  break;
            case 'a': snake.setDirection(LEFT);  break;
            case 'd': snake.setDirection(RIGHT); break;
            case 'q': quit = true;               break;
            default:  break;
        }
    }

    /*
     * logic() — Process one game tick
     * Time Complexity per tick: O(n)
     */
    void logic() {
        bool grew = snake.ateFood(food.getPosition());
        snake.move(grew);

        if (grew) {
            scores.addPoints(10);
            food.respawn(snake.getBody(), snake.getLength());
        }

        if (snake.hitWall() || snake.hitSelf())
            gameOver = true;
    }

    void drawGameOver() {
        system(CLEAR);
        cout << "\n\n";
        cout << "  +==============================+\n";
        cout << "  |         GAME  OVER           |\n";
        cout << "  +==============================+\n";
        cout << "  |  Score     : " << scores.getScore() << "\n";
        cout << "  |  High Score: " << scores.getHigh()  << "\n";
        cout << "  |  Length    : " << snake.getLength() << "\n";
        cout << "  +==============================+\n\n";
        cout << "  Play again? (Y / N): ";
    }

    void run() {
#ifndef _WIN32
        enableRawMode();
#endif
        bool playAgain = true;

        while (playAgain) {
            setup();

            while (!gameOver && !quit) {
                draw();
                input();
                logic();
                SLEEP(SPEED_MS);
            }

            if (quit) break;

            drawGameOver();

#ifdef _WIN32
            while (_kbhit()) _getch();
#endif
            char ch;
            cin >> ch;
            playAgain = (tolower(ch) == 'y');
        }

#ifndef _WIN32
        disableRawMode();
#endif
        cout << "\n  Thanks for playing! Final High Score: "
             << scores.getHigh() << "\n\n";
    }
};

// ─────────────────────────────────────────────────────────────
//  MAIN — Entry Point
// ─────────────────────────────────────────────────────────────
int main() {
    cout << "  +======================================+\n";
    cout << "  |        S N A K E  G A M E           |\n";
    cout << "  |   OOP + Data Structures in C++      |\n";
    cout << "  +======================================+\n";
    cout << "  |  Controls : W = Up    S = Down       |\n";
    cout << "  |             A = Left  D = Right      |\n";
    cout << "  |             Q = Quit                 |\n";
    cout << "  +======================================+\n\n";
    cout << "  Press ENTER to start...";
    cin.get();

    SnakeGame game;
    game.run();

    return 0;
}

/*
 * ============================================================
 *  PSEUDOCODE
 * ============================================================
 *
 *  SETUP:
 *    snake ← [centre, centre-1, centre-2]   (initial body)
 *    direction ← RIGHT
 *    score ← 0
 *    place food at random unoccupied cell
 *
 *  GAME LOOP (each tick, O(n)):
 *    read keyboard → update direction
 *    newHead ← currentHead + directionVector
 *    IF newHead == foodPosition:
 *        length++
 *        shift body right (all segments move one slot back)
 *        body[0] ← newHead   (snake grows)
 *        score += 10
 *        respawn food
 *    ELSE:
 *        shift body right (length unchanged — tail slot overwritten)
 *        body[0] ← newHead   (snake slides)
 *    IF newHead hits wall OR newHead in body[1..n-1]:
 *        gameOver ← true
 *    render grid
 *    sleep(SPEED_MS)
 *
 * ============================================================
 *  COMPLEXITY SUMMARY
 * ============================================================
 *
 *  Operation               Time       Space
 *  ──────────────────────  ─────────  ──────
 *  Snake movement (shift)  O(n)       O(1)
 *  Wall collision check    O(1)       O(1)
 *  Self collision check    O(n)       O(1)
 *  Food spawn (worst case) O(n)       O(1)
 *  Grid render             O(W × H)   O(W × H)
 *
 *  Overall per tick → O(n) where n = snake body length
 *
 * ============================================================
 *  EMPIRICAL TEST RESULTS
 * ============================================================
 *
 *  Snake Length (n) | Comparisons Needed | Behaviour
 *  ─────────────────────────────────────────────────────────
 *  Small  (n =  5)  |  ~5  comparisons   | Very fast
 *  Medium (n = 20)  |  ~20 comparisons   | Moderate
 *  Large  (n = 50)  |  ~50 comparisons   | Slightly slower
 *
 *  Conclusion: Linear O(n) growth confirmed for collision
 *  detection, consistent with theoretical analysis.
 *
 * ============================================================
 */
