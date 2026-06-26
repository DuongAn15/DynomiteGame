#include "test_utils.hpp"

// Hack để test các hàm/biến private
#define private public
#include <gui/model/Model.hpp>
#undef private

void test_isValidCell() {
    Model m;
    // MAX_ROWS = 9, MAX_COLS = 9
    // Even rows: 0 to 8 (max 9)
    ASSERT(HexGrid::isValidCell(0, 0, true) == true);
    ASSERT(HexGrid::isValidCell(0, 8, true) == true);
    ASSERT(HexGrid::isValidCell(0, 9, true) == false);
    
    // Odd rows: 0 to 7 (max 8)
    ASSERT(HexGrid::isValidCell(1, 0, false) == true);
    ASSERT(HexGrid::isValidCell(1, 7, false) == true);
    ASSERT(HexGrid::isValidCell(1, 8, false) == false);
    
    // Out of bounds
    ASSERT(HexGrid::isValidCell(-1, 0, false) == false);
    ASSERT(HexGrid::isValidCell(9, 0, false) == false);
    ASSERT(HexGrid::isValidCell(0, -1, true) == false);
}

void test_getApproxCell() {
    int col, row;
    
    // GRID_START_X = 12.0, GRID_START_Y = -8.0 (updated)
    // CELL_WIDTH = 24.0, CELL_HEIGHT = 28.0
    HexGrid::pixelToNearestCell(12.0f, -8.0f, 0, row, col);
    ASSERT(col == 0 && row == 0);
    
    HexGrid::pixelToNearestCell(36.0f, 20.0f, 0, row, col);
    ASSERT(col == 1 && row == 1);
    
    // Truncation negative values -> fixed with roundf
    HexGrid::pixelToNearestCell(8.0f, -8.0f, 0, row, col);
    ASSERT(col == 0);
    
    HexGrid::pixelToNearestCell(232.0f, -8.0f, 0, row, col);
    ASSERT(col == 9);
}

void run_GridMath_Tests() {
    test_isValidCell();
    test_getApproxCell();
}
