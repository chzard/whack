/*******************************************************************************************
*  whac-a-what?
*  @author: chzard8 (on github)
* @version: 
********************************************************************************************/

#include "../include/raylib.h"
#include <string>
#include <algorithm>
#include <vector>
using namespace std;

//DEFINES

// Corresponding actions & nums
#define NO_ACTION 0
#define LIT 1
#define INC_CLICK 2
#define STANDBY 3
#define COOL 4
#define SB_CLICK 5

//GLOBAL VARIABLES

//CONSTANTS
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 700;

const int GRID_WIDTH = 600;
const int GRID_HEIGHT = 300;
const int PLANE_LINE_SPACE = 10;
const int SQUARE_SIZE = 100;

const int GRID_MARGIN_LEFT = 100;
const int GRID_MARGIN_RIGHT = 100;
const int GRID_MARGIN_TOP = 100;
const int GRID_MARGIN_BOTTOM = 300;

const int SQUARESX = GRID_WIDTH / SQUARE_SIZE;      // Num of squares per row
const int SQUARESY = GRID_HEIGHT / SQUARE_SIZE;     // Num of squares per column

const int LIT_LIMIT = 5;                            // Max amount of lit squares at any time
const int STANDBY_LIMIT = 6;                        // Max amount of 'standby' squares at any time
const int SPAWN_MAX = 3;                            // Max amount of LIT squares that can spawn after cooldown
const int SB_SPAWN_MAX = 4;                         // Max amoutn of 'standby' squares that can spawn at once
const int DESPAWN_MAX = 4;                          // Max amount of 'standby' squares that can despawn at once
const int LIT_TIME = 150;                           // Frames = 150 = 2.5 seconds 
const int COOLDOWN = 180;                           // Frames = 180 = 3 seconds
const int SPAWN_COOLDOWN = 30;                      // Frames = 30 = 0.5 seconds

const int MAX_LIVES = 10;

const Color PLANE_LINES_COLOR = ColorFromHSV(220, 0.1, 0.6);
const Color GRID_LINES_COLOR = ColorFromHSV(260, 0.58, 0.25);
const Color LIVES_COLOR = ColorFromHSV(359, 0.75, 0.8);
const Color CURSOR_SHADOW = {82,83,107,90};
const Color SQUARE_COLOR = ColorFromHSV(125, 0.7, 0.7);
const Color SQUARE_WR_COLOR = ColorFromHSV(5, 0.7, 0.85);
const Color SQUARE_SB_COLOR = ColorFromHSV(45, 0.78, 0.85);

//GLOBAL

int lives = MAX_LIVES;
int score = 0;

bool gameStart = true;
bool gameOver = false;

int framesLitNum = 0;
int framesStandbyNum = 0;
int squaresSpawnCooldown = 0;

Vector2 mousePos = {0,0};

//FUNCTIONS

void initGameScreen(void);
void updateGame(void);
void updateFrame(void);

void drawPlane(void);
void drawGrid(void);
void drawLives(void);
void drawSquares(void);
void drawSprite(int x, int y, Texture2D draw);
void drawCursorShadow(void);
void drawGameScreen(void);
void drawEndScreen(void);
void drawStartScreen(void);

void chooseRandomSprite(int x, int y);
void spawnSquare(int act);
void despawnSquare(int x, int y);
void RESET_ALL(void);

struct fishSprite {
    Texture2D alive;
    Texture2D dead;
    Texture2D dirty;
    Texture2D missing;
};

// TEXTURES

fishSprite FISH1;
fishSprite FISH2;
fishSprite SPECIALFISH1;

//fishSprite sprites[3];

struct {
    int action;
    int actionTime;
    fishSprite fishType;
} squares[SQUARESY][SQUARESX];

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    // Initialization
    //--------------------------------------------------------------------------------------

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "dirty gloves");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // INITIALIZE MUSIC
    InitAudioDevice();
    Music TEST_BGM = LoadMusicStream("../assets/bgm/Test_song.mp3");
    TEST_BGM.looping = true;
    PlayMusicStream(TEST_BGM);

    //  LOAD TEXTURES

    FISH1.alive = LoadTexture("../assets/sprites/fish/1/alive.PNG");
    FISH1.dead = LoadTexture("../assets/sprites/fish/1/dead.PNG");
    FISH1.dirty = LoadTexture("../assets/sprites/fish/1/dirty.PNG");
    FISH1.missing = LoadTexture("../assets/sprites/fish/1/missing.PNG");

    FISH2.alive = LoadTexture("../assets/sprites/fish/2/alive.PNG");
    FISH2.dead = LoadTexture("../assets/sprites/fish/2/dead.PNG");
    FISH2.dirty = LoadTexture("../assets/sprites/fish/2/dirty.PNG");
    FISH2.missing = LoadTexture("../assets/sprites/fish/2/missing.PNG");

    SPECIALFISH1.alive = LoadTexture("../assets/sprites/spec/1/alive.PNG");
    SPECIALFISH1.dead = LoadTexture("../assets/sprites/spec/1/dead.PNG");
    SPECIALFISH1.dirty = LoadTexture("../assets/sprites/spec/1/dirty.PNG");
    SPECIALFISH1.missing = LoadTexture("../assets/sprites/spec/1/missing.PNG");

    // sprites[0]=FISH1;
    // sprites[1]=FISH2;
    // sprites[2]=SPECIALFISH1;

    initGameScreen();
    gameStart = false;

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        if (gameStart && !gameOver) {UpdateMusicStream(TEST_BGM);}

        BeginDrawing();
            ClearBackground(RAYWHITE);
            updateFrame();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    UnloadMusicStream(TEST_BGM);
    CloseAudioDevice();

    UnloadTexture(FISH1.alive);
    UnloadTexture(FISH1.dead);
    UnloadTexture(FISH1.dirty);
    UnloadTexture(FISH1.missing);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void initGameScreen(void) {
    RESET_ALL();

    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            chooseRandomSprite(j,i);
        }
    }
    // PSA: if you see a cs major, pet it (it's probably in desperate need of petting)
    // watch house md you freaky little herpeee
        
    //INITIALIZE: SPAWN MAX AMOUNT OF LIT SQUARES
    for (int i=0; i<LIT_LIMIT-2; i++) {spawnSquare(LIT);}
    for (int i=0; i<STANDBY_LIMIT-2; i++) {spawnSquare(STANDBY);}
    squaresSpawnCooldown=SPAWN_COOLDOWN;
    lives = MAX_LIVES;

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

            if (squares[sqY][sqX].action == LIT) {
                framesLitNum--;
                despawnSquare(sqX, sqY);
            }
            else {
                if (squares[sqY][sqX].action == STANDBY) {
                    framesStandbyNum--;
                    squares[sqY][sqX].action = SB_CLICK;
                }
                else if (squares[sqY][sqX].action != SB_CLICK) {
                    squares[sqY][sqX].action = INC_CLICK;
                }
                squares[sqY][sqX].actionTime = 0;
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
            else if (squares[i][j].action == COOL) {
                if (squares[i][j].actionTime == 0) {
                    squares[i][j].action = NO_ACTION;
                }
                else {squares[i][j].actionTime--;}
            }
            else {
                squares[i][j].actionTime++;
                if (squares[i][j].actionTime > LIT_TIME){
                    if (squares[i][j].action == LIT) {
                        lives--;
                        if (lives == 0) {
                            gameOver = true;
                            return;
                        }
                        framesLitNum--;
                        despawnSquare(j,i);
                    }
                    else if (squares[i][j].action == STANDBY) {
                        int gen = GetRandomValue(10, framesStandbyNum*10);
                        if (gen > (framesStandbyNum*5)) {
                            framesStandbyNum--;
                            despawnSquare(j,i);
                        }
                    }
                    else {despawnSquare(j,i);}
                }
            }
        }
    }

    if (!squaresSpawnCooldown) {
        if (framesLitNum<LIT_LIMIT) {
            int TIMES = GetRandomValue(1, min(SPAWN_MAX, LIT_LIMIT - framesLitNum));
            for (int i=0; i<TIMES; i++) {spawnSquare(LIT);}
        }
        if (framesStandbyNum<STANDBY_LIMIT) {
            int TIMES = GetRandomValue(1, min(SB_SPAWN_MAX, STANDBY_LIMIT - framesStandbyNum));
            for (int i=0; i<TIMES; i++) {spawnSquare(STANDBY);}
        }
        
        squaresSpawnCooldown = SPAWN_COOLDOWN;
    }

}

void drawGameScreen(void) {
    //drawPlane();
    drawSquares();
    drawLives();
    //drawGrid();
    //drawCursorShadow();
}

void drawEndScreen(void) {
    drawGameScreen();
    DrawText("GAME OVER", 165, SCREEN_HEIGHT/2 - 75/2, 75, BLACK);
    DrawText("press enter to play again", 220, SCREEN_HEIGHT/2 + 50 - 10, 20, BLACK);
}

void drawStartScreen(void) {
    drawPlane();
    DrawText("whac-a-what?", 150, SCREEN_HEIGHT/2 - 75/2, 75, BLACK);
    DrawText("press enter to play", 220, SCREEN_HEIGHT/2 + 50 - 10, 20, BLACK);
}

void updateFrame(void) {
    mousePos = GetMousePosition();
    drawCursorShadow();
    if (!gameStart) {

        if (IsKeyPressed(KEY_ENTER)) {
            gameStart=true;
        }
        else {drawStartScreen(); return;}
    }
    
    if (gameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            gameOver=false;
            initGameScreen();
        }
        else {
            drawEndScreen(); return;
        }
    }
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
        DrawCircle(GRID_MARGIN_LEFT + (GRID_WIDTH / (2 * lives)) + (i * GRID_WIDTH / lives), SCREEN_HEIGHT-(GRID_MARGIN_BOTTOM/2), 20, LIVES_COLOR);
    }
}

void drawSquares(void) {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if (squares[i][j].action == LIT) {drawSprite(j,i, squares[i][j].fishType.dirty);}
            else if (squares[i][j].action == STANDBY) {drawSprite(j,i, squares[i][j].fishType.alive);}
            else if (squares[i][j].action == INC_CLICK) {drawSprite(j,i, squares[i][j].fishType.missing);}
            else if (squares[i][j].action == SB_CLICK) {drawSprite(j,i, squares[i][j].fishType.dead);}
        }
    }
}

void drawSprite(int x, int y, Texture2D draw) {
    DrawTexture(draw, GRID_MARGIN_LEFT + (x * SQUARE_SIZE), GRID_MARGIN_TOP + (y*SQUARE_SIZE), WHITE);
}

void drawCursorShadow(void) {
    DrawCircle(mousePos.x, mousePos.y, (SQUARE_SIZE/4)+10, CURSOR_SHADOW);
}

// ---------------------------------
// UPDATE HELPERS

void chooseRandomSprite(int x, int y) {
    int gen = GetRandomValue(0, 100);
    if (gen < 40) {
        squares[y][x].fishType = FISH1;
    }
    else if (gen < 80) {
        squares[y][x].fishType = FISH2;
    }
    else {
        squares[y][x].fishType = SPECIALFISH1;
    }
}

void spawnSquare(int act) {
    for (int i=0; i<(2*SQUARESX*SQUARESY); i++) {
        int ran = GetRandomValue(0,(SQUARESX*SQUARESY)-1);
        int sqY = ran/SQUARESX, sqX = ran%SQUARESX;
        if (squares[sqY][sqX].action == NO_ACTION) {
            squares[sqY][sqX].action = act;
            if (act == STANDBY) {
                framesStandbyNum++;
            }
            if (act == LIT) {
                framesLitNum++;
            }
            return;
        } 
    }
}

void despawnSquare(int x, int y) {
    squares[y][x].action = COOL;
    squares[y][x].actionTime = COOLDOWN;

    // CHOOSE A RANDOM SPRITE PACK
    chooseRandomSprite(x,y);
}


void RESET_ALL() {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            squares[i][j].action = NO_ACTION;
            squares[i][j].actionTime = 0;
        }
    }
    framesLitNum=0;
    framesStandbyNum=0;
}