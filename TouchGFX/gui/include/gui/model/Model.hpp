#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>


#include <gui/common/GameConstants.hpp>

enum GameState {
    STATE_IDLE,
    STATE_AIMING,
    STATE_FLYING,
    STATE_CLEARING,
    STATE_GAME_OVER,
    STATE_PAUSE
};

enum DinoState {
    DINO_HOLD,
    DINO_THROW,
    DINO_DONE
};

struct TrajectoryPoint {
    float x;
    float y;
};

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick(); // Gọi mỗi frame (60Hz)

    // Nhận sự kiện chạm từ View (tạm thời thay thế nút cứng)
    void handleTouchAim(int x, int y);
    void handleTouchShoot(int x, int y);
    void swapColor();

    // Bắt đầu game mới
    void startNewGame();
    void pauseGame() { 
        if (gameState != STATE_PAUSE) {
            prePauseState = gameState;
            gameState = STATE_PAUSE;
        }
    }
    void resumeGame() { 
        if (gameState == STATE_PAUSE) {
            gameState = prePauseState;
        }
    }

    // Getter cho UI (View) cập nhật
    GameState getGameState() const { return gameState; }
    inline int getPhysicalIndex(int logicalRow, int col) const {
        int r = headRowIndex + logicalRow;
        if (r >= GameConstants::MAX_ROWS) r -= GameConstants::MAX_ROWS;
        return r * GameConstants::MAX_COLS + col;
    }
    uint8_t getGridCell(int row, int col) const { return grid[getPhysicalIndex(row, col)]; }
    float getBulletX() const { return bulletX; }
    float getBulletY() const { return bulletY; }
    int getCurrentColor() const { return currentColor; }
    int getNextColor() const { return nextColor; }
    int getScore() const { return score; }
    int getHighScore() const { return highScore; }
    DinoState getDinoState() const { return dinoState; }
    bool isBulletVisible() const { return bulletVisible; }
    float getGlobalOffsetY() const { return globalOffsetY; }
    int getGridParityOffset() const { return gridParityOffset; }
    void getGridData(uint8_t* out) const;
    void calculateTrajectory(float startX, float startY, float dx, float dy, TrajectoryPoint* outPath, int& outCount, int maxSteps);
    
    // Predictive Raycasting
    bool isCollisionAt(float x, float y) const;

protected:
    ModelListener* modelListener;

private:
    int cachedEggCount = 0;
    // Core game data - Không dùng bit-packing, dùng Flatten array
    uint8_t grid[GameConstants::MAX_CELLS];
    int headRowIndex = 0;
    GameState gameState;
    GameState prePauseState;
    
    // Tọa độ quả trứng đang bay (float để nội suy mượt mà qua FPU)
    float bulletX;
    float bulletY;
    float vx;
    float vy;

    int currentColor;
    int nextColor;
    int score;
    int highScore;
    bool bulletVisible;

    // Dino animation
    DinoState dinoState;
    int dinoAnimTimer;
    uint32_t rngState;
    
    // Survival Mode
    int gridParityOffset;
    int dropTimer;
    int clearingTimer;
    int totalTicks;
    uint32_t systemTicks;
    float globalOffsetY;
    
    // Row spawn timer (frame counting)
    int rowSpawnTimer;
    int rowSpawnInterval;
    int rowsSpawnedCount;

    // Các hàm nội bộ của Core Game
    void updateFlyingPhysics();
    void snapToGrid(float px, float py, int &outCol, int &outRow);
    void checkMatches(int col, int row);
    void dropFloatingEggs();
    void shiftGridDown();
    int getEggCount() const;
    
    // Thuật toán: Fixed-size tĩnh để tránh overhead cấp phát
    bool visited[GameConstants::MAX_CELLS];
    bool connected[GameConstants::MAX_CELLS];
    int algoQueueStack[GameConstants::MAX_CELLS];
    int matchGroup[GameConstants::MAX_CELLS];
    
    // Helper tọa độ & tiện ích DRY
    void getCellCenter(int col, int row, float &px, float &py) const;
    void getApproxCell(float x, float y, int &col, int &row) const;
    bool isValidCell(int row, int col) const;
    float calculateDistanceSq(float x1, float y1, float x2, float y2) const;

    // Random
    uint32_t nextRandom();
    int randomColor();
};

#endif // MODEL_HPP
