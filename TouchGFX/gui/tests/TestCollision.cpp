#include "test_utils.hpp"

#define private public
#include <gui/model/Model.hpp>
#undef private
#include <math.h>

void test_wallReflection() {
    Model m;
    
    // Left Wall
    m.bulletX = GameConstants::LEFT_WALL; 
    m.bulletY = 100.0f;
    m.vx = -5.0f; // Moving left, hitting wall
    
    m.updateFlyingPhysics();
    // After logic: bulletX += vx -> 8 - 5 = 3
    // 3 <= 8, vx < 0 -> bulletX = 8 + (8 - 3) = 13. vx = 5
    ASSERT(fabs(m.bulletX - 13.0f) < 0.001f);
    ASSERT(fabs(m.vx - 5.0f) < 0.001f);
    
    // Right Wall (208.0f)
    m.bulletX = GameConstants::RIGHT_WALL;
    m.vx = 5.0f;
    m.updateFlyingPhysics();
    // 208 + 5 = 213. 213 >= 208, vx > 0 -> 208 - (213 - 208) = 203. vx = -5
    ASSERT(fabs(m.bulletX - 203.0f) < 0.001f);
    ASSERT(fabs(m.vx - (-5.0f)) < 0.001f);
}

void test_hitboxEdge() {
    Model m;
    // Dọn lưới
    for(int i = 0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) {
        m.grid[i] = GameConstants::EMPTY_COLOR;
    }
    
    // Test chống xuyên vật thể
    // Left Wall (8.0), Trứng ở row 0, col 0 (12.0)
    m.grid[0] = GameConstants::COLOR_RED;
    
    // Bắn sát lề trái
    bool col1 = m.isCollisionAt(8.0f, GameConstants::GRID_START_Y + GameConstants::HITBOX_RADIUS - 1.0f);
    ASSERT(col1 == true); // Phải chạm trứng ngoài cùng
    
    // Right Wall
    m.grid[0] = GameConstants::EMPTY_COLOR;
    // Row 0 (chẵn), trứng cuối ở col 8 (px = 204.0)
    m.grid[8] = GameConstants::COLOR_BLUE; 
    
    // Bắn sát lề phải
    bool col2 = m.isCollisionAt(208.0f, GameConstants::GRID_START_Y + GameConstants::HITBOX_RADIUS - 1.0f);
    ASSERT(col2 == true); // Phải chạm trứng ngoài cùng
}

void run_Collision_Tests() {
    test_wallReflection();
    test_hitboxEdge();
}
