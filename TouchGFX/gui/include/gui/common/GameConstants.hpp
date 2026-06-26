#ifndef GAME_CONSTANTS_HPP
#define GAME_CONSTANTS_HPP

#include <stdint.h>

namespace GameConstants {
    // Grid & Matrix
    constexpr int MAX_ROWS = 12;
    constexpr int MAX_COLS = 11;
    
    // UI Layout & Grid
    constexpr int EGG_GRID_WIDTH = 228;
    constexpr float CELL_WIDTH = 24.0f;
    constexpr float CELL_HEIGHT = 28.0f;
    constexpr float GRID_START_X = 12.0f;
    constexpr float GRID_START_Y = -8.0f; // Shifted up by 28.0f (CELL_HEIGHT) from 20.0f
    constexpr int UI_CELL_WIDTH = 24;
    constexpr int UI_CELL_HEIGHT = 21;
    constexpr int UI_CELL_OFFSET_X = 12;

    // Colors
    constexpr int EMPTY_COLOR = 0;
    constexpr int NUM_COLORS = 7;

    // Egg / Bullet Props
    constexpr float EGG_RADIUS = 12.0f;
    constexpr float EGG_WIDTH = 24.0f;
    constexpr float EGG_HEIGHT = 32.0f;
    constexpr float BULLET_START_X = 120.0f;
    constexpr float BULLET_START_Y = 243.0f;
    constexpr float BULLET_SPEED = 5.0f;

    // Bounds & Walls
    constexpr float LEFT_WALL = 8.0f;
    constexpr float RIGHT_WALL = 208.0f;
    constexpr float TOP_WALL = 16.0f;
    
    // Animations & Timing
    constexpr int DINO_THROW_FRAMES = 15;
    constexpr int DINO_DONE_FRAMES = 10;
    
    // Physics & Aiming
    constexpr float AIM_SMOOTH_FACTOR = 0.3f;
    constexpr float AIM_MIN_DY = -10.0f;
    
    constexpr int TRAJECTORY_DOTS_COUNT = 15;
    constexpr int16_t TRAJECTORY_DOT_SIZE = 4;
    constexpr int16_t TRAJECTORY_DOT_OFFSET = 2;
    constexpr float TRAJECTORY_DOT_START_DIST = 30.0f;
    constexpr float TRAJECTORY_DOT_SPACING = 20.0f;
    constexpr uint8_t TRAJECTORY_DOT_COLOR_R = 200;
    constexpr uint8_t TRAJECTORY_DOT_COLOR_G = 200;
    constexpr uint8_t TRAJECTORY_DOT_COLOR_B = 200;

    // Hitbox logic
    constexpr float HITBOX_RADIUS = 18.04f;
    constexpr float HITBOX_PENALTY_MAX = 1.4f;
    constexpr float HITBOX_PENALTY_MIN = 1.0f;
    constexpr float HITBOX_WALL_DIST = 30.0f;
    
    // Render Invalidation padding
    constexpr int16_t INVALIDATE_PADDING = 10;
    constexpr int16_t INVALIDATE_WIDTH = 44;
    constexpr int16_t INVALIDATE_HEIGHT = 52;
    constexpr uint8_t SHADOW_GRID_INIT_VAL = 0xFF;
    constexpr float CAMERA_DISTANCE_Z = 1000.0f;
    constexpr int DINO_COLOR_OFFSET = 100;

    // Scoring
    constexpr int SCORE_MATCH_3 = 3;
    constexpr int SCORE_MATCH_4 = 5;
    constexpr int SCORE_MATCH_5 = 8;
    constexpr int SCORE_DROP_ORPHAN = 1;

    // Game Logic
    constexpr uint32_t RNG_INITIAL_SEED = 12345;
    constexpr int SPAWN_INTERVAL_START = 2800;
    constexpr int SPAWN_INTERVAL_LUT[32] = {
        2800,2800,2800,2800,2800,2800,2800,2800,2800,2800,
        2600,2600,2600,2600,2600,2600,2600,2600,
        2400,2400,2400,2400,2400,2400,
        2200,2200,2200,2200,
        2000,2000,2000,2000
    };
    constexpr uint32_t RNG_MULTIPLIER = 1664525;
    constexpr uint32_t RNG_INCREMENT = 1013904223;
    constexpr int INITIAL_ROWS = 3;
    constexpr int GAME_OVER_ROW = 9;
    constexpr int MIN_BALLS_THRESHOLD = 12;
    constexpr float DROP_SPEED_START = 0.015f; // Pixels per frame
    constexpr float DROP_SPEED_INCREMENT = 0.01f;
    constexpr float DROP_SPEED_MAX = 0.15f;
    
    constexpr int HEX_NEIGHBORS_COUNT = 6;
    constexpr int COLLISION_SCAN_RADIUS = 2;
    constexpr float MAX_DIST_SQ_INIT = 999999.0f;
    constexpr int MIN_MATCH_COUNT = 3;

    // Color IDs
    constexpr int COLOR_BLUE = 1;
    constexpr int COLOR_GREEN = 2;
    constexpr int COLOR_ORANGE = 3;
    constexpr int COLOR_PINK = 4;
    constexpr int COLOR_PURPLE = 5;
    constexpr int COLOR_RED = 6;
    constexpr int COLOR_YELLOW = 7;


    constexpr float MODEL_CELL_Y[MAX_ROWS] = {
        GRID_START_Y + 0 * CELL_HEIGHT, GRID_START_Y + 1 * CELL_HEIGHT,
        GRID_START_Y + 2 * CELL_HEIGHT, GRID_START_Y + 3 * CELL_HEIGHT,
        GRID_START_Y + 4 * CELL_HEIGHT, GRID_START_Y + 5 * CELL_HEIGHT,
        GRID_START_Y + 6 * CELL_HEIGHT, GRID_START_Y + 7 * CELL_HEIGHT,
        GRID_START_Y + 8 * CELL_HEIGHT, GRID_START_Y + 9 * CELL_HEIGHT,
        GRID_START_Y + 10 * CELL_HEIGHT, GRID_START_Y + 11 * CELL_HEIGHT
    };

    constexpr float MODEL_CELL_X_EVEN[MAX_COLS] = {
        GRID_START_X + 0 * CELL_WIDTH, GRID_START_X + 1 * CELL_WIDTH,
        GRID_START_X + 2 * CELL_WIDTH, GRID_START_X + 3 * CELL_WIDTH,
        GRID_START_X + 4 * CELL_WIDTH, GRID_START_X + 5 * CELL_WIDTH,
        GRID_START_X + 6 * CELL_WIDTH, GRID_START_X + 7 * CELL_WIDTH,
        GRID_START_X + 8 * CELL_WIDTH, GRID_START_X + 9 * CELL_WIDTH,
        GRID_START_X + 10 * CELL_WIDTH
    };

    constexpr float MODEL_CELL_X_ODD[MAX_COLS] = {
        GRID_START_X + 0 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 1 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 2 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 3 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 4 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 5 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 6 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 7 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 8 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 9 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 10 * CELL_WIDTH + (CELL_WIDTH / 2.0f)
    };

    constexpr int UI_CELL_Y[MAX_ROWS] = {
        0 * UI_CELL_HEIGHT, 1 * UI_CELL_HEIGHT,
        2 * UI_CELL_HEIGHT, 3 * UI_CELL_HEIGHT,
        4 * UI_CELL_HEIGHT, 5 * UI_CELL_HEIGHT,
        6 * UI_CELL_HEIGHT, 7 * UI_CELL_HEIGHT,
        8 * UI_CELL_HEIGHT, 9 * UI_CELL_HEIGHT,
        10 * UI_CELL_HEIGHT, 11 * UI_CELL_HEIGHT
    };

    constexpr int UI_CELL_X_EVEN[MAX_COLS] = {
        0 * UI_CELL_WIDTH, 1 * UI_CELL_WIDTH,
        2 * UI_CELL_WIDTH, 3 * UI_CELL_WIDTH,
        4 * UI_CELL_WIDTH, 5 * UI_CELL_WIDTH,
        6 * UI_CELL_WIDTH, 7 * UI_CELL_WIDTH,
        8 * UI_CELL_WIDTH, 9 * UI_CELL_WIDTH,
        10 * UI_CELL_WIDTH
    };

    constexpr int UI_CELL_X_ODD[MAX_COLS] = {
        0 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 1 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        2 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 3 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        4 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 5 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        6 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 7 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        8 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 9 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        10 * UI_CELL_WIDTH + UI_CELL_OFFSET_X
    };
}

#endif // GAME_CONSTANTS_HPP
