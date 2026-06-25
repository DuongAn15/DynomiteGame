#include "test_utils.hpp"
#define private public
#include <gui/model/Model.hpp>
#undef private

void test_game_over() {
    Model m;
    for(int i=0; i<GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    
    m.gameState = STATE_FLYING;
    m.score = 0;
    m.currentColor = GameConstants::COLOR_BLUE;

    // Đặt 2 quả trứng làm chướng ngại vật ở hàng 7 (lẻ, so le)
    m.grid[7 * GameConstants::MAX_COLS + 2] = GameConstants::COLOR_RED;
    m.grid[7 * GameConstants::MAX_COLS + 3] = GameConstants::COLOR_RED;
    
    // Bắn từ dưới lên thẳng vào hàng 8 cột 3 (chẵn, tọa độ px = 84, py = 244)
    m.bulletX = 84.0f;
    m.bulletY = 244.0f; 
    m.vx = 0.0f;
    m.vy = -1.0f; // Bay lên để kích hoạt va chạm
    
    // Quét 100 frame để chắc chắn va chạm xảy ra
    for(int i=0; i<100; i++) {
        m.updateFlyingPhysics();
        if (m.gameState != STATE_FLYING) break;
    }

    ASSERT(m.grid[8 * GameConstants::MAX_COLS + 3] == GameConstants::COLOR_BLUE);
    ASSERT(m.gameState == STATE_GAME_OVER);
}

void test_no_game_over_if_match() {
    Model m;
    for(int i=0; i<GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    
    m.gameState = STATE_FLYING;
    m.score = 0;
    m.currentColor = GameConstants::COLOR_RED;

    m.grid[7 * GameConstants::MAX_COLS + 2] = GameConstants::COLOR_RED;
    m.grid[7 * GameConstants::MAX_COLS + 3] = GameConstants::COLOR_RED;
    
    m.bulletX = 84.0f; 
    m.bulletY = 244.0f; 
    m.vx = 0.0f;
    m.vy = -1.0f;
    
    for(int i=0; i<100; i++) {
        m.updateFlyingPhysics();
        if (m.gameState != STATE_FLYING) break;
    }

    ASSERT(m.gameState != STATE_GAME_OVER);
    ASSERT(m.score > 0);
}

void run_GameOver_Tests() {
    test_game_over();
    test_no_game_over_if_match();
}
