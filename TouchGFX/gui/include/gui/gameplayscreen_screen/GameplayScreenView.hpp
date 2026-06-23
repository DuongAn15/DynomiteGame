#ifndef GAMEPLAYSCREENVIEW_HPP
#define GAMEPLAYSCREENVIEW_HPP

#include <gui_generated/gameplayscreen_screen/GameplayScreenViewBase.hpp>
#include <gui/gameplayscreen_screen/GameplayScreenPresenter.hpp>

class GameplayScreenView : public GameplayScreenViewBase
{
public:
    GameplayScreenView();
    virtual ~GameplayScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // GAMEPLAYSCREENVIEW_HPP
