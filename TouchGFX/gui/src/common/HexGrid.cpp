#include <gui/common/HexGrid.hpp>
#include <math.h>

static const NeighborOffset neighbors[2][6] = {
    {{-1,-1}, { 0,-1}, {-1, 0}, { 1, 0}, {-1, 1}, { 0, 1}}, // EVEN row
    {{ 0,-1}, { 1,-1}, {-1, 0}, { 1, 0}, { 0, 1}, { 1, 1}}  // ODD row
};

bool HexGrid::isEvenRow(int row, int parityOffset) {
    return ((row + parityOffset) & 1) == 0;
}

int HexGrid::index(int row, int col) {
    return row * GameConstants::MAX_COLS + col;
}

void HexGrid::indexToCell(int index, int& row, int& col) {
    row = index / GameConstants::MAX_COLS;
    col = index % GameConstants::MAX_COLS;
}

bool HexGrid::isValidCell(int row, int col, bool isEven) {
    if (row < 0 || row >= GameConstants::MAX_ROWS || col < 0) return false;
    int maxC = isEven ? GameConstants::MAX_COLS : (GameConstants::MAX_COLS - 1);
    return col < maxC;
}

float HexGrid::cellToPixelX(int col, bool isEven) {
    return isEven ? GameConstants::MODEL_CELL_X_EVEN[col] : GameConstants::MODEL_CELL_X_ODD[col];
}

float HexGrid::cellToPixelY(int row) {
    return GameConstants::MODEL_CELL_Y[row];
}

bool HexGrid::pixelToNearestCell(float x, float y_minus_globalOffset, int parityOffset, int& row, int& col) {
    row = (int)roundf((y_minus_globalOffset - GameConstants::GRID_START_Y) / GameConstants::CELL_HEIGHT);
    col = (int)roundf((x - GameConstants::GRID_START_X) / GameConstants::CELL_WIDTH);
    return isValidCell(row, col, isEvenRow(row, parityOffset));
}

const NeighborOffset* HexGrid::getNeighbors(bool isEven) {
    return isEven ? neighbors[0] : neighbors[1];
}
