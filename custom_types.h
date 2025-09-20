#include "raylib.h"

enum tile_type {
    BROWN_GROUND,
    LIGHT_SKY
};

enum direction {    
    RIGHT,    
    LEFT
};

typedef struct {
    int x;
    int y;
    enum tile_type tile_type;
} game_tile;

typedef struct {    
    int left_x;
    int right_x;
    int top_y;
    int bottom_y;
} hitbox;

typedef struct {
    Vector2 position;
    hitbox hitbox;
    int height_pixels;    // image height
    int horizontal_speed; // negative means to left, zero standing, positive to right
    int vertical_speed; // negative means up, zero stay same, positive up    
    int jumping_power;
} main_character;
