/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "../include/raylib.h"
#include <algorithm>
#include <format>
using namespace std;

//DEFINES

//GLOBAL VARIABLES

//CONSTANTS
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const int GRID_WIDTH = 600;
const int GRID_HEIGHT = 300;
const int PLANE_LINE_SPACE = 10;
const int SQUARE_SIZE = 100;

const int GRID_MARGIN_LEFT = 100;
const int GRID_MARGIN_RIGHT = 100;
const int GRID_MARGIN_TOP = 200;
const int GRID_MARGIN_BOTTOM = 100;

const int SQUARESX = GRID_WIDTH / SQUARE_SIZE;  // Num of squares per row
const int SQUARESY = GRID_HEIGHT / SQUARE_SIZE; // Num of squares per column

const int LIT_LIMIT = 5;                        // Max amount of lit squares at any frame
const int SPAWN_MAX = 3;                        // Max amount of squares that can spawn after cooldown
const int LIT_TIME = 150;                       // Frames = 150 = 2.5 seconds 
const int COOLDOWN = 180;                       // Frames = 180 = 3 seconds
const int SPAWN_COOLDOWN = 45;                  // Frames = 60 = 0.75 seconds

const Color PLANE_LINES_COLOR = ColorFromHSV(220, 0.1, 0.6);
const Color GRID_LINES_COLOR = ColorFromHSV(260, 0.58, 0.25);
const Color LIVES_COLOR = ColorFromHSV(359, 0.75, 0.8);
const Color CURSOR_SHADOW = {82,83,107,90};

const int INIT_SQ_HUE = 125;
const double INIT_SQ_SAT = 0.70;
const double INIT_SQ_VAL = 0.70;

const int INIT_WRSQ_HUE = 5;
const double INIT_WRSQ_SAT = 0.70;
const double INIT_WRSQ_VAL = 0.85;

const Color SQUARE_COLOR = ColorFromHSV(INIT_SQ_HUE, INIT_SQ_SAT, INIT_SQ_VAL);
const Color SQUARE_WR_COLOR = ColorFromHSV(INIT_WRSQ_HUE, INIT_WRSQ_SAT, INIT_WRSQ_VAL);

// LOAD IMAGING

//const Image TEST_SPRITE_GREEN = LoadImage("../assets/sprites/TestSpriteGreen_scaled.png");

//GLOBAL

int lives = 5;
int score = 0;

bool gameStart = false;
bool gameOver = false;

bool framesLit[SQUARESY][SQUARESX];             // Stores by row x column
bool framesWrLit[SQUARESY][SQUARESX];           // Stores by row x column
int framesLitTime[SQUARESY][SQUARESX];          // Stores by row x column
int framesCooldown[SQUARESY][SQUARESX];        // Stores by row x column

//CONDENSED
int framesAction[SQUARESY][SQUARESX];           // 0 = None, 1 = Incorrectly Clicked, 2 = Lit, 3 = 
int framesSprite[SQUARESY][SQUARESX];           // 0 = None, 1 = Baby, 2 = Pitcher, 3 = Vase
int framesActionTime[SQUARESY][SQUARESY];

int framesLitNum = 0;
int squaresSpawnCooldown = 0;

// COLOR CHANGES: OPTIONAL

Vector2 mousePos = {0,0};

//FUNCTIONS

void initGameScreen(void);
void updateGame(void);
void updateFrame(void);

void drawPlane(void);
void drawGrid(void);
void drawLives(void);
void drawSquares(void);
void drawCursorShadow(void);
//void drawGameStartScreen(void);
void drawGameScreen(void);
void drawEndScreen(void);
void displayStats(void);

void spawnSquare(void);
void resetSquare(int x, int y, bool wr);

// bool isSquareInactive(int x, int y);
// bool isSquareLit(int x, int y);
// bool isSquareWrLit(int x, int y);
// bool isSquareFade(int x, int y);
// bool isSquareCooldown(int x, int y);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "whac-a-what?");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    initGameScreen();

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            if (!gameStart) {}
            ClearBackground(RAYWHITE);
            updateFrame();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void initGameScreen(void) {

    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            framesLitTime[i][j]=0;
            framesCooldown[i][j]=0;
            framesLit[i][j]=false;
            framesWrLit[i][j]=false;
            framesLitNum=0;
        }
    } 
    //INITIALIZE: SPAWN MAX AMOUNT OF SQUARES
    for (int i=0; i<LIT_LIMIT; i++) {spawnSquare();}
}

void updateGame(void) {
    mousePos = GetMousePosition();
    if (squaresSpawnCooldown) {squaresSpawnCooldown--;}
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // If mouse is in the grid region
        if (mousePos.x >= GRID_MARGIN_LEFT && 
            mousePos.x <= SCREEN_WIDTH-GRID_MARGIN_RIGHT && 
            mousePos.y >= GRID_MARGIN_TOP && 
            mousePos.y <= SCREEN_HEIGHT-GRID_MARGIN_BOTTOM) {
            
            int sqX = (mousePos.y - GRID_MARGIN_TOP) / SQUARE_SIZE;
            int sqY = (mousePos.x - GRID_MARGIN_BOTTOM) / SQUARE_SIZE;   
            
            //if (isSquareLit(sqX, sqY)) {resetSquare(sqX, sqY, false);}
            if (framesLit[sqX][sqY]) {resetSquare(sqX,sqY,false);}
            else {
                framesWrLit[sqX][sqY] = true;
                lives--;
                if (lives == 0) {
                    gameOver = true;
                    return;
                }
            }
        }
    }

    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if (framesCooldown[i][j]) {framesCooldown[i][j]--;}
            if (framesLit[i][j] || framesWrLit[i][j]) {
                framesLitTime[i][j]++;
                if (framesLit[i][j] && framesLitTime[i][j] > LIT_TIME){
                    lives--;
                    if (lives == 0) {
                        gameOver = true;
                        return;
                    }
                    resetSquare(i,j,false);
                }
                if (framesWrLit[i][j] && framesLitTime[i][j] > LIT_TIME) {
                    resetSquare(i,j,true);
                }
            }
        }
    }

    if (!squaresSpawnCooldown) {
        for (int i=0; i<min(SPAWN_MAX, LIT_LIMIT-framesLitNum); i++) { 
            spawnSquare();
        }
        squaresSpawnCooldown = SPAWN_COOLDOWN;
    }

}

void drawGameScreen(void) {
    drawPlane();
    drawSquares();
    drawLives();
    drawGrid();
    drawCursorShadow();
    // displayStats();
}

void drawEndScreen(void) {
    drawGameScreen();
    DrawText("GAME OVER", 200, SCREEN_HEIGHT/2, 75, BLACK);
}

void updateFrame(void) {
    if (gameOver) {drawEndScreen(); drawCursorShadow(); return;}
    updateGame();
    drawGameScreen();
}

// ---------------------------------
// DRAWING HELPERS

void drawPlane(void) {
    for (int i=0; i<SCREEN_WIDTH; i+=PLANE_LINE_SPACE) {
        DrawLineEx((Vector2){i, 0}, (Vector2){i,SCREEN_HEIGHT}, 1,  PLANE_LINES_COLOR);
    }
    for (int i=0; i<SCREEN_HEIGHT; i+=PLANE_LINE_SPACE) {
        DrawLineEx((Vector2){0,i}, (Vector2){SCREEN_WIDTH,i}, 1, PLANE_LINES_COLOR);
    }
}

void drawGrid(void) {
    // HORIZONTAL
    for (int i=GRID_MARGIN_TOP; i<=SCREEN_HEIGHT-GRID_MARGIN_BOTTOM; i += SQUARE_SIZE) {
        DrawLineEx((Vector2){GRID_MARGIN_LEFT, i}, (Vector2){SCREEN_WIDTH-GRID_MARGIN_RIGHT, i}, 2.5, GRID_LINES_COLOR);
    }
    //VERTICAL
    for (int i=GRID_MARGIN_LEFT; i<=SCREEN_WIDTH-GRID_MARGIN_RIGHT; i += SQUARE_SIZE) {
        DrawLineEx((Vector2){i, GRID_MARGIN_TOP}, (Vector2){i, SCREEN_HEIGHT-GRID_MARGIN_BOTTOM}, 2.5, GRID_LINES_COLOR);
    }
}

void drawLives(void) {
    for (int i=0; i<lives; i++) {
        DrawCircle(GRID_MARGIN_LEFT + (GRID_WIDTH / (2 * lives)) + (i * GRID_WIDTH / lives), GRID_MARGIN_TOP/2, 50, LIVES_COLOR);
    }
    return;
}

void drawSquares(void) {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if (framesLit[i][j] || framesWrLit[i][j]) {
                //Draw square
                DrawRectangleV(
                    (Vector2){GRID_MARGIN_LEFT + (j * SQUARE_SIZE), GRID_MARGIN_TOP + (i*SQUARE_SIZE)},
                    (Vector2){SQUARE_SIZE, SQUARE_SIZE},
                    (framesLit[i][j] ? SQUARE_COLOR : SQUARE_WR_COLOR)
                );
            }
        }
    }
}

void displayStats(void) {
    DrawText(TextFormat("FRAMES LIT NUM: %d", framesLitNum), 50, 25, 10, PLANE_LINES_COLOR);
    DrawText(TextFormat("SQUARE SPAWN COOLDOWN: %d", squaresSpawnCooldown), 200, 25, 10, PLANE_LINES_COLOR);
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            DrawText(TextFormat("(L: %d, LT: %d, CD: %d)", framesLit[i][j], framesLitTime[i][j], framesCooldown[i][j]), 20 + (j * 120), 50 + (i * 50), 10, GetColor(0x0b0d40));
        }
    }
}

void drawCursorShadow(void) {
    DrawCircle(mousePos.x, mousePos.y, (SQUARE_SIZE/2)+10, CURSOR_SHADOW);
}

// ---------------------------------
// UPDATE HELPERS

void spawnSquare(void) {
    while (true) {
        int ran = GetRandomValue(0,(SQUARESX*SQUARESY)-1);
        if (!framesLit[(int)ran/6][ran%6] && !framesWrLit[(int)ran/6][ran%6] && !framesCooldown[(int)ran/6][ran%6]) {
            framesLit[(int)ran/6][ran%6] = true;
            framesLitNum++;
            return;
        } 
    }
}

void resetSquare(int x, int y, bool wr) {
    framesLit[x][y] = false;
    framesWrLit[x][y] = false;
    framesLitTime[x][y] = 0;
    framesCooldown[x][y] = COOLDOWN;
    if (!wr) {framesLitNum--;}
}

bool isSquareInactive(int x, int y) {
    return (framesAction[x][y]==0);
}

bool isSquareLit(int x, int y) {
    return (framesAction[x][y]==1) && (framesActionTime[x][y]>0);
}

bool isSquareWrLit(int x, int y) {
    return (framesAction[x][y]==2) && (framesActionTime[x][y]>0);
}

bool isSquareFade(int x, int y) {
    return (framesAction[x][y]==3) && (framesActionTime[x][y]>0);
}

bool isSquareCooldown(int x, int y) {
    return (framesAction[x][y]==4) && (framesActionTime[x][y]>0);
}