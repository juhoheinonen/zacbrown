#include "raylib.h"
#include "raymath.h" // Required for: Vector2Clamp()
#include <stdio.h>
#include "custom_types.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED 1
#define GAME_MAP_WIDTH 80
#define GAME_MAP_HEIGHT 60
#define TILE_SIZE 8

// Function to initialize the game map
void initialize_game_map(game_tile game_map[GAME_MAP_WIDTH][GAME_MAP_HEIGHT])
{
    for (int x = 0; x < GAME_MAP_WIDTH; x++)
    {
        for (int y = 0; y < GAME_MAP_HEIGHT; y++)
        {
            if (y >= GAME_MAP_HEIGHT - 4)
            {
                game_map[x][y] = (game_tile){.x = x, .y = y, .tile_type = BROWN_GROUND};
            }
            else
            {
                game_map[x][y] = (game_tile){.x = x, .y = y, .tile_type = LIGHT_SKY};
            }
        }
    }
}

// Function to draw the map tiles
void draw_map_tiles(game_tile game_map[GAME_MAP_WIDTH][GAME_MAP_HEIGHT], Texture2D brown_ground_texture, Texture2D light_sky_texture)
{
    for (int x = 0; x < GAME_MAP_WIDTH; x++)
    {
        for (int y = 0; y < GAME_MAP_HEIGHT; y++)
        {
            Texture2D tile_texture;
            if (game_map[x][y].tile_type == BROWN_GROUND)
            {
                tile_texture = brown_ground_texture;
            }
            else if (game_map[x][y].tile_type == LIGHT_SKY)
            {
                tile_texture = light_sky_texture;
            }

            Vector2 tile_position = {x * TILE_SIZE, y * TILE_SIZE};
            Rectangle texture_rectangle = {(float)(x * TILE_SIZE), (float)(y * TILE_SIZE), tile_texture.width, tile_texture.height};
            DrawTextureRec(tile_texture, texture_rectangle, tile_position, WHITE);
        }
    }
}

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
    Texture2D brown_ground_texture = LoadTexture("img/brown_ground.png");
    Texture2D light_sky_texture = LoadTexture("img/light_sky.png");

    // Vector2 position_standing = {350.0f, 150.0f};
    //  Rectangle static_rec = {0.0f, 0.0f, (float)brownie_standing.width, (float)brownie_standing.height};

    Vector2 position = {350.0f, 280.0f};
    Rectangle frameRec = {0.0f, 0.0f, (float)brownie_running.width / 6, (float)brownie_running.height};
    int currentFrame = 0;

    // initialize map here
    game_tile game_map[GAME_MAP_WIDTH][GAME_MAP_HEIGHT];
    initialize_game_map(game_map);

    main_character player_character = {.position = {1.0f, 200.0f}, .horizontal_speed = 0, .position = 0};

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

        // debug code starts
        if (IsKeyPressed(KEY_UP)) {
            player_character.position.y--;
        }
        // debug code ends

        if (IsKeyDown(KEY_RIGHT))
        {
            player_character.horizontal_speed = 5;            
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            player_character.horizontal_speed = -5;            
        }
        else
        {
            player_character.horizontal_speed = 0;
        }

        // check if player feet hit blocking tile
        if (player_character.position.y)
        {
            // loop from player's x the player size. TODO: enable hitbox.
            for (int i = player_character.position.x; i < player_character.position.x + 64; i++)
            {                
                // distance to next tile
                int distance_to_next_tile = (int)player_character.position.y % TILE_SIZE;

                // check tile type in next tile
                int next_tile_y = ((int)player_character.position.y + distance_to_next_tile + 1) / TILE_SIZE;

                int x_calculated = player_character.position.x * TILE_SIZE / GAME_MAP_WIDTH;
                float height_calculated = 1.0 * TILE_SIZE / GAME_MAP_HEIGHT;

                int y_calculated = player_character.position.y * height_calculated; // TODO: fix this, this is just guessing

                if (game_map[x_calculated][y_calculated].tile_type == BROWN_GROUND)
                {
                    player_character.vertical_speed = 0;
                }
                else
                {
                    player_character.vertical_speed = 1;
                }
            }
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

        // Draw
        //----------------------------------------------------------------------------------
        // Draw everything in the render texture, note this will not be rendered on screen, yet
        BeginTextureMode(target);

        ClearBackground(RAYWHITE);

        // Draw the map tiles
        draw_map_tiles(game_map, brown_ground_texture, light_sky_texture);

        // is player falling
        if (player_character.vertical_speed > 0)
        {
            player_character.position.y += player_character.vertical_speed;
        } // TODO: jumping, flying upwards?
        
        if (player_character.horizontal_speed == 0)
        {
            Rectangle static_rec = {0.f, 0.f, (float)brownie_standing.width, (float)brownie_standing.height};
            DrawTextureRec(brownie_standing, static_rec, (Vector2){player_character.position.x, player_character.position.y}, WHITE);
        }
        else if (player_character.horizontal_speed > 0)
        {
            player_character.position.x += player_character.horizontal_speed;
            DrawTextureRec(brownie_running, frameRec, (Vector2){player_character.position.x, player_character.position.y}, WHITE);
        }
        else
        {
            player_character.position.x += player_character.horizontal_speed;
            // Flip texture horizontally by using negative width
            Rectangle flipped_frameRec = {frameRec.x + frameRec.width, frameRec.y, -frameRec.width, frameRec.height};
            DrawTexturePro(brownie_running, flipped_frameRec,
                           (Rectangle){player_character.position.x, player_character.position.y, frameRec.width, frameRec.height},
                           (Vector2){0, 0}, 0.0f, WHITE);
        }        

        char pc_position_information[50];
        snprintf(pc_position_information, sizeof(pc_position_information), 
                 "x: %.1f, y: %.1f", player_character.position.x, player_character.position.y);
        DrawText(pc_position_information, 10, 10, 20, BLACK);
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
    UnloadTexture(brownie_standing); // Texture unloading
    UnloadTexture(brown_ground_texture); // Texture unloading
    UnloadTexture(light_sky_texture); // Texture unloading

    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}