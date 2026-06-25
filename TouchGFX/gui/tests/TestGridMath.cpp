#include "test_utils.hpp"

// Hack để test các hàm/biến private
#define private public
#include <gui/model/Model.hpp>
#undef private

void test_isValidCell() {
    Model m;
    // MAX_ROWS = 9, MAX_COLS = 9
    // Even rows: 0 to 8 (max 9)
    ASSERT(m.isValidCell(0, 0) == true);
    ASSERT(m.isValidCell(0, 8) == true);
    ASSERT(m.isValidCell(0, 9) == false);
    
    // Odd rows: 0 to 7 (max 8)
    ASSERT(m.isValidCell(1, 0) == true);
    ASSERT(m.isValidCell(1, 7) == true);
    ASSERT(m.isValidCell(1, 8) == false);
    
    // Out of bounds
    ASSERT(m.isValidCell(-1, 0) == false);
    ASSERT(m.isValidCell(9, 0) == false);
    ASSERT(m.isValidCell(0, -1) == false);
}

void test_getApproxCell() {
    Model m;
    int col, row;
    
    // GRID_START_X = 12.0, GRID_START_Y = 20.0
    // CELL_WIDTH = 24.0, CELL_HEIGHT = 28.0
    m.getApproxCell(12.0f, 20.0f, col, row);
    ASSERT(col == 0 && row == 0);
    
    m.getApproxCell(36.0f, 48.0f, col, row);
    ASSERT(col == 1 && row == 1);
    
    // Truncation negative values -> fixed with roundf
    m.getApproxCell(8.0f, 20.0f, col, row); // 8 - 12 = -4 -> roundf(-4/24) = 0
    ASSERT(col == 0);
    
    m.getApproxCell(232.0f, 20.0f, col, row); // 232 - 12 = 220 -> roundf(220/24) = 9
    ASSERT(col == 9);
}

void run_GridMath_Tests() {
    test_isValidCell();
    test_getApproxCell();
}
