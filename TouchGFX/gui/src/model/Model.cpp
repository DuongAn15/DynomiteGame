#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <math.h>
#include <string.h>

using namespace GameConstants;

Model::Model() : modelListener(0), rngState(RNG_INITIAL_SEED)
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
    memset(grid, EMPTY_COLOR, sizeof(grid));
    headRowIndex = 0;
    
    gridParityOffset = 0;
    for(int row = 0; row < INITIAL_ROWS; row++) {
        int maxCol = ((row + gridParityOffset) % 2 != 0) ? (MAX_COLS - 1) : MAX_COLS;
        for(int col = 0; col < maxCol; col++) {
            grid[getPhysicalIndex(row, col)] = randomColor();
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
        
        // Tốc độ trượt (pixels/frame)
        float dropSpeed = GameConstants::DROP_SPEED_START + (totalTicks / 600) * GameConstants::DROP_SPEED_INCREMENT;
        if (dropSpeed > GameConstants::DROP_SPEED_MAX) {
            dropSpeed = GameConstants::DROP_SPEED_MAX;
        }
        
        globalOffsetY += dropSpeed;
        if (globalOffsetY >= GameConstants::CELL_HEIGHT) {
            globalOffsetY -= GameConstants::CELL_HEIGHT;
            shiftGridDown();
        }
    }

    if (gameState == STATE_CLEARING) {
        clearingTimer--;
        if (clearingTimer <= 0) {
            gameState = STATE_IDLE;
            currentColor = nextColor;
            nextColor = randomColor();
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
        // Tính góc ngắm
        float dx = x - BULLET_START_X;
        float dy = y - BULLET_START_Y;
        
        // Chặn bắn ngang hoặc bắn xuống
        if (dy > AIM_MIN_DY) dy = AIM_MIN_DY;
        
        float length = sqrtf(dx*dx + dy*dy);
        float speed = BULLET_SPEED; // Yêu cầu 1: Giảm tốc độ đạn từ 8.0f xuống 5.0f để dễ quan sát và tránh lỗi tunneling
        
        vx = (dx / length) * speed;
        vy = (dy / length) * speed;
        
        bulletX = BULLET_START_X;
        bulletY = BULLET_START_Y;
        bulletVisible = true;
        
        gameState = STATE_FLYING;

        // Dino ném animation
        dinoState = DINO_THROW;
        dinoAnimTimer = DINO_THROW_FRAMES;
    }
}

void Model::updateFlyingPhysics()
{
    bulletX += vx;
    bulletY += vy;
    
    // 1. Phản xạ tường (đồng bộ điểm dội với UI) và chặn dính tường
    if (bulletX <= LEFT_WALL && vx < 0) {
        bulletX = LEFT_WALL + (LEFT_WALL - bulletX);
        vx = -vx;
    }
    else if (bulletX >= RIGHT_WALL && vx > 0) {
        bulletX = RIGHT_WALL - (bulletX - RIGHT_WALL);
        vx = -vx;
    }
    
    // 2. Kiểm tra va chạm (Dùng hàm phi trạng thái đã tách)
    bool collision = isCollisionAt(bulletX, bulletY);
    
    // 3. Xử lý sau va chạm
    if (collision) {
        int snapCol, snapRow;
        snapToGrid(bulletX, bulletY, snapCol, snapRow);
        
        if (snapRow >= 0 && snapRow < MAX_ROWS && snapCol >= 0 && snapCol < MAX_COLS) {
            grid[getPhysicalIndex(snapRow, snapCol)] = currentColor;
            
            int oldScore = score;
            checkMatches(snapCol, snapRow);
            
            // Ẩn bullet sau khi gắn vào lưới
            bulletVisible = false;
            
            // Yêu cầu: NẾU không có bóng nổ/rơi (score không đổi) thì mới Game Over nếu có bóng ở hàng 9
            if (score == oldScore) {
                bool isGameOver = false;
                for (int r = GAME_OVER_ROW; r < MAX_ROWS; r++) {
                    for (int c = 0; c < MAX_COLS; c++) {
                        if (grid[getPhysicalIndex(r, c)] != EMPTY_COLOR) {
                            isGameOver = true;
                            break;
                        }
                    }
                    if (isGameOver) break;
                }
                
                if (isGameOver) {
                    gameState = STATE_GAME_OVER;
                    if (modelListener) modelListener->notifyGameOver();
                } else {
                    if (gameState != STATE_CLEARING) { 
                        gameState = STATE_IDLE;
                        currentColor = nextColor;
                        nextColor = randomColor();
                    }
                }
            } else {
                // Đã ăn màu thành công -> Hủy bỏ Game Over, tiếp tục chơi.
                // (Hàm checkMatches đã tự động đổi state sang IDLE và tạo bóng mới)
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
    int endR = approxRow + COLLISION_SCAN_RADIUS;
    int startC = approxCol - COLLISION_SCAN_RADIUS;
    int endC = approxCol + COLLISION_SCAN_RADIUS;
    
    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (isValidCell(r, c) && grid[getPhysicalIndex(r, c)] == EMPTY_COLOR) {
                bool canAttach = false;
                if (r == 0) {
                    canAttach = true;
                } else {
                    for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
                        int parity = (r + gridParityOffset) % 2;
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
    outCol = bestC;
    outRow = bestR;
}

void Model::checkMatches(int col, int row)
{
    uint8_t targetColor = grid[getPhysicalIndex(row, col)];
    if (targetColor == EMPTY_COLOR) return;
    
    memset(visited, 0, sizeof(visited));
    int qHead = 0, qTail = 0;
    
    int matchCount = 0;
    
    int startIdx = row * MAX_COLS + col;
    algoQueueStack[qTail++] = startIdx;
    visited[startIdx] = true;
    
    while (qHead < qTail) {
        int curr = algoQueueStack[qHead++];
        matchGroup[matchCount++] = curr;
        
        int r = curr / MAX_COLS;
        int c = curr % MAX_COLS;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            int parity = (r + gridParityOffset) % 2;
            int nr = r + NEIGHBOR_OFFSETS[parity][i][1];
            int nc = c + NEIGHBOR_OFFSETS[parity][i][0];
            
            if (isValidCell(nr, nc)) {
                int nIdx = nr * MAX_COLS + nc;
                if (!visited[nIdx] && grid[getPhysicalIndex(nr, nc)] == targetColor) {
                    visited[nIdx] = true;
                    algoQueueStack[qTail++] = nIdx;
                }
            }
        }
    }
    
    if (matchCount >= GameConstants::MIN_MATCH_COUNT) {
        gameState = STATE_CLEARING;
        if (matchCount == 3) {
            score += GameConstants::SCORE_MATCH_3;
        } else if (matchCount == 4) {
            score += GameConstants::SCORE_MATCH_4;
        } else if (matchCount >= 5) {
            score += GameConstants::SCORE_MATCH_5;
        }
        if (modelListener) modelListener->notifyScoreUpdated(score);
        
        for (int i = 0; i < matchCount; i++) {
            int r = matchGroup[i] / MAX_COLS;
            int c = matchGroup[i] % MAX_COLS;
            grid[getPhysicalIndex(r, c)] = GameConstants::EMPTY_COLOR;
        }
        
        dropFloatingEggs();
        
        // Thay vì reset về IDLE ngay, chờ 30 ticks (0.5s) cho UI thả rơi trứng
        clearingTimer = 30;
    }
}

void Model::dropFloatingEggs()
{
    memset(connected, 0, sizeof(connected));
    int top = 0;
    
    for (int c = 0; c < MAX_COLS; c++) {
        if (grid[getPhysicalIndex(0, c)] != EMPTY_COLOR) {
            algoQueueStack[top++] = c;
            connected[c] = true;
        }
    }
    
    while (top > 0) {
        int curr = algoQueueStack[--top];
        int r = curr / MAX_COLS;
        int c = curr % MAX_COLS;
        
        for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++) {
            int parity = (r + gridParityOffset) % 2;
            int nr = r + NEIGHBOR_OFFSETS[parity][i][1];
            int nc = c + NEIGHBOR_OFFSETS[parity][i][0];
            
            if (isValidCell(nr, nc)) {
                int nIdx = nr * MAX_COLS + nc;
                if (!connected[nIdx] && grid[getPhysicalIndex(nr, nc)] != EMPTY_COLOR) {
                    connected[nIdx] = true;
                    algoQueueStack[top++] = nIdx;
                }
            }
        }
    }
    
    int dropCount = 0;
    for (int r = 0; r < GameConstants::MAX_ROWS; r++) {
        for (int c = 0; c < GameConstants::MAX_COLS; c++) {
            int i = r * GameConstants::MAX_COLS + c;
            if (grid[getPhysicalIndex(r, c)] != GameConstants::EMPTY_COLOR && !connected[i]) {
                grid[getPhysicalIndex(r, c)] = GameConstants::EMPTY_COLOR;
                score += GameConstants::SCORE_DROP_ORPHAN;
                dropCount++;
            }
        }
    }
    
    if (dropCount > 0) {
        if (modelListener) modelListener->notifyScoreUpdated(score);
    }
}

void Model::getCellCenter(int col, int row, float &px, float &py) const
{
    px = ((row + gridParityOffset) % 2 == 0) ? MODEL_CELL_X_EVEN[col] : MODEL_CELL_X_ODD[col];
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
    int maxC = ((row + gridParityOffset) % 2 != 0) ? (MAX_COLS - 1) : MAX_COLS;
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
    int endR = approxRow + COLLISION_SCAN_RADIUS;
    int startC = approxCol - COLLISION_SCAN_RADIUS;
    int endC = approxCol + COLLISION_SCAN_RADIUS;

    float hitRadiusSq = HITBOX_RADIUS * HITBOX_RADIUS;

    for (int r = startR; r <= endR; r++) {
        for (int c = startC; c <= endC; c++) {
            if (isValidCell(r, c) && grid[getPhysicalIndex(r, c)] != EMPTY_COLOR) {
                float px = ((r + gridParityOffset) % 2 == 0) ? MODEL_CELL_X_EVEN[c] : MODEL_CELL_X_ODD[c];
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
    // Nếu có trứng ở hàng Game Over, báo kết thúc
    for (int c = 0; c < GameConstants::MAX_COLS; c++) {
        if (grid[getPhysicalIndex(GameConstants::GAME_OVER_ROW, c)] != GameConstants::EMPTY_COLOR) {
            gameState = STATE_GAME_OVER;
            if (modelListener) modelListener->notifyGameOver();
            return;
        }
    }
    
    // Dịch xuống bằng cách thay đổi headRowIndex (Kiến trúc Ring Buffer)
    headRowIndex = (headRowIndex - 1 + GameConstants::MAX_ROWS) % GameConstants::MAX_ROWS;
    
    // Đảo parity
    gridParityOffset = (gridParityOffset + 1) % 2;
    
    // Random hàng 0 mới (100% ra bóng, không ra rỗng để chống kẹt starvation)
    int maxC = ((0 + gridParityOffset) % 2 != 0) ? (GameConstants::MAX_COLS - 1) : GameConstants::MAX_COLS;
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
