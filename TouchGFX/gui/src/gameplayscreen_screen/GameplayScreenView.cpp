#include <gui/gameplayscreen_screen/GameplayScreenView.hpp>
#include <BitmapDatabase.hpp>
#include <gui/common/PhysicsEngine.hpp>
#include <gui/common/HexGrid.hpp>
#include <cstring>
#include <touchgfx/Color.hpp>
#include <math.h>

using namespace GameConstants;

GameplayScreenView::GameplayScreenView()
{
    std::memset(shadowGrid, SHADOW_GRID_INIT_VAL, sizeof(shadowGrid));
    lastBulletX = -1;
    lastBulletY = -1;
    lastDinoState = -1;
    lastNextColor = -1;
    lastColor = -1;
    smoothedAimX = BULLET_START_X;
    smoothedAimY = BULLET_START_Y;
}

void GameplayScreenView::setupScreen()
{
    GameplayScreenViewBase::setupScreen();
    
    // Khởi tạo dino hiển thị next color
    int nextColor = presenter->getNextColor();
    imgDinoHold.setBitmap(touchgfx::Bitmap(getColorBitmap(nextColor + DINO_COLOR_OFFSET))); // dino color mapping
    imgDinoHold.invalidate();
    lastNextColor = nextColor;
    lastDinoState = -1;
    lastColor = -1;
    lastBulletX = -1;
    lastBulletY = -1;

    // Yêu cầu 1: Mở rộng lưới eggGrid
    eggGrid.setWidth(EGG_GRID_WIDTH);

    // Yêu cầu 2: Khởi tạo thanh nét đứt (trajectory)
    isAiming = false;
    for (int i = 0; i < TRAJECTORY_DOTS_COUNT; i++) {
        trajectoryDots[i].setWidth(TRAJECTORY_DOT_SIZE);
        trajectoryDots[i].setHeight(TRAJECTORY_DOT_SIZE);
        trajectoryDots[i].setColor(touchgfx::Color::getColorFromRGB(TRAJECTORY_DOT_COLOR_R, TRAJECTORY_DOT_COLOR_G, TRAJECTORY_DOT_COLOR_B));
        trajectoryDots[i].setVisible(false);
        add(trajectoryDots[i]);
    }
}

void GameplayScreenView::tearDownScreen()
{
    GameplayScreenViewBase::tearDownScreen();
}

void GameplayScreenView::pauseGame()
{
    presenter->pauseGame();
    application().gotoEndScreenScreenNoTransition();
}

void GameplayScreenView::hideTrajectory()
{
    for (int i = 0; i < TRAJECTORY_DOTS_COUNT; i++) {
        if (trajectoryDots[i].isVisible()) {
            trajectoryDots[i].setVisible(false);
            trajectoryDots[i].invalidate();
        }
    }
}

void GameplayScreenView::updateScore(int newScore)
{
    touchgfx::Unicode::snprintf(txtScoreBuffer, 10, "%d", newScore);
    txtScore.setWildcard(txtScoreBuffer);
    txtScore.invalidate();
}

void GameplayScreenView::updateFullGridUI()
{
    // Bắt buộc invalidate toàn bộ để xóa shadow rác
    std::memset(shadowGrid, SHADOW_GRID_INIT_VAL, sizeof(shadowGrid));
    eggGrid.invalidate();
}

uint16_t GameplayScreenView::getColorBitmap(int color)
{
    switch (color)
    {
        case COLOR_BLUE: return BITMAP_EGG_BLUE_ID;
        case COLOR_GREEN: return BITMAP_EGG_GREEN_ID;
        case COLOR_ORANGE: return BITMAP_EGG_ORANGE_ID;
        case COLOR_PINK: return BITMAP_EGG_PINK_ID;
        case COLOR_PURPLE: return BITMAP_EGG_PURPLE_ID;
        case COLOR_RED: return BITMAP_EGG_RED_ID;
        case COLOR_YELLOW: return BITMAP_EGG_YELLOW_ID;
        default: return BITMAP_EGG_BLUE_ID;
    }
}

void GameplayScreenView::handleClickEvent(const touchgfx::ClickEvent& evt)
{
    GameplayScreenViewBase::handleClickEvent(evt);
    if (evt.getType() == touchgfx::ClickEvent::PRESSED)
    {
        {
            int x = evt.getX(), y = evt.getY();
            if (x >= btnPause.getX() && x <= btnPause.getX() + btnPause.getWidth() &&
                y >= btnPause.getY() && y <= btnPause.getY() + btnPause.getHeight())
                return;
            if (x >= btnQuit.getX() && x <= btnQuit.getX() + btnQuit.getWidth() &&
                y >= btnQuit.getY() && y <= btnQuit.getY() + btnQuit.getHeight())
                return;
        }
        isAiming = true;
        aimX = evt.getX();
        aimY = evt.getY();
        smoothedAimX = aimX;
        smoothedAimY = aimY;
        presenter->handleTouchAim(evt.getX(), evt.getY());
    }
    else if (evt.getType() == touchgfx::ClickEvent::RELEASED)
    {
        if (!isAiming) return;
        isAiming = false;
        // Bắn đạn theo hệ tọa độ đã được làm mượt để đồng bộ với UI nét đứt
        presenter->handleTouchShoot((int)smoothedAimX, (int)smoothedAimY);
    }
}

void GameplayScreenView::handleDragEvent(const touchgfx::DragEvent& evt)
{
    GameplayScreenViewBase::handleDragEvent(evt);
    if (isAiming) {
        aimX = evt.getNewX();
        aimY = evt.getNewY();
    }
    presenter->handleTouchAim(evt.getNewX(), evt.getNewY());
}

void GameplayScreenView::handleTickEvent()
{
    GameplayScreenViewBase::handleTickEvent();

    if (presenter->getGameState() == STATE_GAME_OVER) {
        application().gotoEndScreenScreenNoTransition();
        return;
    }

    presenter->tick();

    // --- Update trajectory ---
    if (isAiming) {
        // Áp dụng Low-Pass Filter để nội suy góc ngắm, loại bỏ hoàn toàn độ nhiễu của Touch (chống Jittering)
        smoothedAimX += (aimX - smoothedAimX) * AIM_SMOOTH_FACTOR;
        smoothedAimY += (aimY - smoothedAimY) * AIM_SMOOTH_FACTOR;
        
        float dx = smoothedAimX - BULLET_START_X;
        float dy = smoothedAimY - BULLET_START_Y;
        if (dy > AIM_MIN_DY) dy = AIM_MIN_DY;
        
        // Reset toàn bộ chấm ảo về trạng thái ẩn
        hideTrajectory();
        
        // Predictive Raycasting
        float simVx, simVy;
        PhysicsEngine::computeVelocity(dx, dy, BULLET_SPEED, simVx, simVy);
        float simX = BULLET_START_X;
        float simY = BULLET_START_Y;
        
        int dotIndex = 0;
        float distTraveled = 0.0f;
        int maxSteps = 1000; // Tránh lặp vô hạn
        
        while (maxSteps-- > 0) {
            PhysicsEngine::advance(simX, simY, simVx, simVy);
            distTraveled += BULLET_SPEED;
            
            // Phản xạ bật tường (chỉnh tâm dội chạm mép theo chuẩn) và chặn dính tường
            PhysicsEngine::reflect(simX, simVx, LEFT_WALL, RIGHT_WALL);
            
            // Dự đoán va chạm: Gọi hàm phi trạng thái từ Model
            if (presenter->isCollisionAt(simX, simY)) {
                break; // Dừng rải nét đứt ngay trước điểm va chạm!
            }
            
            float nextDotDist = TRAJECTORY_DOT_START_DIST + dotIndex * TRAJECTORY_DOT_SPACING;
            if (distTraveled >= nextDotDist && dotIndex < TRAJECTORY_DOTS_COUNT) {
                trajectoryDots[dotIndex].setPosition((int)simX - TRAJECTORY_DOT_OFFSET, (int)simY - TRAJECTORY_DOT_OFFSET, TRAJECTORY_DOT_SIZE, TRAJECTORY_DOT_SIZE);
                trajectoryDots[dotIndex].setVisible(true);
                trajectoryDots[dotIndex].invalidate();
                dotIndex++;
            }
        }
    } else {
        hideTrajectory();
    }

    // --- Update bullet egg ---
    bool bulletVis = presenter->isBulletVisible();
    float bx, by;
    
    if (bulletVis)
    {
        bx = presenter->getBulletX();
        by = presenter->getBulletY();
    }
    else
    {
        // Vị trí cố định trên bệ phóng (tâm quả bóng)
        bx = BULLET_START_X;
        by = BULLET_START_Y;
    }

    int16_t newBulletX = (int16_t)(bx - (EGG_WIDTH / 2.0f));
    int16_t newBulletY = (int16_t)(by - (EGG_HEIGHT / 2.0f));

    int color = presenter->getCurrentColor();
    
    if (newBulletX != lastBulletX || newBulletY != lastBulletY || color != lastColor)
    {
        // Invalidate old position first with expanded rect (clear trail completely)
        if (lastBulletX != -1 && lastBulletY != -1)
        {
            touchgfx::Rect oldR(lastBulletX - INVALIDATE_PADDING, lastBulletY - INVALIDATE_PADDING, INVALIDATE_WIDTH, INVALIDATE_HEIGHT);
            this->invalidateRect(oldR);
        }
        
        uint16_t bmpId = getColorBitmap(color);
        bulletEgg.setBitmap(touchgfx::Bitmap(bmpId));
        
        // Khôi phục kích thước và vị trí của TextureMapper sau khi setBitmap
        bulletEgg.setWidth((int16_t)EGG_WIDTH);
        bulletEgg.setHeight((int16_t)EGG_HEIGHT);
        bulletEgg.setBitmapPosition(0.0f, 0.0f);
        bulletEgg.setOrigo(EGG_WIDTH / 2.0f, EGG_HEIGHT / 2.0f, CAMERA_DISTANCE_Z);
        bulletEgg.setCamera(EGG_WIDTH / 2.0f, EGG_HEIGHT / 2.0f);
        
        bulletEgg.setXY(newBulletX, newBulletY);
        bulletEgg.setVisible(true);
        bulletEgg.invalidate();
        
        // Mở rộng invalidate cho tọa độ mới (đảm bảo phủ viền lúc đập tường)
        touchgfx::Rect newR(newBulletX - INVALIDATE_PADDING, newBulletY - INVALIDATE_PADDING, INVALIDATE_WIDTH, INVALIDATE_HEIGHT);
        this->invalidateRect(newR);
        
        lastBulletX = newBulletX;
        lastBulletY = newBulletY;
        lastColor = color;
    }

    // --- Update dino animation ---
    DinoState dinoState = presenter->getDinoState();
    int nextColor = presenter->getNextColor();
    
    if (dinoState != lastDinoState || nextColor != lastNextColor)
    {
        // Ẩn tất cả dino states
        imgDinoHold.setVisible(false);
        imgDinoThrow.setVisible(false);
        imgDinoDone.setVisible(false);
        
        switch (dinoState)
        {
            case DINO_HOLD:
            {
                // Dino giữ quả bóng next color
                uint16_t dinoBitmap;
                switch (nextColor)
                {
                    case COLOR_BLUE: dinoBitmap = BITMAP_DINO_BLUE_ID; break;
                    case COLOR_GREEN: dinoBitmap = BITMAP_DINO_GREEN_ID; break;
                    case COLOR_ORANGE: dinoBitmap = BITMAP_DINO_ORANGE_ID; break;
                    case COLOR_PINK: dinoBitmap = BITMAP_DINO_PINK_ID; break;
                    case COLOR_PURPLE: dinoBitmap = BITMAP_DINO_PURPLE_ID; break;
                    case COLOR_RED: dinoBitmap = BITMAP_DINO_RED_ID; break;
                    case COLOR_YELLOW: dinoBitmap = BITMAP_DINO_YELLOW_ID; break;
                    default: dinoBitmap = BITMAP_DINO_BLUE_ID; break;
                }
                imgDinoHold.setBitmap(touchgfx::Bitmap(dinoBitmap));
                imgDinoHold.setVisible(true);
                break;
            }
            case DINO_THROW:
                imgDinoThrow.setVisible(true);
                break;
            case DINO_DONE:
                imgDinoDone.setVisible(true);
                break;
        }
        
        imgDinoHold.invalidate();
        imgDinoThrow.invalidate();
        imgDinoDone.invalidate();
        
        lastDinoState = dinoState;
        lastNextColor = nextColor;
    }

    // --- Update egg grid ---
    // Gọi trực tiếp updateGrid, hàm sẽ tự nhận diện Dirty Flag
    updateGrid();
}

void GameplayScreenView::updateGrid()
{
    int newGridY = 22 - (int)GameConstants::CELL_HEIGHT + (int)presenter->getGlobalOffsetY();
    if (eggGrid.getY() != newGridY) {
        eggGrid.setY(newGridY);
        eggGrid.invalidate();
    }

    for (int row = 0; row < MAX_ROWS; row++)
    {
        for (int col = 0; col < MAX_COLS; col++)
        {
            uint8_t color = presenter->getGridCell(row, col);
            if (color != shadowGrid[row][col])
            {
                // Invalidate trạng thái cũ trước khi thay đổi (nếu có)
                if (shadowGrid[row][col] != 0) {
                    eggImages[row][col].invalidate();
                }
                
                shadowGrid[row][col] = color;
                
                if (color != 0)
                {
                    uint16_t bmpId = getColorBitmap(color);
                    eggImages[row][col].setBitmap(touchgfx::Bitmap(bmpId));
                    
                    int parity = presenter->getGridParityOffset();
                    int x = (((row + parity) & 1) == 0) ? UI_CELL_X_EVEN[col] : UI_CELL_X_ODD[col];
                    int y = UI_CELL_Y[row];
                    eggImages[row][col].setXY(x, y);
                    
                    if (!eggImages[row][col].getParent())
                    {
                        eggGrid.add(eggImages[row][col]);
                    }
                    eggImages[row][col].setVisible(true);
                    
                    // Invalidate hiển thị mới
                    eggImages[row][col].invalidate();
                }
                else
                {
                    eggImages[row][col].setVisible(false);
                }
            }
        }
    }
}
