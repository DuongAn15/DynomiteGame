#include <gui/common/CollisionEngine.hpp>
#include <gui/common/GameConstants.hpp>
#include <gui/common/HexGrid.hpp>
#include <gui/common/GameBoardMapper.hpp>

bool CollisionEngine::computeCollisionAt(
    float x, float y,
    const uint8_t* grid,
    int headRowIndex,
    int gridParityOffset,
    float globalOffsetY)
{
    if (y - GameConstants::EGG_RADIUS <= GameConstants::TOP_WALL + globalOffsetY) {
        return true;
    }
    
    int approxRow, approxCol;
    GameBoardMapper::pixelToNearestCell(x, y - globalOffsetY, gridParityOffset, approxRow, approxCol);
    
    int startR = approxRow - GameConstants::COLLISION_SCAN_RADIUS;
    if (startR < 0) startR = 0;
    int endR = approxRow + GameConstants::COLLISION_SCAN_RADIUS;
    if (endR >= GameConstants::MAX_ROWS) endR = GameConstants::MAX_ROWS - 1;
    int startC = approxCol - GameConstants::COLLISION_SCAN_RADIUS;
    if (startC < 0) startC = 0;
    int endC = approxCol + GameConstants::COLLISION_SCAN_RADIUS;
    if (endC >= GameConstants::MAX_COLS) endC = GameConstants::MAX_COLS - 1;

    float hitRadiusSq = GameConstants::HITBOX_RADIUS * GameConstants::HITBOX_RADIUS;

    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (HexGrid::isValidCell(r, c, GameBoardMapper::isLogicalRowEven(r, gridParityOffset)) && grid[GameBoardMapper::computePhysicalIndex(r, c, headRowIndex)] != GameConstants::EMPTY_COLOR) {
                bool isEven = GameBoardMapper::isLogicalRowEven(r, gridParityOffset);
                float px = HexGrid::cellToPixelX(c, isEven);
                float py = HexGrid::cellToPixelY(r) + globalOffsetY;
                
                float dx = px - x;
                float dy = py - y;
                
                float penaltyX = CollisionEngine::computeWallPenalty(x, GameConstants::LEFT_WALL, GameConstants::RIGHT_WALL, GameConstants::HITBOX_PENALTY_MIN, GameConstants::HITBOX_PENALTY_MAX, GameConstants::HITBOX_WALL_DIST);
                float distSq = (dx * penaltyX) * (dx * penaltyX) + (dy * 1.0f) * (dy * 1.0f);
                
                if (distSq < hitRadiusSq) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void CollisionEngine::resolveSnapToGrid(
    float px, float py,
    int& outCol, int& outRow,
    const uint8_t* grid,
    int headRowIndex,
    int gridParityOffset,
    float globalOffsetY)
{
    float minDistSq = GameConstants::MAX_DIST_SQ_INIT;
    int bestC = 0, bestR = 0;
    
    int approxRow, approxCol;
    GameBoardMapper::pixelToNearestCell(px, py - globalOffsetY, gridParityOffset, approxRow, approxCol);
    
    int startR = approxRow - GameConstants::COLLISION_SCAN_RADIUS;
    if (startR < 0) startR = 0;
    int endR = approxRow + GameConstants::COLLISION_SCAN_RADIUS;
    if (endR >= GameConstants::MAX_ROWS) endR = GameConstants::MAX_ROWS - 1;
    int startC = approxCol - GameConstants::COLLISION_SCAN_RADIUS;
    if (startC < 0) startC = 0;
    int endC = approxCol + GameConstants::COLLISION_SCAN_RADIUS;
    if (endC >= GameConstants::MAX_COLS) endC = GameConstants::MAX_COLS - 1;
    
    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (HexGrid::isValidCell(r, c, GameBoardMapper::isLogicalRowEven(r, gridParityOffset)) && grid[GameBoardMapper::computePhysicalIndex(r, c, headRowIndex)] == GameConstants::EMPTY_COLOR) {
                bool canAttach = false;
                if (r == 0) {
                    canAttach = true;
                } else {
                    for (int i = 0; i < GameConstants::HEX_NEIGHBORS_COUNT; i++) {
                        bool isEven = GameBoardMapper::isLogicalRowEven(r, gridParityOffset);
                        const NeighborOffset* neighbors = HexGrid::getNeighbors(isEven);
                        int nr = r + neighbors[i].dy;
                        int nc = c + neighbors[i].dx;
                        if (HexGrid::isValidCell(nr, nc, GameBoardMapper::isLogicalRowEven(nr, gridParityOffset)) && grid[GameBoardMapper::computePhysicalIndex(nr, nc, headRowIndex)] != GameConstants::EMPTY_COLOR) {
                            canAttach = true;
                            break;
                        }
                    }
                }
                
                if (canAttach) {
                    bool isEven = GameBoardMapper::isLogicalRowEven(r, gridParityOffset);
                    float cx = HexGrid::cellToPixelX(c, isEven);
                    float cy = HexGrid::cellToPixelY(r) + globalOffsetY;
                    float distSq = CollisionEngine::computeDistanceSquared(px, py, cx, cy);
                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        bestC = c;
                        bestR = r;
                    }
                }
            }
        }
    }
    if (minDistSq >= GameConstants::MAX_DIST_SQ_INIT) {
        outCol = -1;
        outRow = -1;
    } else {
        outCol = bestC;
        outRow = bestR;
    }
}

float CollisionEngine::computeDistanceSquared(float x1, float y1, float x2, float y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

float CollisionEngine::computeWallPenalty(float x, float leftWall, float rightWall, float penaltyMin, float penaltyMax, float wallDist) {
    float penaltyX = penaltyMax;
    float distToLeft = x - leftWall;
    float distToRight = rightWall - x;
    float minDistToWall = (distToLeft < distToRight) ? distToLeft : distToRight;
    
    if (minDistToWall < wallDist) {
        if (minDistToWall < 0.0f) minDistToWall = 0.0f;
        penaltyX = penaltyMin + (penaltyMax - penaltyMin) * (minDistToWall / wallDist);
    }
    
    return penaltyX;
}
