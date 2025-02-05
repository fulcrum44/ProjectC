/*******************************************************************************************
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   This example has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    int screenWidth = 1200;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "GTA VI");

    SetTargetFPS(100);

    float x_verde=100;
    float x_rojo=100;
    int velocidad;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------
        velocidad=0;
        if(IsKeyDown(KEY_LEFT)) velocidad-=10;
        if(IsKeyDown(KEY_RIGHT)) velocidad+=10;
        x_verde+=velocidad*GetFrameTime();
        x_rojo+=velocidad;
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(WHITE);

            DrawRectangle(x_verde,200,50,50, GREEN);
            DrawRectangle(x_rojo,400,50,50, RED);
            DrawFPS(0,0);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
