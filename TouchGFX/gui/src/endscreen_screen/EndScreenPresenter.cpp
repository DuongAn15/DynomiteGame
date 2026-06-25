#include <gui/endscreen_screen/EndScreenView.hpp>
#include <gui/endscreen_screen/EndScreenPresenter.hpp>

EndScreenPresenter::EndScreenPresenter(EndScreenView& v)
    : view(v)
{

}

void EndScreenPresenter::activate()
{
    view.setScores(model->getScore(), model->getHighScore());
}

void EndScreenPresenter::deactivate()
{

}
