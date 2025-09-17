#include "raylib.h"
// #include <stdio.h>
#include "raymath.h" // Required for: Vector2Clamp()

#include "custom_types.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

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
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Texture scale filter to use

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D brownie_standing = LoadTexture("img/brownie.png");        // Texture loading
    Texture2D brownie_running = LoadTexture("img/brownie_running.png"); // Texture loading

    // Vector2 position_standing = {350.0f, 150.0f};
    //  Rectangle static_rec = {0.0f, 0.0f, (float)brownie_standing.width, (float)brownie_standing.height};

    Vector2 position = {350.0f, 280.0f};
    Rectangle frameRec = {0.0f, 0.0f, (float)brownie_running.width / 6, (float)brownie_running.height};
    int currentFrame = 0;

    // initialize map here
    game_tile game_map[64][48];
    for (int x = 0; x < 64; x++)
    {
        for (int y = 0; y < 48; y++)
        {
            if (y >= 45)
            {
                game_map[x][y] = (game_tile){.x = x, .y = y, .tile_type = BROWN_GROUND};
            }
            else
            {
                game_map[x][y] = (game_tile){.x = x, .y = y, .tile_type = LIGHT_SKY};
            }
        }
    }

    main_character player_character = {.position = {1.0f, 6.0 * 64.0f}, .speed = 0, .direction = RIGHT};

    int framesCounter = 0;
    int framesSpeed = 10; // Number of spritesheet frames shown by second

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    float deltaTime = 0.0f;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {        
        deltaTime = GetFrameTime();

        float scale = MIN((float)GetScreenWidth() / gameScreenWidth, (float)GetScreenHeight() / gameScreenHeight);

        if (IsKeyDown(KEY_RIGHT))
        {
            player_character.speed = 5;
            player_character.direction = RIGHT;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            player_character.speed = 5;
            player_character.direction = LEFT;
        }
        else
        {
            player_character.speed = 0;
        }

        // check if player feet hit blocking tile
        if (player_character.position.y) {

        }

        // todo: check if player would hit wall


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

        Texture2D brown_ground_texture = LoadTexture("img/brown_ground.png");
        Texture2D light_sky_texture = LoadTexture("img/light_sky.png");

        // Draw
        //----------------------------------------------------------------------------------
        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);

        ClearBackground(RAYWHITE);

        // Draw the map tiles
        for (int x = 0; x < 64; x++)
        {
            for (int y = 0; y < 48; y++)
            {
                Texture2D tile_texture;
                if (game_map[x][y].tile_type == BROWN_GROUND)
                {
                    tile_texture = brown_ground_texture; // LoadTexture("img/brown_ground.png");
                }
                else if (game_map[x][y].tile_type == LIGHT_SKY)
                {
                    tile_texture = light_sky_texture; // LoadTexture("img/light_sky.png");
                }

                Vector2 tile_position = {x * 10, y * 10};
                Rectangle texture_rectangle = {(float)(x * 10), (float)(y * 10), tile_texture.width, tile_texture.height};
                DrawTextureRec(tile_texture, texture_rectangle, tile_position, WHITE);
            }
        }

        // Draw the brownie above brown_ground (y=0)
        //        DrawTextureRec(brownie_running, frameRec, (Vector2){position.x, 0}, WHITE);

        // todo: check if on ground or in air
        if (player_character.speed == 0)
        {
            Rectangle static_rec = {0.f, 0.f, (float)brownie_standing.width, (float)brownie_standing.height};
            DrawTextureRec(brownie_standing, static_rec, (Vector2){player_character.position.x, player_character.position.y}, WHITE);
        }
        else
        {
            if (player_character.direction == RIGHT)
            {
                player_character.position.x += player_character.speed;
                DrawTextureRec(brownie_running, frameRec, (Vector2){player_character.position.x, player_character.position.y}, WHITE);
            }
            else if (player_character.direction == LEFT)
            {
                player_character.position.x -= player_character.speed;
                // Flip texture horizontally by using negative width
                Rectangle flipped_frameRec = {frameRec.x + frameRec.width, frameRec.y, -frameRec.width, frameRec.height};
                DrawTexturePro(brownie_running, flipped_frameRec,
                               (Rectangle){player_character.position.x, player_character.position.y, frameRec.width, frameRec.height},
                               (Vector2){0, 0}, 0.0f, WHITE);
            }
        }

        DrawText("(c) Zachary Brownie by Juho Antti Heinonen", screenWidth - 400, screenHeight - 20, 10, GRAY);

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexturePro(target.texture, (Rectangle){0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height},
                       (Rectangle){(GetScreenWidth() - ((float)gameScreenWidth * scale)) * 0.5f, (GetScreenHeight() - ((float)gameScreenHeight * scale)) * 0.5f,
                                   (float)gameScreenWidth * scale, (float)gameScreenHeight * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
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