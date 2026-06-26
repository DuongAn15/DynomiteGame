#ifndef HEXGRID_HPP
#define HEXGRID_HPP

#include <gui/common/GameConstants.hpp>

struct NeighborOffset {
    int dx; // col offset
    int dy; // row offset
};

class HexGrid {
public:
    static int index(int row, int col);
    static void indexToCell(int index, int& row, int& col);
    static bool isValidCell(int row, int col, bool isEven);
    static float cellToPixelX(int col, bool isEven);
    static float cellToPixelY(int row);
    static void pixelToRowCol(float x, float y, int& row, int& col);
    static const NeighborOffset* getNeighbors(bool isEven);
};

#endif
