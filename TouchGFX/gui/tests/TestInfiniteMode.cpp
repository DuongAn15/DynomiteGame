#include "test_utils.hpp"
#define private public
#include <gui/model/Model.hpp>
#undef private

#include "MockModelListener.hpp"

// ... các test cũ giữ nguyên hoặc update
void test_shift_grid() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);

    for(int i=0; i<GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    
    m.gridParityOffset = 0;
    m.grid[2 * GameConstants::MAX_COLS + 3] = GameConstants::COLOR_RED;
    
    m.shiftGridDown();
    
    ASSERT(m.gridParityOffset == 1);
    ASSERT(m.grid[3 * GameConstants::MAX_COLS + 3] == GameConstants::COLOR_RED);
    ASSERT(m.grid[2 * GameConstants::MAX_COLS + 3] == GameConstants::EMPTY_COLOR);
    
    // Kiểm tra Integration báo UI
    ASSERT(mock.isGridShiftedCalled == true);
}

void test_survival_game_over() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);

    for(int i=0; i<GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    
    m.gameState = STATE_IDLE;
    m.grid[GameConstants::GAME_OVER_ROW * GameConstants::MAX_COLS + 1] = GameConstants::COLOR_RED;
    
    m.shiftGridDown();
    
    ASSERT(m.gameState == STATE_GAME_OVER);
    // Integration báo UI
    ASSERT(mock.isGameOverCalled == true);
}

void test_offset_y_increment() {
    Model m;
    m.gameState = STATE_IDLE;
    m.totalTicks = 0;
    m.globalOffsetY = 0.0f;
    
    m.tick();
    
    // Tốc độ ban đầu là DROP_SPEED_START
    ASSERT(m.globalOffsetY == GameConstants::DROP_SPEED_START);
    
    m.totalTicks = 600 * 10; // Đổi tốc độ
    m.tick();
    float expectedSpeed = GameConstants::DROP_SPEED_START + (m.totalTicks / 600) * GameConstants::DROP_SPEED_INCREMENT;
    if (expectedSpeed > GameConstants::DROP_SPEED_MAX) expectedSpeed = GameConstants::DROP_SPEED_MAX;
    
    // Vị trí mới sẽ là điểm cũ cộng thêm speed mới
    ASSERT(m.globalOffsetY == GameConstants::DROP_SPEED_START + expectedSpeed);
}

void test_offset_y_trigger_shift() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);
    m.gameState = STATE_IDLE;
    
    m.globalOffsetY = GameConstants::CELL_HEIGHT - 0.01f;
    m.gridParityOffset = 0;
    
    // Tick lần này sẽ cộng thêm dropSpeed, làm offset vượt ngưỡng CELL_HEIGHT
    m.tick();
    
    // Phải shiftGridDown và đẩy gridParityOffset lên 1
    ASSERT(m.gridParityOffset == 1);
    
    // globalOffsetY phải bị reset (trừ đi CELL_HEIGHT)
    ASSERT(m.globalOffsetY < GameConstants::CELL_HEIGHT);
    ASSERT(mock.isGridShiftedCalled == true);
}

void test_hitbox_with_offset() {
    Model m;
    m.gameState = STATE_IDLE;
    m.globalOffsetY = 14.0f; // Trôi 1 nửa ô
    
    for(int i=0; i<GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.grid[0 * GameConstants::MAX_COLS + 0] = GameConstants::COLOR_RED; // Ô góc trên cùng bên trái
    
    // Tọa độ tâm ô [0,0] lúc bình thường: X = GRID_START_X, Y = GRID_START_Y
    // Do globalOffsetY = 14.0f, nên Y của nó là GRID_START_Y + 14.0f
    float cx, cy;
    m.getCellCenter(0, 0, cx, cy);
    ASSERT(cy == GameConstants::GRID_START_Y + 14.0f);
    
    // Tính approxCell từ tọa độ Y = GRID_START_Y + 14.0f -> Phải ra đúng row = 0
    int col, row;
    m.getApproxCell(cx, cy, col, row);
    ASSERT(row == 0);
    ASSERT(col == 0);
}

void test_integration_rng_stress() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);

    for (int i = 0; i < 10000; i++) {
        m.shiftGridDown();
        
        // Kiểm tra hàng 0 không được có bóng rỗng (trừ những ô thuộc parity thừa)
        int maxC = ((0 + m.gridParityOffset) % 2 != 0) ? (GameConstants::MAX_COLS - 1) : GameConstants::MAX_COLS;
        for (int c = 0; c < maxC; c++) {
            ASSERT(m.grid[0 * GameConstants::MAX_COLS + c] != GameConstants::EMPTY_COLOR);
            ASSERT(m.grid[0 * GameConstants::MAX_COLS + c] >= 1 && m.grid[0 * GameConstants::MAX_COLS + c] <= GameConstants::NUM_COLORS);
        }
    }
}

void run_InfiniteMode_Tests() {
    test_shift_grid();
    test_survival_game_over();
    test_offset_y_increment();
    test_offset_y_trigger_shift();
    test_hitbox_with_offset();
    test_integration_rng_stress();
}
