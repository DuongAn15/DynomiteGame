#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <math.h>
#include <string.h>

#include <gui/common/AudioManager.hpp>
#include "audio_sfx_shoot.h"
#include "audio_sfx_hit.h"
#include "audio_sfx_match.h"
#include "audio_sfx_drop.h"
#include "audio_sfx_gameover.h"

using namespace GameConstants;

Model::Model() : modelListener(0), highScore(0), rngState(RNG_INITIAL_SEED)
{
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
    cachedEggCount = 0;
    memset(grid, EMPTY_COLOR, sizeof(grid));
    headRowIndex = 0;
    
    gridParityOffset = 0;
    for(int row = 0; row < INITIAL_ROWS; row++) {
        int maxCol = ((row + gridParityOffset) & 1) ? (MAX_COLS - 1) : MAX_COLS;
        for(int col = 0; col < maxCol; col++) {
            grid[getPhysicalIndex(row, col)] = randomColor();
            cachedEggCount++;
        }
    }
    
    gameState = STATE_IDLE;
    currentColor = randomColor();
    nextColor = randomColor();
    score = 0;
    bulletVisible = false;
    
    bulletX = BULLET_START_X;
    bulletY = BULLET_START_Y;
    vx = 0.0f;
    vy = 0.0f;

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
    
    if (gameState == STATE_IDLE || gameState == STATE_AIMING || gameState == STATE_FLYING || gameState == STATE_CLEARING) {
        totalTicks++;
        
        rowSpawnTimer--;
        if (rowSpawnTimer <= 0) {
            shiftGridDown();
            rowsSpawnedCount++;
            
            int idx = rowsSpawnedCount > MAX_SPAWN_LUT_INDEX ? MAX_SPAWN_LUT_INDEX : rowsSpawnedCount;
            rowSpawnInterval = SPAWN_INTERVAL_LUT[idx];
            
            rowSpawnTimer = rowSpawnInterval;
        }
        // Force spawn khi so luong trung qua it
        if (getEggCount() < MIN_BALLS_THRESHOLD) {
            shiftGridDown();
            rowsSpawnedCount++;
            int idx = rowsSpawnedCount > MAX_SPAWN_LUT_INDEX ? MAX_SPAWN_LUT_INDEX : rowsSpawnedCount;
            rowSpawnInterval = SPAWN_INTERVAL_LUT[idx];
            rowSpawnTimer = rowSpawnInterval;
        }
    }

    if (gameState == STATE_CLEARING) {
        clearingTimer--;
        if (clearingTimer <= 0) {
            gameState = STATE_IDLE;
            bulletVisible = false;
            if (modelListener) modelListener->notifyTurnEnd();
        }
    }

    if (gameState == STATE_FLYING)
    {
        updateFlyingPhysics();
    }
}

void Model::handleTouchAim(int x, int y)
{
    if (gameState != STATE_FLYING && gameState != STATE_CLEARING)
    {
        gameState = STATE_AIMING;
    }
}

void Model::handleTouchShoot(int x, int y)
{
    if (gameState == STATE_AIMING || gameState == STATE_IDLE)
    {
        AudioManager::playSFX(audio_sfx_shoot, audio_sfx_shoot_length);
        
        // Tinh goc ngam
        float dx = x - BULLET_START_X;
        float dy = y - BULLET_START_Y;
        
        // Chan ban ngang hoac ban xuong
        if (dy > AIM_MIN_DY) dy = AIM_MIN_DY;
        
        float length = sqrtf(dx*dx + dy*dy);
        float speed = BULLET_SPEED;
        
        vx = (dx / length) * speed;
        vy = (dy / length) * speed;
        
        bulletX = BULLET_START_X;
        bulletY = BULLET_START_Y;
        bulletVisible = true;
        
        gameState = STATE_FLYING;

        // Dino nem animation
        dinoState = DINO_THROW;
        dinoAnimTimer = DINO_THROW_FRAMES;
    }
}

void Model::updateFlyingPhysics()
{
    bulletX += vx;
    bulletY += vy;
    
    // 1. Phan xa tuong (dong bo diem doi voi UI) va chan dinh tuong
    if (bulletX <= LEFT_WALL && vx < 0) {
        bulletX = LEFT_WALL + (LEFT_WALL - bulletX);
        vx = -vx;
    }
    else if (bulletX >= RIGHT_WALL && vx > 0) {
        bulletX = RIGHT_WALL - (bulletX - RIGHT_WALL);
        vx = -vx;
    }
    
    // 2. Kiem tra va cham (Dung ham phi trang thai da tach)
    bool collision = isCollisionAt(bulletX, bulletY);
    
    // 3. Xu ly sau va cham
    if (collision) {
        int snapCol, snapRow;
        snapToGrid(bulletX, bulletY, snapCol, snapRow);
        
        // Snap that bai hoac o da bi chiem -> bo qua, mat luot
        if (snapCol < 0 || snapRow < 0 || grid[getPhysicalIndex(snapRow, snapCol)] != EMPTY_COLOR) {
            bulletVisible = false;
            gameState = STATE_IDLE;
            currentColor = nextColor;
            nextColor = randomColor();
            return;
        }
        
        grid[getPhysicalIndex(snapRow, snapCol)] = currentColor;
        cachedEggCount++;
        
        int oldScore = score;
        checkMatches(snapCol, snapRow);
        
        // An bullet sau khi gan vao luoi
        bulletVisible = false;
        currentColor = nextColor;
        nextColor = randomColor();
        
        // Neu khong co bong no/roi (score khong doi) thi moi Game Over neu co bong o hang 9
        if (score == oldScore) {
            AudioManager::playSFX(audio_sfx_hit, audio_sfx_hit_length);
            
            bool isGameOver = false;
            for (int c = 0; c < GameConstants::MAX_COLS; c++) {
                if (grid[getPhysicalIndex(GAME_OVER_ROW, c)] != GameConstants::EMPTY_COLOR) {
                    isGameOver = true;
                    break;
                }
            }
            
            if (isGameOver) {
                AudioManager::stopBGM();
                AudioManager::playSFX(audio_sfx_gameover, audio_sfx_gameover_length);
                gameState = STATE_GAME_OVER;
                if (modelListener) modelListener->notifyGameOver();
            } else {
                if (gameState != STATE_CLEARING) { 
                    gameState = STATE_IDLE;
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
    getApproxCell(px, py, approxCol, approxRow);
    
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
            if (isValidCell(r, c) && grid[getPhysicalIndex(r, c)] == EMPTY_COLOR) {
                bool canAttach = false;
                if (r == 0) {
                    canAttach = true;
                } else {
                    for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
                        int parity = (r + gridParityOffset) & 1;
                        int nr = r + NEIGHBOR_OFFSETS[parity][i][1];
                        int nc = c + NEIGHBOR_OFFSETS[parity][i][0];
                        if (isValidCell(nr, nc) && grid[getPhysicalIndex(nr, nc)] != EMPTY_COLOR) {
                            canAttach = true;
                            break;
                        }
                    }
                }
                
                if (canAttach) {
                    float cx, cy;
                    getCellCenter(c, r, cx, cy);
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
    visited[row * MAX_COLS + col] = true;
    
    while (qHead < qTail) {
        int curr = algoQueueStack[qHead++];
        matchGroup[matchCount++] = curr;
        
        int r = curr >> 8;
        int c = curr & 0xFF;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            int parity = (r + gridParityOffset) & 1;
            int nr = r + NEIGHBOR_OFFSETS[parity][i][1];
            int nc = c + NEIGHBOR_OFFSETS[parity][i][0];
            
            if (isValidCell(nr, nc)) {
                int nIdx = nr * MAX_COLS + nc;
                if (!visited[nIdx] && grid[getPhysicalIndex(nr, nc)] == targetColor) {
                    visited[nIdx] = true;
                    algoQueueStack[qTail++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    if (matchCount >= GameConstants::MIN_MATCH_COUNT) {
        AudioManager::playSFX(audio_sfx_match, audio_sfx_match_length);
        gameState = STATE_CLEARING;
        if (matchCount == 3) {
            score += GameConstants::SCORE_MATCH_3;
        } else if (matchCount == 4) {
            score += GameConstants::SCORE_MATCH_4;
        } else if (matchCount >= 5) {
            score += GameConstants::SCORE_MATCH_5;
        }
        if (score > highScore) highScore = score;
        if (modelListener) modelListener->notifyScoreUpdated(score);
        
        for (int i = 0; i < matchCount; i++) {
            int curr = matchGroup[i];
            grid[getPhysicalIndex(curr >> 8, curr & 0xFF)] = GameConstants::EMPTY_COLOR;
            cachedEggCount--;
        }
        
        dropFloatingEggs();
        
        // Thay vi reset ve IDLE ngay, cho 30 ticks (0.5s) cho UI tha roi trung
        clearingTimer = CLEARING_FRAMES_DELAY;
    }
}

void Model::dropFloatingEggs()
{
    memset(connected, 0, sizeof(connected));
    int top = 0;
    
    for (int c = 0; c < MAX_COLS; c++) {
        if (grid[getPhysicalIndex(0, c)] != EMPTY_COLOR) {
            algoQueueStack[top++] = (0 << 8) | c;
            connected[0 * MAX_COLS + c] = true;
        }
    }
    
    while (top > 0) {
        int curr = algoQueueStack[--top];
        int r = curr >> 8;
        int c = curr & 0xFF;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            int parity = (r + gridParityOffset) & 1;
            int nr = r + NEIGHBOR_OFFSETS[parity][i][1];
            int nc = c + NEIGHBOR_OFFSETS[parity][i][0];
            
            if (isValidCell(nr, nc)) {
                int nIdx = nr * MAX_COLS + nc;
                if (!connected[nIdx] && grid[getPhysicalIndex(nr, nc)] != EMPTY_COLOR) {
                    connected[nIdx] = true;
                    algoQueueStack[top++] = (nr << 8) | nc;
                }
            }
        }
    }
    
    int dropCount = 0;
    int r = 0, c = 0;
    for (int i = 0; i < GameConstants::MAX_CELLS; i++) {
        if (grid[getPhysicalIndex(r, c)] != GameConstants::EMPTY_COLOR && !connected[i]) {
            grid[getPhysicalIndex(r, c)] = GameConstants::EMPTY_COLOR;
            cachedEggCount--;
            score += GameConstants::SCORE_DROP_ORPHAN;
            dropCount++;
        }
        c++;
        if (c >= MAX_COLS) { c = 0; r++; }
    }
    
    if (score > highScore) highScore = score;
    
    if (dropCount > 0) {
        AudioManager::playSFX(audio_sfx_drop, audio_sfx_drop_length);
        if (modelListener) modelListener->notifyScoreUpdated(score);
    }
}

void Model::getCellCenter(int col, int row, float &px, float &py) const
{
    px = ((row + gridParityOffset) & 1) == 0 ? MODEL_CELL_X_EVEN[col] : MODEL_CELL_X_ODD[col];
    py = MODEL_CELL_Y[row] + globalOffsetY;
}

float Model::calculateDistanceSq(float x1, float y1, float x2, float y2) const
{
    return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
}

void Model::getApproxCell(float x, float y, int &col, int &row) const
{
    row = (int)roundf((y - GameConstants::GRID_START_Y - globalOffsetY) / GameConstants::CELL_HEIGHT);
    col = (int)roundf((x - GameConstants::GRID_START_X) / GameConstants::CELL_WIDTH);
}

bool Model::isValidCell(int row, int col) const
{
    if (row < 0 || row >= MAX_ROWS || col < 0) return false;
    int maxC = ((row + gridParityOffset) & 1) ? (MAX_COLS - 1) : MAX_COLS;
    return col < maxC;
}

bool Model::isCollisionAt(float x, float y) const
{
    if (y - GameConstants::EGG_RADIUS <= GameConstants::TOP_WALL + globalOffsetY) {
        return true;
    }
    
    int approxRow, approxCol;
    getApproxCell(x, y, approxCol, approxRow);
    
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
            if (isValidCell(r, c) && grid[getPhysicalIndex(r, c)] != EMPTY_COLOR) {
                float px = ((r + gridParityOffset) & 1) == 0 ? MODEL_CELL_X_EVEN[c] : MODEL_CELL_X_ODD[c];
                float py = MODEL_CELL_Y[r] + globalOffsetY;
                
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
            AudioManager::stopBGM();
            AudioManager::playSFX(audio_sfx_gameover, audio_sfx_gameover_length);
            gameState = STATE_GAME_OVER;
            if (modelListener) modelListener->notifyGameOver();
            return;
        }
    }
    
    // Dich xuong bang cach thay doi headRowIndex (Kien truc Ring Buffer)
    if (--headRowIndex < 0) headRowIndex = GameConstants::MAX_ROWS - 1;
    
    // Dao parity
    gridParityOffset = (gridParityOffset + 1) & 1;
    
    // Random hang 0 moi (100% ra bong, khong ra rong de chong ket starvation)
    int maxC = (gridParityOffset & 1) ? (GameConstants::MAX_COLS - 1) : GameConstants::MAX_COLS;
    for (int c = 0; c < GameConstants::MAX_COLS; c++) {
        if (c < maxC) {
            grid[getPhysicalIndex(0, c)] = randomColor();
            cachedEggCount++;
        } else {
            grid[getPhysicalIndex(0, c)] = GameConstants::EMPTY_COLOR;
        }
    }
    
    if (modelListener) modelListener->notifyGridShifted();
}

int Model::getEggCount() const {
    return cachedEggCount;
}

void Model::getGridData(uint8_t* out) const {
    for (int r = 0; r < GameConstants::MAX_ROWS; r++) {
        for (int c = 0; c < GameConstants::MAX_COLS; c++) {
            out[r * GameConstants::MAX_COLS + c] = grid[getPhysicalIndex(r, c)];
        }
    }
}

void Model::swapColor()
{
    int temp = currentColor;
    currentColor = nextColor;
    nextColor = temp;
    
    dinoState = DINO_THROW;
    dinoAnimTimer = GameConstants::DINO_THROW_FRAMES;
}
