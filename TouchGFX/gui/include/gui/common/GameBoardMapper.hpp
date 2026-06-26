#ifndef GAMEBOARDMAPPER_HPP
#define GAMEBOARDMAPPER_HPP

#include <gui/common/GameConstants.hpp>
#include <gui/common/HexGrid.hpp>

class GameBoardMapper {
public:
    static bool isLogicalRowEven(int logicalRow, int gridParityOffset);
    static int computePhysicalIndex(int logicalRow, int col, int headRowIndex);
    static bool pixelToNearestCell(float px, float py_minus_globalOffsetY, int gridParityOffset, int& outRow, int& outCol);
};

#endif // GAMEBOARDMAPPER_HPP
