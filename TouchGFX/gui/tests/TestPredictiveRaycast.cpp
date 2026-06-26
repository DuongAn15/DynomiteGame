#include "test_utils.hpp"
#define private public
#include <gui/model/Model.hpp>
#undef private
#include <math.h>

void test_raycast() {
    Model m;
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    
    // Đặt 1 quả trứng làm chắn đường
    m.grid[HexGrid::index(2, 4)] = GameConstants::COLOR_RED; // Tọa độ khoảng x=108, y=76

    // Giả lập bắn đạn từ (108, 243) thẳng lên trên
    float simX = 108.0f;
    float simY = 243.0f;
    float simVx = 0.0f;
    float simVy = -5.0f;
    
    bool hit = false;
    for (int step = 0; step < 100; step++) {
        simX += simVx;
        simY += simVy;
        if (m.isCollisionAt(simX, simY)) {
            hit = true;
            break;
        }
    }
    
    ASSERT(hit == true); // Phải chạm trứng
    // Khi chạm, tọa độ Y phải nhô lên một đoạn nhỏ (tương đương EGG_HEIGHT / HITBOX_RADIUS)
    ASSERT(simY > 76.0f); 
}

void run_PredictiveRaycast_Tests() {
    test_raycast();
}
