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
    int speed;
    int vertical_speed;
    enum direction direction;
} main_character;