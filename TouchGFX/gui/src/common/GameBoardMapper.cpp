#include <gui/common/GameBoardMapper.hpp>

bool GameBoardMapper::isLogicalRowEven(int logicalRow, int gridParityOffset) {
    return ((logicalRow + gridParityOffset) & 1) == 0;
}

int GameBoardMapper::computePhysicalIndex(int logicalRow, int col, int headRowIndex) {
    int r = headRowIndex + logicalRow;
    if (r >= GameConstants::MAX_ROWS) r -= GameConstants::MAX_ROWS;
    return HexGrid::index(r, col);
}

bool GameBoardMapper::pixelToNearestCell(float px, float py_minus_globalOffsetY, int gridParityOffset, int& outRow, int& outCol) {
    HexGrid::pixelToRowCol(px, py_minus_globalOffsetY, outRow, outCol);
    bool isEven = isLogicalRowEven(outRow, gridParityOffset);
    return HexGrid::isValidCell(outRow, outCol, isEven);
}
