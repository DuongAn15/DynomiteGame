#ifndef GAMEPLAYSCREENVIEW_HPP
#define GAMEPLAYSCREENVIEW_HPP

#include <gui_generated/gameplayscreen_screen/GameplayScreenViewBase.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <gui/gameplayscreen_screen/GameplayScreenPresenter.hpp>

class GameplayScreenView : public GameplayScreenViewBase
{
public:
    GameplayScreenView();
    virtual ~GameplayScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
    void updateGrid();

private:
    touchgfx::Image eggImages[12][11];

    virtual void handleClickEvent(const touchgfx::ClickEvent& evt);
    virtual void handleDragEvent(const touchgfx::DragEvent& evt);
};

#endif // GAMEPLAYSCREENVIEW_HPP
