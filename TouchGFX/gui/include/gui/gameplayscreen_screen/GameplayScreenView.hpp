#ifndef GAMEPLAYSCREENVIEW_HPP
#define GAMEPLAYSCREENVIEW_HPP

#include <gui_generated/gameplayscreen_screen/GameplayScreenViewBase.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <gui/gameplayscreen_screen/GameplayScreenPresenter.hpp>

#include <gui/common/GameConstants.hpp>

class GameplayScreenView : public GameplayScreenViewBase
{
public:
    GameplayScreenView();
    virtual ~GameplayScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    
    void hideTrajectory();
    
    void updateScore(int newScore);
    void updateFullGridUI();
    
protected:
    void updateGrid();

private:
    touchgfx::Unicode::UnicodeChar txtScoreBuffer[10];
    touchgfx::Image eggImages[GameConstants::MAX_ROWS][GameConstants::MAX_COLS];

    uint8_t shadowGrid[GameConstants::MAX_ROWS][GameConstants::MAX_COLS];

    int16_t lastBulletX;
    int16_t lastBulletY;
    int lastDinoState;
    int lastNextColor;
    int lastColor;

    touchgfx::Box trajectoryDots[GameConstants::TRAJECTORY_DOTS_COUNT];
    int aimX;
    int aimY;
    float smoothedAimX;
    float smoothedAimY;
    bool isAiming;

    uint16_t getColorBitmap(int color);

    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);
    virtual void handleDragEvent(const touchgfx::DragEvent& evt);
    virtual void handleTickEvent();
};

#endif // GAMEPLAYSCREENVIEW_HPP
