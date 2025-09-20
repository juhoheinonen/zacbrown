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
    Vector2 position;
    int horizontal_speed; // negative means to left, zero standing, positive to right
    int vertical_speed; // negative means up, zero stay same, positive up    
} main_character;