#include "raylib.h"
#include "raymath.h"

int main(void) {
    
    InitWindow(800, 450, "My Game");

    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Raylib Says Hello!", 304, 200, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;

}