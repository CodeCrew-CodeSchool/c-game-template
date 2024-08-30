/*******************************************************************************************
*
*   raylib [core] example - 2D Camera platformer
*
*   Example originally created with raylib 2.5, last time updated with raylib 3.0
*
*   Example contributed by arvyy (@arvyy) and reviewed by Ramon Santamaria (@raysan5)
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2019-2024 arvyy (@arvyy)
*
********************************************************************************************/
//DONE
//jump kinda works
//keep camera

//TODO
//main - loads screenmanager -> loads each platform level
//bitcoin power icon in corner
//map as 0,1 tileset
//tile convert rendering
//settle on camera mode
//1 good level
//random level generation?
//emscripten tutorial
//make inscription
//submit buidl

//IDEA
//Enemy is an Shady Organization - Robots - Eagles, Soldiers, CorporateSuits, MegaEagleRobotInSuitWithGavel
//level 1 White - seemingly plain, deceptively evil. coerced the world. lift the veil - Eagle Bots
//Blue - seemingly peaceful, actually forceful. must be stopped - CorporateSuits
//Red - bloody violent, absolute power. How can we overcome it? - Soldiers
//Green - Final boss, the one to rule them all. infinite slavery - MegaEagleRoboSuitWithGavel

//Player 
//Orange - You, the hero, the difference. Orange Pill the world

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
// #include "string.h"

//#define PLATFORM_WEB
#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//platfomer tile collision config

bool PLATFORMER = true; // toggle to top-down movement with TAB

//tile collision types
#define EMPTY 0
#define BLOCK 1
typedef struct{
    float x;
    float y;
    int w;      // width
    int h;      // height
    int s;      // cell size (square cells)
    int *cell;  // array of tiles
}Grid;

// List of Rectangles to check for a collision.
typedef struct{
    Rectangle *rect;
    int size;
}RectList;

//old player
typedef struct Player {
    Vector2 position;
    float speed;
    bool canJump;
} Player;

//keeping EnvItem for Backdrop and camera positioning. rebind camera to map?
typedef struct EnvItem {
    Rectangle rect;
    int blocking;
    Color color;
    int isWall;
} EnvItem;

typedef enum GameScreen { LOGO = 0, TITLE, LVL1, LVL2, LVL3, LVL4, ENDING, GAMEOVER } GameScreen;
GameScreen currentScreen = LOGO;
bool nextLevel = false;
int framesCounter = 0; 

//method declarations
void GameInit();
void GameUpdate();
void GameDraw();
//void GameLoop(){GameUpdate(); GameDraw();}
void Reset();

void DrawTileGrid();
void DrawTileMap();
void DrawCoins();
void DrawPlayer();
void DrawScoreText();

void UpdateScreen();
void UpdatePlayer();
void UpdateCoin();

void UpdateCameraCenterInsideMap(Camera2D *camera, float delta, int width, int height);

void ScreenManagerUpdate();
void ScreenManagerDraw();


//collision methods
void        RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity);
Rectangle   RectangleResize(Rectangle *rect, Vector2 *size);
RectList*   RectangleListFromTiles(Rectangle *rect, Grid *grid);
void        RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list);

//map rendering config. would like to array/enumify this to reuse the variables and make shift levels simpler
#define MAP_W 45
#define MAP_H 16
int screenWidth = 32*MAP_W;
int screenHeight = 32*MAP_H;
const int gameWidth = 32*MAP_W;
const int gameHeight = 32*MAP_H;
const int myW = 1280;
const int myH = 896;
//RenderTexture viewport;
int scale = 1;
Vector2 vpOffset = (Vector2){0.0f, 0.0f};

Rectangle player = {32.0f * 2, 32.0f * 8, 32.0f, 32.0f}; //what are the rectangle components?


#define COIN_COUNT 21
Rectangle coins[COIN_COUNT] = {0};
bool visible[COIN_COUNT] = {0};
int points = 0;
int time = 0;       // For animation
float location = 1.5f;
Rectangle goals[4] = {0};

#define G 800
#define PLAYER_JUMP_SPD 400.0f
#define PLAYER_HOR_SPD 200.0f



Grid map;

//10 x 32 width
//by
//7 x 32 height

//320 x 224
//1280 x 896

int tiles1[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,9,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,9,0,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
int tiles2[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
int tiles3[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};
int tiles4[] = {
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
void (*CameraUpdate)(Camera2D*, float, int, int) = {
        UpdateCameraCenterInsideMap
    };
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "BFT - Bitcoin Fixed This");

    //create camera
    Camera2D camera = { 0 };
    camera.target = (Vector2){(int)player.x, (int)player.y};
    camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        //----------------------------------------------------------------------------------
        
        //screenmanager switch
        ScreenManagerUpdate();

        //update camera bound to game world bounds
        camera.offset = (Vector2){ screenWidth/2, screenHeight/2 };
        camera.target = (Vector2){ player.x, player.y };

        float minX = 0, minY = 0, maxX = gameWidth, maxY = gameHeight;
        
        minX = fminf(0, minX);
        maxX = fmaxf(0 + gameWidth, maxX);
        minY = fminf(0, minY);
        maxY = fmaxf(0 + gameHeight, maxY);

        Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, camera);
        Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, camera);

        if (max.x < screenWidth) {
            camera.offset.x = screenWidth - (max.x - screenWidth/2);
        }
        if (max.y < screenHeight) {
            camera.offset.y = screenHeight - (max.y - screenHeight/2);
        }
        if (min.x > 0) {
            camera.offset.x = screenWidth/2 - min.x;
        }
        if (min.y > 0) {
            camera.offset.y = screenHeight/2 - min.y;
        }
        //end camera update
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(LIGHTGRAY);

            BeginMode2D(camera);

                ScreenManagerDraw();
                
                //DELETE
                // char cameraKnows[30];
                // sprintf(cameraKnows,"camera is @ (X: %.2f, Y: %.2f)", camera.offset.x, camera.offset.y);
                // DrawText(cameraKnows, 20, 185, 20, BLACK);

            EndMode2D();
            if (currentScreen == LVL1 || currentScreen == LVL2 || currentScreen == LVL3 || currentScreen == LVL4) {

                DrawText("Controls:", 20, 20, 10, DARKGRAY);
                DrawText("- Right/Left to move", 40, 40, 10, DARKGRAY);
                DrawText("- Space to jump", 40, 60, 10, DARKGRAY);
                DrawText("- Mouse Wheel to Zoom in-out, R to reset zoom", 40, 80, 10, DARKGRAY);
                
                char playerPosText[50];// = "Player X: &s" + player.position.x;
                sprintf(playerPosText, "Camera Offset X: %.2f, Camera Offset Y: %.2f", camera.offset.x, camera.offset.y);
                DrawText(playerPosText, 20, 160, 10, BLACK);
                // DrawText(playerPosText, 20 + camera.offset.x, 160 + camera.offset.y, 10, BLACK); //funny movement
                DrawScoreText();
            }
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}


//Game Methods

void GameInit() {
    
    //viewport = LoadRenderTexture(gameWidth, gameHeight);
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles1;
    Reset();
}
void Lvl2Init() {
    
    nextLevel = false;
    //viewport = LoadRenderTexture(gameWidth, gameHeight);
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles2;
    Reset();
}
void Lvl3Init() {
    
    nextLevel = false;
    //viewport = LoadRenderTexture(gameWidth, gameHeight);
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles3;
    Reset();
}
void Lvl4Init() {
   
    nextLevel = false;
    //viewport = LoadRenderTexture(gameWidth, gameHeight);
    map.x = 0.0f;
    map.y = 0.0f;
    map.w = MAP_W;
    map.h = MAP_H;
    map.s = 32;
    map.cell = tiles4;
    Reset();
}

void Reset(){
    const float s = 32.0f;
    player = (Rectangle){s, s * 6, s, s};
    points = 0;
    time = 0;
    
    for (int i = 0; i < COIN_COUNT; i++) {
        coins[i] = (Rectangle){s * location, s * (4 + (i % 4)), 10.0f, 10.0f};
        location += 2;
    }
    
    for (int i = 0; i < COIN_COUNT; i++){visible[i] = true;}

}

//may have to call the update methods separately to work with the screen manager

void GameUpdate(){
    
    UpdateScreen();// Adapt to resolution
    UpdatePlayer();
    UpdateCoin();
    //UpdateCamera(camera, player);
}

//Update Methods
void ScreenManagerUpdate(){
        //CameraUpdate(&camera, GetFrameTime(), screenWidth, screenHeight);

    switch(currentScreen)
        {
            case LOGO:
            {
                // TODO: Update LOGO screen variables here!
                
                framesCounter++;    // Count frames

                // Wait for 2 seconds (120 frames) before jumping to TITLE screen
                if (framesCounter > 180)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE:
            {
                // TODO: Update TITLE screen variables here!

                // Press enter to change to GAMEPLAY screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    GameInit();
                    currentScreen = LVL1;
                }
            } break;
            case LVL1:
            {
                // TODO: Update GAMEPLAY screen variables here!

                GameUpdate();

                if (nextLevel == true)
                {
                    currentScreen = LVL2;
                }
                // Press enter to change to ENDING screen
                if (IsKeyPressed(KEY_P) || IsGestureDetected(GESTURE_TAP))
                {
                    Lvl2Init();
                    currentScreen = LVL2;
                }
            } break;
            case LVL2:
            {
                GameUpdate();
                if (nextLevel == true)
                {
                    currentScreen = LVL3;
                }
                if (IsKeyPressed(KEY_P) || IsGestureDetected(GESTURE_TAP))
                {
                    Lvl3Init();
                    currentScreen = LVL3;
                }
            } break;
            case LVL3:
            {
                GameUpdate();
                if (nextLevel == true)
                {
                    currentScreen = LVL4;
                }
                if (IsKeyPressed(KEY_P) || IsGestureDetected(GESTURE_TAP))
                {
                    Lvl4Init();
                    currentScreen = LVL4;
                }
            } break;
            case LVL4:
            {   
                GameUpdate();
                if (nextLevel == true)
                {
                    currentScreen = ENDING;
                }
                if (IsKeyPressed(KEY_P) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = ENDING;
                }

            } break;
            case ENDING:
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                }
            } break;
            default: break;
        }
}

void UpdateScreen(){
    // Adapt to resolution
    if (IsWindowResized()){
        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();
        scale = MAX(1, MIN((screenWidth/gameWidth), (screenHeight/gameHeight)));
        vpOffset.x = (screenWidth - (gameWidth * scale)) / 2;
        vpOffset.y = (screenHeight - (gameHeight * scale)) / 2;
    }

}

void UpdatePlayer(){
    const float maxSpd = 6.0f;
    const float acc = 0.07f;
    const float grav = 0.6f;
    const float jmpImpulse = -15.0f;
    const int jmpBufferTime = 30;
    static bool isGrounded = false;
    static int jmpBuffer = 0;
    static int dirX = 0;
    static int dirY = 0;
    static Vector2 vel = {0};
    static Vector2 prevVel = {0};
    
    
    // INPUT
    dirX = (float)(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));// || (float)(IsKeyDown(KEY_LEFT) - IsKeyDown(KEY_RIGHT));
    dirY = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W));
    if(IsKeyPressed(KEY_TAB)){PLATFORMER = !PLATFORMER;}
    
    // HORIZONTAL SPEED
    vel.x += (dirX * maxSpd - vel.x) * acc;
    if (vel.x < -maxSpd){
        vel.x = -maxSpd;
    }
    else if (vel.x > maxSpd){
        vel.x = maxSpd;
    }
    
    // VERTICAL SPEED
    if (PLATFORMER){
        if (isGrounded && jmpBuffer != jmpBufferTime){
            jmpBuffer = jmpBufferTime;
        }
        if (isGrounded && IsKeyPressed(KEY_SPACE)){
            vel.y = jmpImpulse;
            jmpBuffer = 0;
        }
        else if (jmpBuffer > 0 && IsKeyPressed(KEY_SPACE)){
            vel.y = jmpImpulse;
            jmpBuffer = 0;
        }
        else{
            if (!IsKeyDown(KEY_SPACE) && vel.y < jmpImpulse * 0.2){
                vel.y = jmpImpulse * 0.2;
            }
            else{
                vel.y += grav;
                if (vel.y > -jmpImpulse){
                    vel.y = -jmpImpulse;
                }
                if (jmpBuffer > 0){
                    jmpBuffer -= 1;
                }
            }
        }
    }
    else{
        // TOP-DOWN
        vel.y += (dirY * maxSpd - vel.y) * acc;
        if (vel.y < -maxSpd){
            vel.y = -maxSpd;
        }
        else if (vel.y > maxSpd){
            vel.y = maxSpd;
        }
    }
    
    prevVel = vel;   // for ground check
    RectangleCollisionUpdate(&player, &vel);
    isGrounded = prevVel.y > 0.0f && vel.y <= 0.0001f;  // naive way to check grounded state
    //player.x += vel.x;
    //player.y += vel.y;
    if (CheckCollisionRecs(goals[currentScreen - 2], player)) {
        nextLevel = true;
    }
}

void UpdateCoin(){
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            //RectangleCollisionUpdate(&coins[i], &(Vector2){0,0}); trying to collide coin with ground to bounce it out
            if (CheckCollisionRecs(coins[i], player)){
                visible[i] = false;
                points += 1;
            }
        }
    }
    
    if (IsKeyPressed(KEY_ENTER)){
        Reset();
    }
}

void UpdateCameraCenterInsideMap(Camera2D *camera, float delta, int width, int height)
{
    camera->target = (Vector2){ player.x, player.y };
    camera->offset = (Vector2){ width/2.0f, height/2.0f };
    float minX = gameWidth, minY = gameHeight, maxX = -1 * gameWidth, maxY = -1 * gameHeight;

    Vector2 max = GetWorldToScreen2D((Vector2){ maxX, maxY }, *camera);
    Vector2 min = GetWorldToScreen2D((Vector2){ minX, minY }, *camera);

    if (max.x > width) camera->offset.x = width - (max.x - width/2);
    if (max.y > height) camera->offset.y = height - (max.y - height/2);
    if (min.x < 0) camera->offset.x = width/2 - min.x;
    if (min.y < 0) camera->offset.y = height/2 - min.y;
}

//Draw Methods
void ScreenManagerDraw() {
    ClearBackground(RAYWHITE);

            switch(currentScreen)
            {
                case LOGO:
                {
                    EndMode2D();
                    //Texture2D dij = LoadTexture("resources/logo.png");
                    // TODO: Draw LOGO screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    //DrawTexture(dij, 0, 0, BLACK);
                    DrawCircle(385, 225, 150.0f, BLACK);
                    DrawCircleLines(385, 225, 150.0f, WHITE);
                    DrawText("dij", 335, 190, 80, WHITE);
                    DrawText("built by", 85, screenHeight / 2 - 15, 35, RAYWHITE);
                    DrawText("WAIT for 3 SECONDS...", screenWidth/2.6, screenHeight - 100, 20, LIGHTGRAY);
                    //UnloadTexture(dij);
                } break;
                case TITLE:
                {
                    EndMode2D();
                    // TODO: Draw TITLE screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
                    DrawText("Bitcoin Fixes this", screenWidth/4 - 150, screenHeight/3, 40, DARKGREEN);
                    DrawText("PRESS ENTER or TAP to JUMP to PLAY", screenWidth/4 - 180, screenHeight/2, 20, DARKGREEN);

                } break;
                case LVL1:
                {
                    // TODO: Draw GAMEPLAY screen here!
                    
                    GameDraw();
                    
                    // DrawRectangle(0, 0, screenWidth, screenHeight, PURPLE);
                    // DrawText("GAMEPLAY SCREEN", 20, 20, 40, MAROON);
                    // DrawText("PRESS ENTER or TAP to JUMP to ENDING SCREEN", 130, 220, 20, MAROON);

                } break;
                case LVL2:
                {
                    GameDraw();
                } break;
                case LVL3:
                {
                    GameDraw();

                } break;
                case LVL4:
                {
                    GameDraw();
                } break;
                case ENDING:
                {
                    EndMode2D();
                    // TODO: Draw ENDING screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("ENDING SCREEN", screenWidth/4 - 100, screenHeight/3, 40, BLACK);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", screenWidth/4 - 200, screenHeight/2, 20, BLACK);

                } break;
                default: break;
            }
}

void GameDraw(){
    // Viewport scaling
    const Vector2 origin = (Vector2){0.0f, 0.0f};
    const Rectangle vp_r = (Rectangle){0.0f,gameHeight,gameWidth, -gameHeight}; // flip vertically: position = left-bottom
    Rectangle out_r = (Rectangle){vpOffset.x, vpOffset.y, gameWidth * scale, gameHeight * scale};
    
    // Render game's viewport
    //BeginTextureMode(viewport);
        Color currentBackgroundColor = { 0 };
        switch (currentScreen) {
            case LVL1: {
                currentBackgroundColor = RAYWHITE;
            } break;
            case LVL2: {
                currentBackgroundColor = BLUE;
            } break;
            case LVL3: {
                currentBackgroundColor = DARKGRAY;
            } break;
            case LVL4: {
                currentBackgroundColor = LIME;
            } break;
        }

        DrawRectangle(0, 0, gameWidth, gameHeight, currentBackgroundColor); // Background
        DrawTileMap();
        DrawTileGrid();
        //DrawScoreText();
        DrawCoins();
        DrawPlayer();
    //EndTextureMode();
    
    // Draw the viewport
    //BeginDrawing();
        ClearBackground(BLACK);
        //DrawTexturePro(viewport.texture, vp_r, out_r, origin, 0.0f, WHITE);
    //EndDrawing();
}

void DrawTileMap(){
    Color wallColor, accentColor;
    switch (currentScreen){
        case LVL1: {
            //white
            wallColor = LIME;
            accentColor = GREEN;
        } break;
        case LVL2: {
            //blue
            wallColor = LIME;
            accentColor = BROWN;
        } break;
        case LVL3: {
            //red
            wallColor = BROWN;
            accentColor = MAROON;
        } break;
        case LVL4: {
            //green
            wallColor = BLACK;
            accentColor = DARKGRAY;
        } break;
    }
    for (int y = 0; y < map.h; y++){
        for (int x = 0; x < map.w; x++){
            int i = x + y * map.w;
            int tile = map.cell[i];
            if (tile > 0){
                float cellX = (map.x + map.s * x);
                float cellY = (map.y + map.s * y);
                if (tile == 1) {
                    DrawRectangle((int)cellX, (int)cellY, map.s, map.s, wallColor);
                } else if (tile == 9) {
                    float y = (float)sin(2 * PI * ((time / 60) / 2) + (cellX * 5)) * 4; // pseudo random offset floating
                    float x = (float)sin(2 * PI * (time / 60 * 2)) * 4;
                    goals[currentScreen - 2] = (Rectangle){(int)(cellX + 4 + x * 0.5), (int)(cellY + y), (int)(32 - 4 - x), (int)32};
                    DrawRectangleRec(goals[currentScreen - 2], ORANGE);
                }
                // check tile above
                if (i - map.w >= 0 && !map.cell[i - map.w]){
                    DrawLineEx((Vector2){cellX, cellY + 3}, (Vector2){cellX + map.s, cellY + 3}, 6.0f, accentColor);
                }
            }
        }
    }
}

void DrawTileGrid(){
    Color c = (Color){255,255,255,25};
    
    for (int y = 0; y < map.h + 1; y++){
        int x1 = map.x;
        int x2 = map.x + map.w * map.s;
        int Y = map.y + map.s * y;
        DrawLine(x1, Y, x2, Y, c);
    }
    for (int x = 0; x < map.w + 1; x++){
        int y1 = map.y;
        int y2 = map.y + map.h * map.s;
        int X = map.x + map.s * x;
        DrawLine(X, y1, X, y2, c);
    }
}

void DrawPlayer(){
    DrawRectangle((int)player.x, (int)player.y, (int)player.width, (int)player.height, ORANGE);
    DrawRectangleLinesEx(player, 2, BLACK);
    
    // Artistic touch
    static int dirX = 0;
    dirX = (float)(IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) * 2;
    Vector2 L1 = (Vector2){player.x + 12 + dirX, player.y + 4}; //left eye
    Vector2 R1 = (Vector2){player.x + 20 + dirX, player.y + 4}; //right eye
    Vector2 L2 = L1;
    L2.y += 8;
    Vector2 R2 = R1;
    R2.y += 8;
    DrawLineEx(L1, L2, 2.0f, BLACK); //left eye
    DrawLineEx(R1, R2, 2.0f, BLACK); //right eye
}

void DrawCoins(){
    time += 1;
    
    for (int i = 0; i < COIN_COUNT; i++){
        if (visible[i]){
            Rectangle c = coins[i];
            float y = (float)sin(2 * PI * (time / 60.0f * 0.5) + (c.x * 5)) * 4; // pseudo random offset floating
            float x = (float)sin(2 * PI * (time / 60.0f * 2)) * 4;
            DrawRectangle((int)(c.x + 4 + x * 0.5), (int)(c.y + y), (int)(c.width - 4 - x), (int)c.height, GOLD);
        }
    }
}

void DrawScoreText(){
    const char *text;
    if (points == COIN_COUNT){
        text = TextFormat("Pres 'ENTER' to restart!");
    }
    else{
        text = TextFormat("Score: %d", points);
    }
    
    const int size = 24;
    int x = screenWidth /2 - MeasureText(text, size) / 2;
    int y = 48;
    
    DrawText(text, x, y+1, size, BLACK);
    DrawText(text, x, y, size, BLACK);
    
    
}

//collision methods
void RectangleCollisionUpdate(Rectangle *rect, Vector2 *velocity){
    Rectangle colArea = RectangleResize(rect, velocity); //collide Area
    RectList *tiles = RectangleListFromTiles(&colArea, &map);
    
    RectangleTileCollision(rect, velocity, tiles);
    // free allocated RectList memory
    MemFree(tiles->rect);
    MemFree(tiles);
}

Rectangle RectangleResize(Rectangle *rect, Vector2 *size){
    Rectangle r = (Rectangle){
        size->x > 0 ? rect->x : rect->x + size->x,
        size->y > 0 ? rect->y : rect->y + size->y,
        size->x > 0 ? rect->width + size->x : rect->width - size->x,
        size->y > 0 ? rect->height + size->y : rect->height - size->y
        };
    return r;
}

RectList* RectangleListFromTiles(Rectangle *rect, Grid *grid){
    float offX = rect->x - grid->x;
    float offY = rect->y - grid->y;
    float offXw = rect->x - grid->x + rect->width;
    float offYh = rect->y - grid->y + rect->height;
    // compensate flooring
    if (offX < 0.0f){offX -= grid->s;}
    if (offY < 0.0f){offY -= grid->s;}
    if (offXw < 0.0f){offXw -= grid->s;}
    if (offYh < 0.0f){offYh -= grid->s;}
    
    // grid coordinates
    int X = (int)(offX / grid->s);
    int Y = (int)(offY / grid->s);
    int sizeX = (int)(offXw / grid->s) - X + 1;
    int sizeY = (int)(offYh / grid->s) - Y + 1;
    
    RectList *list = MemAlloc(sizeof(RectList));
    list->rect = MemAlloc(sizeof(Rectangle) * sizeX * sizeY);
    list->size = 0;
    
    for (int y = Y; y < Y + sizeY; y++){
        if (y >= 0 && y < grid->h){
            for (int x = X; x < X + sizeX; x++){
                if (x >= 0 && x < grid->w){
                    int tile = grid->cell[x + y * grid->w];
                    if (tile){
                        list->rect[list->size] = (Rectangle){
                            grid->x + x * grid->s,
                            grid->y + y * grid->s,
                            grid->s,
                            grid->s
                            };
                        list->size += 1;
                    }
                }
            }
        }
    }
    return list;
}

void RectangleTileCollision(Rectangle *rect, Vector2 *velocity, RectList *list){
    Rectangle *a = rect;
    Rectangle *b;
    Rectangle c = (Rectangle){a->x + velocity->x, a->y, a->width, a->height};
    
    // Solve X axis separately
    for (int i = 0; i < list->size; i++){
        b = &list->rect[i]; // next collision Rectangle
        if (c.x == goals[currentScreen - 2].x && c.y == goals[currentScreen - 2].y && c.height == goals[currentScreen - 2].height && c.width == goals[currentScreen - 2].width) {
            continue;
        }
        if (CheckCollisionRecs(c, *b)) {
            // moving to the right
            if (velocity->x > 0.0f) {
                // adjust velocity 
                velocity->x = (b->x - a->width) - a->x;
            }
            // moving to the left
            else if (velocity->x < 0.0f) {
                velocity->x = (b->x + b->width) - a->x;
            }
        }
    }
    // set C to resolved X position
    c.x = a->x + velocity->x;
    
    // move on Y axis
    // set C on test Y position
    c.y += velocity->y;
    for (int i = 0; i < list->size; i++){
        b = &list->rect[i];
        if (CheckCollisionRecs(c, *b)) {
            // moving down
            if (velocity->y > 0.0f) {
                velocity->y = (b->y - a->height) - a->y;
            }
            // moving up
            else if (velocity->y < 0.0f) {
                velocity->y = (b->y + b->height) - a->y;
            }
        }
    }
    
    rect->x += velocity->x;
    
    rect->y += velocity->y;
}