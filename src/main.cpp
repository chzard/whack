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

//DEFINES

//GLOBAL VARIABLES

//CONSTANTS
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GRID_WIDTH = 600;
const int GRID_HEIGHT = 300;
const int SQUARE_SIZE = 100;
const int GRID_MARGIN_LEFT = 100;
const int GRID_MARGIN_RIGHT = 100;
const int GRID_MARGIN_TOP = 200;
const int GRID_MARGIN_BOTTOM = 100;

//GLOBAL
bool gameOver = false;

int framesLitTime[3][6];
bool framesLit[3][6];

Vector2 mousePos = {0,0};

//debug

//FUNCTIONS
void initGameScreen(void);
void updateGame(void);
void drawGrid(void);
void drawGameScreen(void);
void updateFrame(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
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

    for (int i=0; i<3; i++) {
        for (int j=0; j<6; j++) {
            framesLitTime[i][j] = 0;
            framesLit[i][j] = false;
        }
    } 

}

void updateGame(void) {

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        mousePos = GetMousePosition();
        // If mouse is in the grid region
        if (mousePos.x >= GRID_MARGIN_LEFT && 
            mousePos.x <= SCREEN_WIDTH-GRID_MARGIN_RIGHT && 
            mousePos.y >= GRID_MARGIN_TOP && 
            mousePos.y <= SCREEN_HEIGHT-GRID_MARGIN_BOTTOM) {
            
            int sqX = (mousePos.y - GRID_MARGIN_TOP) / SQUARE_SIZE;
            int sqY = (mousePos.x - GRID_MARGIN_BOTTOM) / SQUARE_SIZE;   
            
            framesLit[sqX][sqY] = true;
        }
    }

    for (int i=0; i<3; i++) {
        for (int j=0; j<6; j++) {

            if (framesLitTime[i][j] == 90){
                framesLit[i][j] = false;
                framesLitTime[i][j] = 0;
            }
            
            if (framesLit[i][j]) {
                framesLitTime[i][j]++;
            }
        }
    }

}

void drawGrid(void) {
    // HORIZONTAL
    for (int i=GRID_MARGIN_TOP; i<=SCREEN_HEIGHT-GRID_MARGIN_BOTTOM; i += SQUARE_SIZE) {
        DrawLineV((Vector2){GRID_MARGIN_LEFT, i}, (Vector2){SCREEN_WIDTH-GRID_MARGIN_RIGHT, i}, GetColor(0x4b7696));
    }
    //VERTICAL
    for (int i=GRID_MARGIN_LEFT; i<=SCREEN_WIDTH-GRID_MARGIN_RIGHT; i += SQUARE_SIZE) {
        DrawLineV((Vector2){i, GRID_MARGIN_TOP}, (Vector2){i, SCREEN_HEIGHT-GRID_MARGIN_BOTTOM}, GetColor(0x4b7696));
    }
}

void drawGameScreen(void) {
    //CLEAR SCREEN
    
    drawGrid();

    // check for frames that should be lit up
    for (int i=0; i<3; i++) {
        for (int j=0; j<6; j++) {
            if (framesLit[i][j]) {
                //Draw square
                DrawRectangleV(
                    (Vector2){GRID_MARGIN_LEFT + (j * SQUARE_SIZE), GRID_MARGIN_TOP + (i*SQUARE_SIZE)},
                    (Vector2){SQUARE_SIZE, SQUARE_SIZE},
                    GetColor(0xeb643b)
                );
            }
        }
    }
}

void updateFrame(void) {
    updateGame();
    drawGameScreen();
}