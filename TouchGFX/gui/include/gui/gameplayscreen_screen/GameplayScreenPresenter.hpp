#ifndef GAMEPLAYSCREENPRESENTER_HPP
#define GAMEPLAYSCREENPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameplayScreenView;

class GameplayScreenPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GameplayScreenPresenter(GameplayScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~GameplayScreenPresenter() {}

    void handleTouchAim(int x, int y);
    void handleTouchShoot(int x, int y);
    void handleSwapColor();
    void pauseGame();
    void tick();

    uint8_t getGridCell(int row, int col) const;
    float getBulletX() const;
    float getBulletY() const;
    int getCurrentColor() const;
    int getNextColor() const;
    DinoState getDinoState() const;
    GameState getGameState() const;
    bool isBulletVisible() const;
    bool isCollisionAt(float x, float y) const;
    float getGlobalOffsetY() const;
    int getGridParityOffset() const;
    void getGridData(uint8_t* out) const;
    
    // ModelListener overrides
    void notifyGameOver() override;
    void notifyScoreUpdated(int newScore) override;
    void notifyGridShifted() override;

private:
    GameplayScreenPresenter();

    GameplayScreenView& view;
};

#endif // GAMEPLAYSCREENPRESENTER_HPP
