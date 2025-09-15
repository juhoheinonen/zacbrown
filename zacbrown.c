#include "raylib.h"
//#include <stdio.h>
#include "raymath.h"        // Required for: Vector2Clamp()

#define MAX(a, b) ((a)>(b)? (a) : (b))
#define MIN(a, b) ((a)<(b)? (a) : (b))

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 2048;
    const int screenHeight = 1600;

    // Enable config flags for resizable window and vertical synchro
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);    
    InitWindow((float)GetScreenWidth(), (float)GetScreenHeight(), "Zachary Brownie - sprite animation");
    SetWindowMinSize(320, 240);

    int gameScreenWidth = 640;
    int gameScreenHeight = 480;

    // Render texture initialization, used to hold the rendering result so we can easily resize it
    RenderTexture2D target = LoadRenderTexture(gameScreenWidth, gameScreenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);  // Texture scale filter to use    

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D brownie_standing = LoadTexture("img/brownie.png"); // Texture loading
    Texture2D brownie_running = LoadTexture("img/brownie_running.png"); // Texture loading

    Vector2 position_standing = {350.0f, 150.0f};
    Rectangle static_rec = {0.0f, 0.0f, (float)brownie_standing.width, (float)brownie_standing.height};

    Vector2 position = {350.0f, 280.0f};
    Rectangle frameRec = {0.0f, 0.0f, (float)brownie_running.width / 6, (float)brownie_running.height};
    int currentFrame = 0;

    int framesCounter = 0;
    int framesSpeed = 8; // Number of spritesheet frames shown by second

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        float scale = MIN((float)GetScreenWidth()/gameScreenWidth, (float)GetScreenHeight()/gameScreenHeight);

        // Update
        //----------------------------------------------------------------------------------
        framesCounter++;

        if (framesCounter >= (60 / framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;

            if (currentFrame > 5)
                currentFrame = 0;

            frameRec.x = (float)currentFrame * (float)brownie_running.width / 6;
        }

        // Control frames speed
        if (IsKeyPressed(KEY_RIGHT))
            framesSpeed++;
        else if (IsKeyPressed(KEY_LEFT))
            framesSpeed--;

        if (framesSpeed > MAX_FRAME_SPEED)
            framesSpeed = MAX_FRAME_SPEED;
        else if (framesSpeed < MIN_FRAME_SPEED)
            framesSpeed = MIN_FRAME_SPEED;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);            

            ClearBackground(RAYWHITE);
            DrawTextureRec(brownie_standing, static_rec, position_standing, WHITE);

            DrawTextureRec(brownie_running, frameRec, position, WHITE); // Draw part of the texture

            DrawText("(c) Zachary Brownie sprite by Juho Antti Heinonen", screenWidth - 400, screenHeight - 20, 10, GRAY);

        EndTextureMode();

        BeginDrawing();
            ClearBackground(BLACK);
            DrawTexturePro(target.texture, (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
                           (Rectangle){ (GetScreenWidth() - ((float)gameScreenWidth*scale))*0.5f, (GetScreenHeight() - ((float)gameScreenHeight*scale))*0.5f,
                           (float)gameScreenWidth*scale, (float)gameScreenHeight*scale }, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();

        //----------------------------------------------------------------------------------        
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(brownie_running); // Texture unloading

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}