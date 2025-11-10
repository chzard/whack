/*******************************************************************************************
*  pet the cs major (dream sweet in c# major) aAAaaAAaaAaaAaaAAAAaloneeee at the edge of a universe humming a tune for merely dreaming we were snow a siren sounds like the goddess who promises endless apologies of paradise and only she can make it right we'll live forever tonight we'll go together in flight instrumental break 
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
const int SCREEN_HEIGHT = 720;

// const int GRID_WIDTH = 600;
// const int GRID_HEIGHT = 300;
const int PLANE_LINE_SPACE = 10;
const int SQUARE_SIZE = 100;

const int GRID_MARGIN_LEFT = 50;
const int GRID_MARGIN_RIGHT = 50;
const int GRID_MARGIN_TOP = 50;
const int GRID_MARGIN_BOTTOM = 210;

const int CELL_PADDING = 20;
// const int SQUARESX = GRID_WIDTH / SQUARE_SIZE;      // Num of squares per row
// const int SQUARESY = GRID_HEIGHT / SQUARE_SIZE;     // Num of squares per column
const int SQUARESX = 6;      // Num of squares per row
const int SQUARESY = 4;     // Num of squares per column

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
bool helpScreen = false;

int framesLitNum = 0;
int framesStandbyNum = 0;
int squaresSpawnCooldown = 0;

Vector2 mousePos = {0,0};
Vector2 clickedSquare = {0,0};

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
void drawTextCenter(string text, int size, int y, Color color);

void drawGameScreen(void);
void drawEndScreen(void);
void drawStartScreen(void);
void drawHelpScreen(void);

void chooseRandomSprite(int x, int y);
void spawnSquare(int act);
void despawnSquare(int x, int y);
void RESET_ALL(void);

bool anySquaresClicked(void);
Vector2 getClickedSquare();

struct fishSprite {
    Texture2D alive;
    Texture2D dead;
    Texture2D dirty;
    Texture2D missing;
    int points;
};

// fISH & TEXTURES
fishSprite FISH1;
fishSprite FISH2;
fishSprite SPECIALFISH1;
fishSprite SPECIALFISH2;

Texture2D LIFE;

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

    SPECIALFISH2.alive = LoadTexture("../assets/sprites/spec/2/alive.PNG");
    SPECIALFISH2.dead = LoadTexture("../assets/sprites/spec/2/dead.PNG");
    SPECIALFISH2.dirty = LoadTexture("../assets/sprites/spec/2/dirty.PNG");
    SPECIALFISH2.missing = LoadTexture("../assets/sprites/spec/2/missing.PNG");

    LIFE = LoadTexture("../assets/sprites/game/life.png");

    // INITIALIZE POINT VALUES
    FISH1.points = 1;
    FISH2.points = 1;
    SPECIALFISH1.points = 5;
    SPECIALFISH2.points = 5;

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

    UnloadTexture(FISH2.alive);
    UnloadTexture(FISH2.dead);
    UnloadTexture(FISH2.dirty);
    UnloadTexture(FISH2.missing);

    UnloadTexture(SPECIALFISH1.alive);
    UnloadTexture(SPECIALFISH1.dead);
    UnloadTexture(SPECIALFISH1.dirty);
    UnloadTexture(SPECIALFISH1.missing);

    UnloadTexture(SPECIALFISH2.alive);
    UnloadTexture(SPECIALFISH2.dead);
    UnloadTexture(SPECIALFISH2.dirty);
    UnloadTexture(SPECIALFISH2.missing);

    UnloadTexture(LIFE);

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

}

void updateGame(void) {

    if (squaresSpawnCooldown) {squaresSpawnCooldown--;}

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        if (anySquaresClicked()) {
            
            int sqX = clickedSquare.x-1;   
            int sqY = clickedSquare.y-1;

            if (squares[sqY][sqX].action == LIT) {
                framesLitNum--;
                score += squares[sqY][sqX].fishType.points;
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
            if (squares[i][j].action == NO_ACTION) {
                continue;
            }
            if (squares[i][j].action == COOL) {
                if (squares[i][j].actionTime == 0) {
                    squares[i][j].action = NO_ACTION;
                }
                else {squares[i][j].actionTime--;}
                continue;
            }
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
                    int gen = GetRandomValue(0, framesStandbyNum*10);
                    if (framesStandbyNum > 2 && gen > (framesStandbyNum*5)) {
                        framesStandbyNum--;
                        despawnSquare(j,i);
                    }
                }
                else {despawnSquare(j,i);}
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
    // SCORE
    drawTextCenter(TextFormat("Score: %d", score), 20, SCREEN_HEIGHT - (2 * GRID_MARGIN_BOTTOM/3), BLACK);
    //drawGrid();
    //drawCursorShadow();
}

void drawEndScreen(void) {
    drawGameScreen();

    drawTextCenter("GAME OVER", 100, SCREEN_HEIGHT/2 - 100, BLACK);
    drawTextCenter("PRESS ENTER TO PLAY AGAIN", 30, SCREEN_HEIGHT/2, BLACK);
}

void drawStartScreen(void) {
    //drawPlane();

    drawTextCenter("what-a-what?", 75, SCREEN_HEIGHT/2 - 70, BLACK);
    drawTextCenter("PRESS ENTER TO PLAY", 25, SCREEN_HEIGHT/2 + 40, BLACK);
    drawTextCenter("PRESS H FOR HELP", 25, SCREEN_HEIGHT/2 + 75, BLACK);
}

void drawHelpScreen(void) {
    drawTextCenter("HOW TO PLAY", 50, 50, BLACK);
    drawTextCenter("PRESS FISH WITH BUGS ON THEM", 25, 125, BLACK);
    drawTextCenter("you get 10 lives. pressing a normal fish will grant you 1 point,", 20, 200, BLACK);
    drawTextCenter("while pressing special fish (with yellow glow!) will grant you 5 points.", 20, 230, BLACK);
    drawTextCenter("pinching a clean fish or grabbing no fish will cost you a life.", 20, 260, BLACK);
    drawTextCenter("press r while playing to restart the game. ", 20, 290, BLACK);
    
    drawTextCenter("PRESS R TO RETURN TO START SCREEN", 15, SCREEN_HEIGHT-30, BLACK);
}

void updateFrame(void) {
    mousePos = GetMousePosition();
    clickedSquare = getClickedSquare();
    drawCursorShadow();
    if (helpScreen) {
        if (IsKeyPressed(KEY_R)) {
            helpScreen=false;
            drawStartScreen();
        }
        else {
            drawHelpScreen(); 
        }
        return;
    }
    if (!gameStart) {
        if (IsKeyPressed(KEY_ENTER)) {
            gameStart=true;
        }
        else if (IsKeyPressed(KEY_H)) {
            drawHelpScreen(); helpScreen = true; return;
        }
        else {drawStartScreen(); return;}
    }
    if (gameOver) {
        if (IsKeyPressed(KEY_ENTER)) {
            gameOver=false;
            initGameScreen();
        }
        else {drawEndScreen(); return;}
    }
    if (IsKeyPressed(KEY_R)) {
        initGameScreen();
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
    for (int i=GRID_MARGIN_TOP; i<=SCREEN_HEIGHT-GRID_MARGIN_BOTTOM; i += CELL_PADDING+SQUARE_SIZE) {
        DrawLineEx((Vector2){GRID_MARGIN_LEFT, i}, (Vector2){SCREEN_WIDTH-GRID_MARGIN_RIGHT, i}, 2.5, RED);
        DrawLineEx((Vector2){GRID_MARGIN_LEFT, i+SQUARE_SIZE}, (Vector2){SCREEN_WIDTH-GRID_MARGIN_RIGHT, i+SQUARE_SIZE}, 2.5, RED);
    }
    //VERTICAL
    for (int i=GRID_MARGIN_LEFT; i<=SCREEN_WIDTH-GRID_MARGIN_RIGHT; i += SQUARE_SIZE+CELL_PADDING) {
        DrawLineEx((Vector2){i, GRID_MARGIN_TOP}, (Vector2){i, SCREEN_HEIGHT-GRID_MARGIN_BOTTOM}, 2.5, RED);
        DrawLineEx((Vector2){i+SQUARE_SIZE, GRID_MARGIN_TOP}, (Vector2){i+SQUARE_SIZE, SCREEN_HEIGHT-GRID_MARGIN_BOTTOM}, 2.5, RED);
    }
}

void drawLives(void) {
    for (int i=0; i<lives; i++) {
        DrawTexture(LIFE, GRID_MARGIN_LEFT + 55 + (i * 62), SCREEN_HEIGHT-(GRID_MARGIN_BOTTOM/3), WHITE);
    }
}

void drawrect(int x, int y, Color color) {
    DrawRectangle(GRID_MARGIN_LEFT + (x * (SQUARE_SIZE+CELL_PADDING)), GRID_MARGIN_TOP + (y*(SQUARE_SIZE+CELL_PADDING)), SQUARE_SIZE, SQUARE_SIZE, color);
}

void drawSquares(void) {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if      (squares[i][j].action == LIT)       {drawSprite(j,i, squares[i][j].fishType.dirty);}
            else if (squares[i][j].action == STANDBY)   {drawSprite(j,i, squares[i][j].fishType.alive); }
            else if (squares[i][j].action == INC_CLICK) {drawSprite(j,i, squares[i][j].fishType.missing);}
            else if (squares[i][j].action == SB_CLICK)  {drawSprite(j,i, squares[i][j].fishType.dead);}
        }
    }
}

void drawSprite(int x, int y, Texture2D draw) {
    DrawTexture(draw, GRID_MARGIN_LEFT + (x * (SQUARE_SIZE+CELL_PADDING)), GRID_MARGIN_TOP + (y*(SQUARE_SIZE+CELL_PADDING)), WHITE);
}

void drawCursorShadow(void) {
    DrawCircle(mousePos.x, mousePos.y, (SQUARE_SIZE/4)+10, CURSOR_SHADOW);
}

void drawTextCenter(string text, int size, int y, Color color) {
    DrawText(
        text.c_str(),
        (SCREEN_WIDTH - MeasureText(text.c_str(), size)) / 2, 
        y, 
        size,
        color
    );
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
    lives = MAX_LIVES; 
    score = 0;
}

bool anySquaresClicked() {
    return (clickedSquare.x && clickedSquare.y);
}

Vector2 getClickedSquare() {
    for (int i=0; i<SQUARESY; i++) {
        for (int j=0; j<SQUARESX; j++) {
            if (mousePos.x >= (GRID_MARGIN_LEFT + ((SQUARE_SIZE + CELL_PADDING) * j)) && 
                mousePos.x <= (GRID_MARGIN_LEFT + (SQUARE_SIZE*(j+1)) + (CELL_PADDING*j)) && 
                mousePos.y >= (GRID_MARGIN_TOP + ((SQUARE_SIZE+CELL_PADDING)*i)) && 
                mousePos.y <= (GRID_MARGIN_TOP + (SQUARE_SIZE*(i+1)) + (CELL_PADDING * i))
            )
            return (Vector2){j+1, i+1};
        }
    }
    return (Vector2){0,0};
}