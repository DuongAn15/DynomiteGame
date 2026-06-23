#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <math.h>
#include <string.h>

// Hex grid offsets (0=Empty, 1-7=Colors)
const int EVEN_NEIGHBORS[6][2] = {{-1,-1}, { 0,-1}, {-1, 0}, { 1, 0}, {-1, 1}, { 0, 1}};
const int ODD_NEIGHBORS[6][2]  = {{ 0,-1}, { 1,-1}, {-1, 0}, { 1, 0}, { 0, 1}, { 1, 1}};

// Screen and Grid Dimensions
#define CELL_WIDTH 24
#define CELL_HEIGHT 28 // Khoảng cách Y giữa 2 hàng so le
#define GRID_START_X 12
#define GRID_START_Y 20

Model::Model() : modelListener(0)
{
    startNewGame();
}

void Model::startNewGame()
{
    memset(grid, EMPTY_COLOR, sizeof(grid));
    
    // Khởi tạo một vài hàng trứng ngẫu nhiên ban đầu để test
    for(int row = 0; row < 3; row++) {
        int maxCol = (row % 2 != 0) ? (MAX_COLS - 1) : MAX_COLS;
        for(int col = 0; col < maxCol; col++) {
            grid[row * MAX_COLS + col] = 1 + ((row + col) % 6); 
        }
    }
    
    gameState = STATE_IDLE;
    currentColor = 1;
    nextColor = 2;
    score = 0;
    
    bulletX = 120.0f;
    bulletY = 300.0f;
    vx = 0.0f;
    vy = 0.0f;
}

void Model::tick()
{
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
        // Có thể bổ sung: modelListener->aimUpdated(angle);
    }
}

void Model::handleTouchShoot(int x, int y)
{
    if (gameState == STATE_AIMING || gameState == STATE_IDLE)
    {
        // Tính góc ngắm
        float dx = x - 120.0f; // Bệ phóng ở giữa màn hình X (120)
        float dy = y - 300.0f; // Bệ phóng ở dưới màn hình Y (300)
        
        // Chặn bắn ngang hoặc bắn xuống
        if (dy > -10.0f) dy = -10.0f;
        
        float length = sqrtf(dx*dx + dy*dy);
        float speed = 8.0f; // Tốc độ đạn
        
        vx = (dx / length) * speed;
        vy = (dy / length) * speed;
        
        bulletX = 120.0f;
        bulletY = 300.0f;
        
        gameState = STATE_FLYING;
    }
}

void Model::updateFlyingPhysics()
{
    bulletX += vx;
    bulletY += vy;
    
    // 1. Phản xạ tường (Wall bounce)
    if (bulletX - EGG_RADIUS <= LEFT_WALL) {
        bulletX = LEFT_WALL + EGG_RADIUS;
        vx = -vx;
    }
    else if (bulletX + EGG_RADIUS >= RIGHT_WALL) {
        bulletX = RIGHT_WALL - EGG_RADIUS;
        vx = -vx;
    }
    
    // 2. Kiểm tra va chạm (Đụng trần hoặc đụng trứng khác)
    bool collision = false;
    
    if (bulletY - EGG_RADIUS <= TOP_WALL) {
        collision = true;
    }
    else {
        // Va chạm hình tròn với các trứng trên lưới
        for (int r = 0; r < MAX_ROWS; r++) {
            for (int c = 0; c < MAX_COLS; c++) {
                if (grid[r * MAX_COLS + c] != EMPTY_COLOR) {
                    float px, py;
                    getCellCenter(c, r, px, py);
                    if (calculateDistance(bulletX, bulletY, px, py) < EGG_RADIUS * 2) {
                        collision = true;
                        break;
                    }
                }
            }
            if (collision) break;
        }
    }
    
    // 3. Xử lý sau va chạm
    if (collision) {
        int snapCol, snapRow;
        snapToGrid(bulletX, bulletY, snapCol, snapRow);
        
        if (snapRow >= 0 && snapRow < MAX_ROWS && snapCol >= 0 && snapCol < MAX_COLS) {
            grid[snapRow * MAX_COLS + snapCol] = currentColor;
            
            // Tìm cụm trứng cùng màu và phá hủy
            checkMatches(snapCol, snapRow);
            
            if (gameState != STATE_CLEARING) { 
                gameState = STATE_IDLE;
                currentColor = nextColor;
                nextColor = 1 + (score % 6); // Đổi màu đạn tiếp theo (test)
            }
        }
    }
}

void Model::snapToGrid(float px, float py, int &outCol, int &outRow)
{
    float minDist = 99999.0f;
    int bestC = 0, bestR = 0;
    
    // Ước lượng hàng Y
    int approxRow = (int)((py - GRID_START_Y) / CELL_HEIGHT);
    if (approxRow < 0) approxRow = 0;
    if (approxRow >= MAX_ROWS) approxRow = MAX_ROWS - 1;
    
    // Dò 5 hàng lân cận để tìm ô trống có khoảng cách tâm gần nhất
    for (int r = approxRow - 2; r <= approxRow + 2; r++) {
        if (r < 0 || r >= MAX_ROWS) continue;
        for (int c = 0; c < MAX_COLS; c++) {
            if (grid[r * MAX_COLS + c] == EMPTY_COLOR) {
                float cx, cy;
                getCellCenter(c, r, cx, cy);
                float dist = calculateDistance(px, py, cx, cy);
                if (dist < minDist) {
                    minDist = dist;
                    bestC = c;
                    bestR = r;
                }
            }
        }
    }
    outCol = bestC;
    outRow = bestR;
}

void Model::checkMatches(int col, int row)
{
    uint8_t targetColor = grid[row * MAX_COLS + col];
    if (targetColor == EMPTY_COLOR) return;
    
    bool visited[MAX_ROWS * MAX_COLS] = {false};
    int queue[MAX_ROWS * MAX_COLS];
    int qHead = 0, qTail = 0;
    
    int matchGroup[MAX_ROWS * MAX_COLS];
    int matchCount = 0;
    
    // BFS tĩnh (0 overhead cấp phát)
    int startIdx = row * MAX_COLS + col;
    queue[qTail++] = startIdx;
    visited[startIdx] = true;
    
    while (qHead < qTail) {
        int curr = queue[qHead++];
        matchGroup[matchCount++] = curr;
        
        int r = curr / MAX_COLS;
        int c = curr % MAX_COLS;
        
        const int (*neighbors)[2] = (r % 2 == 0) ? EVEN_NEIGHBORS : ODD_NEIGHBORS;
        
        for (int i = 0; i < 6; i++) {
            int nr = r + neighbors[i][1];
            int nc = c + neighbors[i][0];
            
            if (nr >= 0 && nr < MAX_ROWS && nc >= 0 && nc < MAX_COLS) {
                int nIdx = nr * MAX_COLS + nc;
                if (!visited[nIdx] && grid[nIdx] == targetColor) {
                    visited[nIdx] = true;
                    queue[qTail++] = nIdx;
                }
            }
        }
    }
    
    // Xóa cụm nếu gom đủ >= 3 quả
    if (matchCount >= 3) {
        gameState = STATE_CLEARING;
        for (int i = 0; i < matchCount; i++) {
            grid[matchGroup[i]] = EMPTY_COLOR;
            score += 10;
        }
        
        // Rớt những quả lơ lửng
        dropFloatingEggs();
        
        gameState = STATE_IDLE;
        currentColor = nextColor;
        nextColor = 1 + (score % 6);
        
        // TODO: modelListener->scoreUpdated(score);
    }
}

void Model::dropFloatingEggs()
{
    bool connected[MAX_ROWS * MAX_COLS] = {false};
    int stack[MAX_ROWS * MAX_COLS];
    int top = 0;
    
    // DFS dò từ trần nhà (Row 0)
    for (int c = 0; c < MAX_COLS; c++) {
        if (grid[c] != EMPTY_COLOR) {
            stack[top++] = c;
            connected[c] = true;
        }
    }
    
    while (top > 0) {
        int curr = stack[--top];
        int r = curr / MAX_COLS;
        int c = curr % MAX_COLS;
        
        const int (*neighbors)[2] = (r % 2 == 0) ? EVEN_NEIGHBORS : ODD_NEIGHBORS;
        
        for (int i = 0; i < 6; i++) {
            int nr = r + neighbors[i][1];
            int nc = c + neighbors[i][0];
            
            if (nr >= 0 && nr < MAX_ROWS && nc >= 0 && nc < MAX_COLS) {
                int nIdx = nr * MAX_COLS + nc;
                if (!connected[nIdx] && grid[nIdx] != EMPTY_COLOR) {
                    connected[nIdx] = true;
                    stack[top++] = nIdx;
                }
            }
        }
    }
    
    // Cắt đứt rễ của những quả lơ lửng
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) {
        if (grid[i] != EMPTY_COLOR && !connected[i]) {
            grid[i] = EMPTY_COLOR;
            score += 20; // Thưởng ăn chùm
        }
    }
}

void Model::getCellCenter(int col, int row, float &px, float &py)
{
    px = GRID_START_X + col * CELL_WIDTH;
    if (row % 2 != 0) {
        px += CELL_WIDTH / 2.0f; // Dịch offset sole cho hàng lẻ
    }
    py = GRID_START_Y + row * CELL_HEIGHT;
}

float Model::calculateDistance(float x1, float y1, float x2, float y2)
{
    return sqrtf((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}
