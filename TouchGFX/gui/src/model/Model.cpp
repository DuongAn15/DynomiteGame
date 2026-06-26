#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <math.h>
#include <string.h>

using namespace GameConstants;

Model::Model() : modelListener(0), rngState(RNG_INITIAL_SEED)
{
    player.highScore = 0;
    startNewGame();
}

uint32_t Model::nextRandom()
{
    rngState = rngState * RNG_MULTIPLIER + RNG_INCREMENT;
    return rngState;
}

int Model::randomColor()
{
    return 1 + (nextRandom() % NUM_COLORS);
}

void Model::startNewGame()
{
    memset(grid, EMPTY_COLOR, sizeof(grid));
    headRowIndex = 0;
    
    gridParityOffset = 0;
    for(int row = 0; row < INITIAL_ROWS; row++) {
        int maxCol = HexGrid::isEvenRow(row, gridParityOffset) ? MAX_COLS : (MAX_COLS - 1);
        for(int col = 0; col < maxCol; col++) {
            grid[getPhysicalIndex(row, col)] = randomColor();
        }
    }
    
    gameState = GameState::STATE_IDLE;
    player.currentEgg = static_cast<EggColor>(randomColor());
    player.nextEgg = static_cast<EggColor>(randomColor());
    player.score = 0;
    bullet.active = false;
    
    bullet.x = BULLET_START_X;
    bullet.y = BULLET_START_Y;
    bullet.vx = 0.0f;
    bullet.vy = 0.0f;
    player.aimAngle = 0.0f;

    dinoState = DINO_HOLD;
    dinoAnimTimer = 0;
    
    gridParityOffset = 0;
    dropTimer = 0;
    clearingTimer = 0;
    totalTicks = 0;
    globalOffsetY = 0.0f;
    
    rowSpawnTimer = SPAWN_INTERVAL_START;
    rowSpawnInterval = SPAWN_INTERVAL_START;
    rowsSpawnedCount = 0;
}

void Model::tick()
{
    // Dino animation update
    if (dinoState == DINO_THROW) {
        dinoAnimTimer--;
        if (dinoAnimTimer <= 0) {
            dinoState = DINO_DONE;
            dinoAnimTimer = DINO_DONE_FRAMES;
        }
    } else if (dinoState == DINO_DONE) {
        dinoAnimTimer--;
        if (dinoAnimTimer <= 0) {
            dinoState = DINO_HOLD;
        }
    }
    
    if (gameState == GameState::STATE_IDLE || gameState == GameState::STATE_AIMING || gameState == GameState::STATE_SHOOTING || gameState == GameState::STATE_WIN) {
        totalTicks++;
        
        rowSpawnTimer--;
        if (rowSpawnTimer <= 0) {
            shiftGridDown();
            rowsSpawnedCount++;
            
            int idx = rowsSpawnedCount > 31 ? 31 : rowsSpawnedCount;
            rowSpawnInterval = SPAWN_INTERVAL_LUT[idx];
            
            rowSpawnTimer = rowSpawnInterval;
        }
        // Force spawn khi so luong trung qua it
        if (getEggCount() < MIN_BALLS_THRESHOLD) {
            shiftGridDown();
            rowsSpawnedCount++;
            int idx = rowsSpawnedCount > 31 ? 31 : rowsSpawnedCount;
            rowSpawnInterval = SPAWN_INTERVAL_LUT[idx];
            rowSpawnTimer = rowSpawnInterval;
        }
    }

    if (gameState == GameState::STATE_WIN) {
        clearingTimer--;
        if (clearingTimer <= 0) {
            gameState = GameState::STATE_IDLE;
            player.currentEgg = player.nextEgg;
            player.nextEgg = static_cast<EggColor>(randomColor());
            bullet.active = false;
            if (modelListener) modelListener->notifyTurnEnd();
        }
    }

    if (gameState == GameState::STATE_SHOOTING)
    {
        updateFlyingPhysics();
    }
}

void Model::handleTouchAim(int x, int y)
{
    if (gameState != GameState::STATE_SHOOTING && gameState != GameState::STATE_WIN)
    {
        gameState = GameState::STATE_AIMING;
    }
}

void Model::handleTouchShoot(int x, int y)
{
    if (gameState == GameState::STATE_AIMING || gameState == GameState::STATE_IDLE)
    {
        // Tinh goc ngam
        float dx = x - BULLET_START_X;
        float dy = y - BULLET_START_Y;
        
        // Chan ban ngang hoac ban xuong
        if (dy > AIM_MIN_DY) dy = AIM_MIN_DY;
        
        PhysicsEngine::computeVelocity(dx, dy, BULLET_SPEED, bullet.vx, bullet.vy);
        
        bullet.x = BULLET_START_X;
        bullet.y = BULLET_START_Y;
        bullet.active = true;
        
        gameState = GameState::STATE_SHOOTING;

        // Dino nem animation
        dinoState = DINO_THROW;
        dinoAnimTimer = DINO_THROW_FRAMES;
    }
}

void Model::updateFlyingPhysics()
{
    PhysicsEngine::advance(bullet.x, bullet.y, bullet.vx, bullet.vy);
    
    // 1. Phan xa tuong (dong bo diem doi voi UI) va chan dinh tuong
    PhysicsEngine::reflect(bullet.x, bullet.vx, LEFT_WALL, RIGHT_WALL);
    
    // 2. Kiem tra va cham (Dung ham phi trang thai da tach)
    bool collision = isCollisionAt(bullet.x, bullet.y);
    
    // 3. Xu ly sau va cham
    if (collision) {
        int snapCol, snapRow;
        snapToGrid(bullet.x, bullet.y, snapCol, snapRow);
        
        // Snap that bai hoac o da bi chiem -> bo qua, mat luot
        if (snapCol < 0 || snapRow < 0 || grid[getPhysicalIndex(snapRow, snapCol)] != EMPTY_COLOR) {
            bullet.active = false;
            gameState = GameState::STATE_IDLE;
            player.currentEgg = player.nextEgg;
            player.nextEgg = static_cast<EggColor>(randomColor());
            return;
        }
        
        grid[getPhysicalIndex(snapRow, snapCol)] = static_cast<uint8_t>(player.currentEgg);
        
        int oldScore = player.score;
        checkMatches(snapCol, snapRow);
        
        // An bullet sau khi gan vao luoi
        bullet.active = false;
        
        // Neu khong co bong no/roi (score khong doi) thi moi Game Over neu co bong o hang 9
        if (player.score == oldScore) {
            bool isGameOver = false;
            for (int c = 0; c < MAX_COLS; c++) {
                if (grid[getPhysicalIndex(GAME_OVER_ROW, c)] != EMPTY_COLOR) {
                    isGameOver = true;
                    break;
                }
            }
            
            if (isGameOver) {
                gameState = GameState::STATE_LOSE;
                if (modelListener) modelListener->notifyGameOver();
            } else {
                if (gameState != GameState::STATE_WIN) { 
                    gameState = GameState::STATE_IDLE;
                    player.currentEgg = player.nextEgg;
                    player.nextEgg = static_cast<EggColor>(randomColor());
                }
            }
        }
    }
}

void Model::snapToGrid(float px, float py, int &outCol, int &outRow)
{
    float minDistSq = MAX_DIST_SQ_INIT;
    int bestC = 0, bestR = 0;
    
    int approxRow, approxCol;
    HexGrid::pixelToNearestCell(px, py - globalOffsetY, gridParityOffset, approxRow, approxCol);
    
    int startR = approxRow - COLLISION_SCAN_RADIUS;
    if (startR < 0) startR = 0;
    int endR = approxRow + COLLISION_SCAN_RADIUS;
    if (endR >= MAX_ROWS) endR = MAX_ROWS - 1;
    int startC = approxCol - COLLISION_SCAN_RADIUS;
    if (startC < 0) startC = 0;
    int endC = approxCol + COLLISION_SCAN_RADIUS;
    if (endC >= MAX_COLS) endC = MAX_COLS - 1;
    
    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (HexGrid::isValidCell(r, c, HexGrid::isEvenRow(r, gridParityOffset)) && grid[getPhysicalIndex(r, c)] == EMPTY_COLOR) {
                bool canAttach = false;
                if (r == 0) {
                    canAttach = true;
                } else {
                    for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
                        bool isEven = HexGrid::isEvenRow(r, gridParityOffset);
                        const NeighborOffset* neighbors = HexGrid::getNeighbors(isEven);
                        int nr = r + neighbors[i].dy;
                        int nc = c + neighbors[i].dx;
                        if (HexGrid::isValidCell(nr, nc, HexGrid::isEvenRow(nr, gridParityOffset)) && grid[getPhysicalIndex(nr, nc)] != EMPTY_COLOR) {
                            canAttach = true;
                            break;
                        }
                    }
                }
                
                if (canAttach) {
                    bool isEven = HexGrid::isEvenRow(r, gridParityOffset);
                    float cx = HexGrid::cellToPixelX(c, isEven);
                    float cy = HexGrid::cellToPixelY(r) + globalOffsetY;
                    float distSq = calculateDistanceSq(px, py, cx, cy);
                    if (distSq < minDistSq) {
                        minDistSq = distSq;
                        bestC = c;
                        bestR = r;
                    }
                }
            }
        }
    }
    if (minDistSq >= MAX_DIST_SQ_INIT) {
        outCol = -1;
        outRow = -1;
    } else {
        outCol = bestC;
        outRow = bestR;
    }
}

void Model::checkMatches(int col, int row)
{
    uint8_t targetColor = grid[getPhysicalIndex(row, col)];
    if (targetColor == EMPTY_COLOR) return;
    
    memset(visited, 0, sizeof(visited));
    int qHead = 0, qTail = 0;
    
    int matchCount = 0;
    
    algoQueueStack[qTail++] = (row << 8) | col;
    visited[HexGrid::index(row, col)] = true;
    
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
                if (!visited[nIdx] && grid[getPhysicalIndex(nr, nc)] == targetColor) {
                    visited[nIdx] = true;
                    algoQueueStack[qTail++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    if (matchCount >= GameConstants::MIN_MATCH_COUNT) {
        gameState = GameState::STATE_WIN;
        if (matchCount == 3) {
            player.score += GameConstants::SCORE_MATCH_3;
        } else if (matchCount == 4) {
            player.score += GameConstants::SCORE_MATCH_4;
        } else if (matchCount >= 5) {
            player.score += GameConstants::SCORE_MATCH_5;
        }
        if (player.score > player.highScore) player.highScore = player.score;
        if (modelListener) modelListener->notifyScoreUpdated(player.score);
        
        for (int i = 0; i < matchCount; i++) {
            int curr = matchGroup[i];
            grid[getPhysicalIndex(curr >> 8, curr & 0xFF)] = GameConstants::EMPTY_COLOR;
        }
        
        dropFloatingEggs();
        
        // Thay vi reset ve IDLE ngay, cho 30 ticks (0.5s) cho UI tha roi trung
        clearingTimer = 30;
    }
}

void Model::dropFloatingEggs()
{
    memset(connected, 0, sizeof(connected));
    int top = 0;
    
    for (int c = 0; c < MAX_COLS; c++) {
        if (grid[getPhysicalIndex(0, c)] != EMPTY_COLOR) {
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
                if (!connected[nIdx] && grid[getPhysicalIndex(nr, nc)] != EMPTY_COLOR) {
                    connected[nIdx] = true;
                    algoQueueStack[top++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    int dropCount = 0;
    int r = 0, c = 0;
    for (int i = 0; i < GameConstants::MAX_ROWS * GameConstants::MAX_COLS; i++) {
        if (grid[getPhysicalIndex(r, c)] != GameConstants::EMPTY_COLOR && !connected[i]) {
            grid[getPhysicalIndex(r, c)] = GameConstants::EMPTY_COLOR;
            player.score += GameConstants::SCORE_DROP_ORPHAN;
            dropCount++;
        }
        c++;
        if (c >= MAX_COLS) { c = 0; r++; }
    }
    
    if (player.score > player.highScore) player.highScore = player.score;
    
    if (dropCount > 0) {
        if (modelListener) modelListener->notifyScoreUpdated(player.score);
    }
}

float Model::calculateDistanceSq(float x1, float y1, float x2, float y2) const
{
    return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
}

bool Model::isCollisionAt(float x, float y) const
{
    if (y - GameConstants::EGG_RADIUS <= GameConstants::TOP_WALL + globalOffsetY) {
        return true;
    }
    
    int approxRow, approxCol;
    HexGrid::pixelToNearestCell(x, y - globalOffsetY, gridParityOffset, approxRow, approxCol);
    
    int startR = approxRow - COLLISION_SCAN_RADIUS;
    if (startR < 0) startR = 0;
    int endR = approxRow + COLLISION_SCAN_RADIUS;
    if (endR >= MAX_ROWS) endR = MAX_ROWS - 1;
    int startC = approxCol - COLLISION_SCAN_RADIUS;
    if (startC < 0) startC = 0;
    int endC = approxCol + COLLISION_SCAN_RADIUS;
    if (endC >= MAX_COLS) endC = MAX_COLS - 1;

    float hitRadiusSq = HITBOX_RADIUS * HITBOX_RADIUS;

    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (HexGrid::isValidCell(r, c, HexGrid::isEvenRow(r, gridParityOffset)) && grid[getPhysicalIndex(r, c)] != EMPTY_COLOR) {
                bool isEven = HexGrid::isEvenRow(r, gridParityOffset);
                float px = HexGrid::cellToPixelX(c, isEven);
                float py = HexGrid::cellToPixelY(r) + globalOffsetY;
                
                float dx = px - x;
                float dy = py - y;
                
                float penaltyX = HITBOX_PENALTY_MAX;
                float distToLeft = x - LEFT_WALL;
                float distToRight = RIGHT_WALL - x;
                float minDistToWall = (distToLeft < distToRight) ? distToLeft : distToRight;
                
                if (minDistToWall < HITBOX_WALL_DIST) {
                    if (minDistToWall < 0.0f) minDistToWall = 0.0f;
                    penaltyX = HITBOX_PENALTY_MIN + (HITBOX_PENALTY_MAX - HITBOX_PENALTY_MIN) * (minDistToWall / HITBOX_WALL_DIST);
                }
                
                float distSq = (dx * penaltyX) * (dx * penaltyX) + (dy * 1.0f) * (dy * 1.0f);
                
                if (distSq < hitRadiusSq) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void Model::shiftGridDown() {
    // Neu co trung o hang Game Over, bao ket thuc
    for (int c = 0; c < GameConstants::MAX_COLS; c++) {
        if (grid[getPhysicalIndex(GameConstants::GAME_OVER_ROW, c)] != GameConstants::EMPTY_COLOR) {
            gameState = GameState::STATE_LOSE;
            if (modelListener) modelListener->notifyGameOver();
            return;
        }
    }
    
    // Dich xuong bang cach thay doi headRowIndex (Kien truc Ring Buffer)
    if (--headRowIndex < 0) headRowIndex = GameConstants::MAX_ROWS - 1;
    
    // Dao parity
    gridParityOffset = (gridParityOffset + 1) & 1;
    
    // Random hang 0 moi (100% ra bong, khong ra rong de chong ket starvation)
    int maxC = HexGrid::isEvenRow(0, gridParityOffset) ? GameConstants::MAX_COLS : (GameConstants::MAX_COLS - 1);
    for (int c = 0; c < GameConstants::MAX_COLS; c++) {
        if (c < maxC) {
            grid[getPhysicalIndex(0, c)] = randomColor();
        } else {
            grid[getPhysicalIndex(0, c)] = GameConstants::EMPTY_COLOR;
        }
    }
    
    if (modelListener) modelListener->notifyGridShifted();
}

int Model::getEggCount() const {
    int count = 0;
    for (int r = 0; r < GameConstants::MAX_ROWS; r++) {
        for (int c = 0; c < GameConstants::MAX_COLS; c++) {
            if (grid[getPhysicalIndex(r, c)] != GameConstants::EMPTY_COLOR) count++;
        }
    }
    return count;
}
