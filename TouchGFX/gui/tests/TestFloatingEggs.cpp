#include "test_utils.hpp"
#define private public
#include <gui/model/Model.hpp>
#undef private

void test_floating_eggs() {
    Model m;
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.player.score = 0;

    // Hàng 0 có trứng (nối trần)
    m.grid[HexGrid::index(0, 2)] = GameConstants::COLOR_RED;
    
    // Hàng 1 có trứng (nối hàng 0)
    m.grid[HexGrid::index(1, 2)] = GameConstants::COLOR_BLUE;

    // Trứng mồ côi (floating) ở hàng 4, 5
    m.grid[HexGrid::index(4, 4)] = GameConstants::COLOR_GREEN;
    m.grid[HexGrid::index(5, 4)] = GameConstants::COLOR_GREEN;

    m.dropFloatingEggs();

    // Trứng nối trần vẫn còn
    ASSERT(m.grid[HexGrid::index(0, 2)] == GameConstants::COLOR_RED);
    ASSERT(m.grid[HexGrid::index(1, 2)] == GameConstants::COLOR_BLUE);

    // Trứng mồ côi rụng sạch
    ASSERT(m.grid[HexGrid::index(4, 4)] == GameConstants::EMPTY_COLOR);
    ASSERT(m.grid[HexGrid::index(5, 4)] == GameConstants::EMPTY_COLOR);

    // Điểm rụng 2 quả: SCORE_DROP_ORPHAN * 2 = 2
    ASSERT(m.player.score == GameConstants::SCORE_DROP_ORPHAN * 2);
}

void run_FloatingEggs_Tests() {
    test_floating_eggs();
}
