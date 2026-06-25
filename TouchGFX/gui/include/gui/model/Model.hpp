#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

#include <gui/common/GameConstants.hpp>

enum GameState {
    STATE_IDLE,
    STATE_AIMING,
    STATE_FLYING,
    STATE_CLEARING,
    STATE_GAME_OVER
};

enum DinoState {
    DINO_HOLD,
    DINO_THROW,
    DINO_DONE
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

    // Bắt đầu game mới
    void startNewGame();

    // Getter cho UI (View) cập nhật
    GameState getGameState() const { return gameState; }
    inline int getPhysicalIndex(int logicalRow, int col) const {
        return ((headRowIndex + logicalRow) % GameConstants::MAX_ROWS) * GameConstants::MAX_COLS + col;
    }
    uint8_t getGridCell(int row, int col) const { return grid[getPhysicalIndex(row, col)]; }
    float getBulletX() const { return bulletX; }
    float getBulletY() const { return bulletY; }
    int getCurrentColor() const { return currentColor; }
    int getNextColor() const { return nextColor; }
    int getScore() const { return score; }
    DinoState getDinoState() const { return dinoState; }
    bool isBulletVisible() const { return bulletVisible; }
    float getGlobalOffsetY() const { return globalOffsetY; }
    int getGridParityOffset() const { return gridParityOffset; }
    
    // Predictive Raycasting
    bool isCollisionAt(float x, float y) const;

protected:
    ModelListener* modelListener;

private:
    // Core game data - Không dùng bit-packing, dùng Flatten array
    uint8_t grid[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    int headRowIndex = 0;
    GameState gameState;
    
    // Tọa độ quả trứng đang bay (float để nội suy mượt mà qua FPU)
    float bulletX;
    float bulletY;
    float vx;
    float vy;

    int currentColor;
    int nextColor;
    int score;
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
    float globalOffsetY;

    // Các hàm nội bộ của Core Game
    void updateFlyingPhysics();
    void snapToGrid(float px, float py, int &outCol, int &outRow);
    void checkMatches(int col, int row);
    void dropFloatingEggs();
    void shiftGridDown();
    int getEggCount() const;
    
    // Thuật toán: Fixed-size tĩnh để tránh overhead cấp phát
    bool visited[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    bool connected[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    int algoQueueStack[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    int matchGroup[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    
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
