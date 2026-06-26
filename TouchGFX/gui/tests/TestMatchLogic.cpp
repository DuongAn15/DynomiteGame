#include "test_utils.hpp"
#define private public
#include <gui/model/Model.hpp>
#undef private

#include "MockModelListener.hpp"

void test_match_3() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.player.score = 0;

    // Đặt 3 quả trứng cùng màu cạnh nhau
    m.grid[HexGrid::index(0, 0)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 1)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(1, 0)] = GameConstants::COLOR_RED;

    m.checkMatches(0, 0);

    // Kiểm tra đã bị nổ
    ASSERT(m.grid[HexGrid::index(0, 0)] == GameConstants::EMPTY_COLOR);
    ASSERT(m.grid[HexGrid::index(0, 1)] == GameConstants::EMPTY_COLOR);
    ASSERT(m.grid[HexGrid::index(1, 0)] == GameConstants::EMPTY_COLOR);

    // Điểm phải tăng: SCORE_MATCH_3 = 3
    ASSERT(m.player.score == GameConstants::SCORE_MATCH_3);
    ASSERT(mock.isScoreUpdatedCalled == true);
    ASSERT(mock.lastScoreUpdated == GameConstants::SCORE_MATCH_3);
}

void test_match_4() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.player.score = 0;

    m.grid[HexGrid::index(0, 0)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 1)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 2)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(1, 0)] = GameConstants::COLOR_RED;

    m.checkMatches(0, 0);

    ASSERT(m.player.score == GameConstants::SCORE_MATCH_4);
    ASSERT(mock.isScoreUpdatedCalled == true);
    ASSERT(mock.lastScoreUpdated == GameConstants::SCORE_MATCH_4);
}

void test_match_5() {
    Model m;
    MockModelListener mock;
    m.bind(&mock);
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.player.score = 0;

    m.grid[HexGrid::index(0, 0)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 1)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 2)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(1, 0)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(1, 1)] = GameConstants::COLOR_RED;

    m.checkMatches(0, 0);

    ASSERT(m.player.score == GameConstants::SCORE_MATCH_5);
    ASSERT(mock.isScoreUpdatedCalled == true);
    ASSERT(mock.lastScoreUpdated == GameConstants::SCORE_MATCH_5);
}

void test_match_2() {
    Model m;
    for(int i=0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) m.grid[i] = GameConstants::EMPTY_COLOR;
    m.player.score = 0;

    // Đặt 2 quả trứng cùng màu cạnh nhau
    m.grid[HexGrid::index(0, 0)] = GameConstants::COLOR_RED;
    m.grid[HexGrid::index(0, 1)] = GameConstants::COLOR_RED;

    m.checkMatches(0, 0);

    // Không đủ 3 quả nên không nổ
    ASSERT(m.grid[HexGrid::index(0, 0)] == GameConstants::COLOR_RED);
    ASSERT(m.grid[HexGrid::index(0, 1)] == GameConstants::COLOR_RED);

    // Điểm không tăng
    ASSERT(m.player.score == 0);
}

void run_MatchLogic_Tests() {
    test_match_3();
    test_match_4();
    test_match_5();
    test_match_2();
}
