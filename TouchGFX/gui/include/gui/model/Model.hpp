#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>


#include <gui/common/GameConstants.hpp>
#include <gui/common/HexGrid.hpp>
#include <gui/common/PhysicsEngine.hpp>
#include <gui/common/CollisionEngine.hpp>

enum GameState {
    STATE_IDLE,
    STATE_AIMING,
    STATE_SHOOTING,
    STATE_WIN,
    STATE_LOSE,
    STATE_PAUSE,

    // Legacy aliases for Presenter/View compatibility
    STATE_FLYING = STATE_SHOOTING,
    STATE_CLEARING = STATE_WIN,
    STATE_GAME_OVER = STATE_LOSE
};

enum class EggColor : uint8_t {
    EMPTY_COLOR = 0,
    RED = 1,
    BLUE = 2,
    GREEN = 3,
    YELLOW = 4,
    PURPLE = 5
};

struct BulletData {
    float x;
    float y;
    float vx;
    float vy;
    bool active;
};

struct PlayerData {
    float aimAngle;
    EggColor currentEgg;
    EggColor nextEgg;
    int score;
    int highScore;
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
    void pauseGame() { 
        if (gameState != GameState::STATE_PAUSE) {
            prePauseState = gameState;
            gameState = GameState::STATE_PAUSE;
        }
    }
    void resumeGame() { 
        if (gameState == GameState::STATE_PAUSE) {
            gameState = prePauseState;
        }
    }

    // Getter cho UI (View) cập nhật
    GameState getGameState() const { return gameState; }
    inline int getPhysicalIndex(int logicalRow, int col) const {
        int r = headRowIndex + logicalRow;
        if (r >= GameConstants::MAX_ROWS) r -= GameConstants::MAX_ROWS;
        return HexGrid::index(r, col);
    }
    uint8_t getGridCell(int row, int col) const { return grid[getPhysicalIndex(row, col)]; }
    float getBulletX() const { return bullet.x; }
    float getBulletY() const { return bullet.y; }
    int getCurrentColor() const { return static_cast<int>(player.currentEgg); }
    int getNextColor() const { return static_cast<int>(player.nextEgg); }
    int getScore() const { return player.score; }
    int getHighScore() const { return player.highScore; }
    DinoState getDinoState() const { return dinoState; }
    bool isBulletVisible() const { return bullet.active; }
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
    GameState prePauseState;
    
    // Tọa độ quả trứng đang bay (float để nội suy mượt mà qua FPU)
    BulletData bullet;
    PlayerData player;

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
    bool visited[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    bool connected[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    int algoQueueStack[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    int matchGroup[GameConstants::MAX_ROWS * GameConstants::MAX_COLS];
    
    // Helper tọa độ & tiện ích DRY

    // Random
    uint32_t nextRandom();
    int randomColor();
};

#endif // MODEL_HPP
