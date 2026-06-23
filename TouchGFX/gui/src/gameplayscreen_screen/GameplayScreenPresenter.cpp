#include <gui/gameplayscreen_screen/GameplayScreenView.hpp>
#include <gui/gameplayscreen_screen/GameplayScreenPresenter.hpp>

GameplayScreenPresenter::GameplayScreenPresenter(GameplayScreenView& v)
    : view(v)
{

}

void GameplayScreenPresenter::activate()
{

}

void GameplayScreenPresenter::deactivate()
{

}

void GameplayScreenPresenter::handleTouchAim(int x, int y)
{
    model->handleTouchAim(x, y);
}

void GameplayScreenPresenter::handleTouchShoot(int x, int y)
{
    model->handleTouchShoot(x, y);
}

uint8_t GameplayScreenPresenter::getGridCell(int row, int col) const
{
    return model->getGridCell(row, col);
}
