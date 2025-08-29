/*******************************************************************************************
*  whac-a-mole, but slightly different :)
*  @author: chzard8 (on github)
*
********************************************************************************************/

#include "../include/raylib.h"
#include <string>
#include <algorithm>
using namespace std;

//DEFINES

// Corresponding actions & nums
#define NO_ACTION 0
#define LIT 1
#define INC_CLICK 2
#define STANDBY 3
#define COOL 4

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
const int STANDBY_LIMIT = 8;
const int SPAWN_MAX = 4;                        // Max amount of squares that can spawn after cooldown
const int LIT_TIME = 150;                       // Frames = 150 = 2.5 seconds 
const int COOLDOWN = 180;                       // Frames = 180 = 3 seconds
const int SPAWN_COOLDOWN = 30;                  // Frames = 30 = 0.5 seconds

const Color PLANE_LINES_COLOR = ColorFromHSV(220, 0.1, 0.6);
const Color GRID_LINES_COLOR = ColorFromHSV(260, 0.58, 0.25);
const Color LIVES_COLOR = ColorFromHSV(359, 0.75, 0.8);
const Color CURSOR_SHADOW = {82,83,107,90};
const Color SQUARE_COLOR = ColorFromHSV(125, 0.7, 0.7);
const Color SQUARE_WR_COLOR = ColorFromHSV(5, 0.7, 0.85);
const Color SQUARE_SB_COLOR = ColorFromHSV(45, 0.78, 0.85);

// LOAD IMAGING

//const Image TEST_SPRITE_GREEN = LoadImage("../assets/sprites/TestSpriteGreen_scaled.png");

//GLOBAL

int lives = 5;
int score = 0;

bool gameStart = true;
bool gameOver = false;

int framesLitNum = 0;
int framesStandbyNum = 0;
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
void drawGameScreen(void);
void drawEndScreen(void);

void spawnSquare(int act);
void resetSquare(int x, int y, int act);
void RESET_ALL(void);

struct {
    int action;
    int actionTime;
    string sprite;
} squares[SQUARESY][SQUARESX];

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "im gonna fucking kill myself");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    initGameScreen();

    // INITIALIZE MUSIC
    InitAudioDevice();
    Music TEST_BGM = LoadMusicStream("../assets/bgm/Test_song.mp3");
    TEST_BGM.looping = true;
    PlayMusicStream(TEST_BGM);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        UpdateMusicStream(TEST_BGM);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            updateFrame();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadMusicStream(TEST_BGM);
    CloseAudioDevice();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void initGameScreen(void) {
    RESET_ALL();

    // PSA: if you see a cs major, pet it (it's probably in desperate need of petting)
    // watch house md you freaky little herpeee
        
    //INITIALIZE: SPAWN MAX AMOUNT OF LIT SQUARES
    for (int i=0; i<LIT_LIMIT; i++) {spawnSquare(LIT);}
}

void updateGame(void) {
    if (squaresSpawnCooldown) {squaresSpawnCooldown--;}

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        // If mouse is in the grid region
        if (mousePos.x >= GRID_MARGIN_LEFT && 
            mousePos.x <= SCREEN_WIDTH-GRID_MARGIN_RIGHT && 
            mousePos.y >= GRID_MARGIN_TOP && 
            mousePos.y <= SCREEN_HEIGHT-GRID_MARGIN_BOTTOM) {
            
            int sqX = (mousePos.x - GRID_MARGIN_LEFT) / SQUARE_SIZE;   
            int sqY = (mousePos.y - GRID_MARGIN_TOP) / SQUARE_SIZE;

            if (squares[sqY][sqX].action == LIT) {resetSquare(sqX, sqY, LIT);}
            else {
                squares[sqY][sqX].action = INC_CLICK;
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
            if (squares[i][j].action == NO_ACTION) {continue;}
            if (squares[i][j].action == COOL) {
                if (squares[i][j].actionTime == 0) {squares[i][j].action = NO_ACTION;}
                else {squares[i][j].actionTime--;}
            }
            else {
                squares[i][j].actionTime++;
                if (squares[i][j].actionTime > LIT_TIME){
                    resetSquare(i, j, squares[i][j].action);
                    if (squares[i][j].action == LIT) {
                        lives--;
                        if (lives == 0) {
                            gameOver = true;
                            return;
                        }
                    }
                    squares[i][j].action = COOL;
                    squares[i][j].actionTime = COOLDOWN;
                }
            }
        }
    }

    if (!squaresSpawnCooldown) {
        int LS = min(GetRandomValue(0,SPAWN_MAX), LIT_LIMIT - framesLitNum); 
        int SBS = min(SPAWN_MAX - LS, STANDBY_LIMIT - framesStandbyNum);
        for (int i=0; i<LS; i++) {spawnSquare(LIT);}
        for (int i=0; i<SBS; i++) {spawnSquare(STANDBY);}
        squaresSpawnCooldown = SPAWN_COOLDOWN;
    }

}

void drawGameScreen(void) {
    drawPlane();
    drawSquares();
    drawLives();
    drawGrid();
    drawCursorShadow();
}

void drawEndScreen(void) {
    drawGameScreen();
    DrawText("GAME OVER", 200, SCREEN_HEIGHT/2, 75, BLACK);
}

void updateFrame(void) {
    mousePos = GetMousePosition();
    if (gameOver) {drawEndScreen(); return;}
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
}

void drawSquares(void) {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if (squares[i][j].action != NO_ACTION && squares[i][j].action != COOL) {
                //Draw square
                DrawRectangleV(
                    (Vector2){GRID_MARGIN_LEFT + (j * SQUARE_SIZE), GRID_MARGIN_TOP + (i*SQUARE_SIZE)},
                    (Vector2){SQUARE_SIZE, SQUARE_SIZE},
                    (squares[i][j].action == LIT ? 
                        SQUARE_COLOR : 
                        (squares[i][j].action == INC_CLICK ? 
                            SQUARE_WR_COLOR : 
                            SQUARE_SB_COLOR)
                    )
                );
            }
        }
    }
}

void drawCursorShadow(void) {
    DrawCircle(mousePos.x, mousePos.y, (SQUARE_SIZE/3)+10, CURSOR_SHADOW);
}

// ---------------------------------
// UPDATE HELPERS

void spawnSquare(int act) {
    for (int i=0; i<(SQUARESX * SQUARESY); i++) {
        int ran = GetRandomValue(0,(SQUARESX*SQUARESY)-1);
        if (squares[(int)ran/6][ran%6].action == 0) {
            squares[(int)ran/6][ran%6].action = act;
            if (act == LIT) {framesLitNum++;}
            if (act == STANDBY) {framesStandbyNum++;}
            return;
        } 
    }
}

void resetSquare(int x, int y, int act) {
    squares[y][x].action = NO_ACTION;
    squares[y][x].actionTime = 0;
    if (act == LIT) {framesLitNum--;}
    else if (act == STANDBY) {framesStandbyNum--;}
}

void RESET_ALL() {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            squares[i][j].action = NO_ACTION;
            squares[i][j].actionTime = 0;
            squares[i][j].sprite = "NONE";
        }
    }
    framesLitNum=0;
    framesStandbyNum=0;
}