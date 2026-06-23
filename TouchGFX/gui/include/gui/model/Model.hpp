#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>

// --- GAME LOGIC CONSTANTS ---
#define MAX_ROWS 12
#define MAX_COLS 11
#define EMPTY_COLOR 0
#define EGG_RADIUS 12

#define LEFT_WALL 8
#define RIGHT_WALL 232
#define TOP_WALL 16

enum GameState {
    STATE_IDLE,
    STATE_AIMING,
    STATE_FLYING,
    STATE_CLEARING,
    STATE_GAME_OVER
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
    uint8_t getGridCell(int row, int col) const { return grid[row * MAX_COLS + col]; }
    float getBulletX() const { return bulletX; }
    float getBulletY() const { return bulletY; }
    int getCurrentColor() const { return currentColor; }
    int getNextColor() const { return nextColor; }
    int getScore() const { return score; }

protected:
    ModelListener* modelListener;

private:
    // Core game data - Không dùng bit-packing, dùng Flatten array
    uint8_t grid[MAX_ROWS * MAX_COLS];
    GameState gameState;
    
    // Tọa độ quả trứng đang bay (float để nội suy mượt mà qua FPU)
    float bulletX;
    float bulletY;
    float vx;
    float vy;

    int currentColor;
    int nextColor;
    int score;

    // Các hàm nội bộ của Core Game
    void updateFlyingPhysics();
    void snapToGrid(float px, float py, int &outCol, int &outRow);
    void checkMatches(int col, int row);
    void dropFloatingEggs();
    
    // Helper tọa độ
    void getCellCenter(int col, int row, float &px, float &py);
    float calculateDistance(float x1, float y1, float x2, float y2);
};

#endif // MODEL_HPP
