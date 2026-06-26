#include <gui/common/MatchEngine.hpp>
#include <gui/common/HexGrid.hpp>
#include <gui/common/GameConstants.hpp>
#include <cstring>

int MatchEngine::getPhysicalIndex(int logicalRow, int col, int headRowIndex) {
    int r = headRowIndex + logicalRow;
    if (r >= GameConstants::MAX_ROWS) r -= GameConstants::MAX_ROWS;
    return HexGrid::index(r, col);
}

int MatchEngine::findMatches(
    const uint8_t* grid,
    int headRowIndex,
    int gridParityOffset,
    int startRow,
    int startCol,
    int* matchGroup,
    bool* visited,
    int* algoQueueStack)
{
    uint8_t targetColor = grid[getPhysicalIndex(startRow, startCol, headRowIndex)];
    if (targetColor == GameConstants::EMPTY_COLOR) return 0;
    
    int totalCells = GameConstants::MAX_ROWS * GameConstants::MAX_COLS;
    memset(visited, 0, totalCells * sizeof(bool));
    
    int qHead = 0, qTail = 0;
    int matchCount = 0;
    
    algoQueueStack[qTail++] = (startRow << 8) | startCol;
    visited[HexGrid::index(startRow, startCol)] = true;
    
    while (qHead < qTail) {
        int curr = algoQueueStack[qHead++];
        matchGroup[matchCount++] = curr;
        
        int r = curr >> 8;
        int c = curr & 0xFF;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            bool isEven = HexGrid::isEvenRow(r, gridParityOffset);
            const NeighborOffset* neighbors = HexGrid::getNeighbors(isEven);
            int nr = r + neighbors[i].dy;
            int nc = c + neighbors[i].dx;
            
            if (HexGrid::isValidCell(nr, nc, HexGrid::isEvenRow(nr, gridParityOffset))) {
                int nIdx = HexGrid::index(nr, nc);
                if (!visited[nIdx] && grid[getPhysicalIndex(nr, nc, headRowIndex)] == targetColor) {
                    visited[nIdx] = true;
                    algoQueueStack[qTail++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    return matchCount;
}

int MatchEngine::dropFloatingEggs(
    uint8_t* grid,
    int headRowIndex,
    int gridParityOffset,
    bool* connected,
    int* algoQueueStack)
{
    int totalCells = GameConstants::MAX_ROWS * GameConstants::MAX_COLS;
    memset(connected, 0, totalCells * sizeof(bool));
    int top = 0;
    
    for (int c = 0; c < GameConstants::MAX_COLS; c++) {
        if (grid[getPhysicalIndex(0, c, headRowIndex)] != GameConstants::EMPTY_COLOR) {
            algoQueueStack[top++] = (0 << 8) | c;
            connected[HexGrid::index(0, c)] = true;
        }
    }
    
    while (top > 0) {
        int curr = algoQueueStack[--top];
        int r = curr >> 8;
        int c = curr & 0xFF;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            bool isEven = HexGrid::isEvenRow(r, gridParityOffset);
            const NeighborOffset* neighbors = HexGrid::getNeighbors(isEven);
            int nr = r + neighbors[i].dy;
            int nc = c + neighbors[i].dx;
            
            if (HexGrid::isValidCell(nr, nc, HexGrid::isEvenRow(nr, gridParityOffset))) {
                int nIdx = HexGrid::index(nr, nc);
                if (!connected[nIdx] && grid[getPhysicalIndex(nr, nc, headRowIndex)] != GameConstants::EMPTY_COLOR) {
                    connected[nIdx] = true;
                    algoQueueStack[top++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    int dropCount = 0;
    int r = 0, c = 0;
    for (int i = 0; i < totalCells; i++) {
        if (grid[getPhysicalIndex(r, c, headRowIndex)] != GameConstants::EMPTY_COLOR && !connected[i]) {
            grid[getPhysicalIndex(r, c, headRowIndex)] = GameConstants::EMPTY_COLOR;
            dropCount++;
        }
        c++;
        if (c >= GameConstants::MAX_COLS) { c = 0; r++; }
    }
    
    return dropCount;
}
