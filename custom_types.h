enum tile_type {
    BROWN_GROUND,
    LIGHT_SKY
};

enum direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef struct {
    int x;
    int y;
    enum tile_type tile_type;
} game_tile;

typedef struct {
    int x;
    int y;
    int speed;
    enum direction direction;
} main_character;