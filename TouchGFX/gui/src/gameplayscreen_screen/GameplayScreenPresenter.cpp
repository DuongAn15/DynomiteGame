#include <gui/gameplayscreen_screen/GameplayScreenView.hpp>
#include <gui/gameplayscreen_screen/GameplayScreenPresenter.hpp>

GameplayScreenPresenter::GameplayScreenPresenter(GameplayScreenView& v)
    : view(v)
{

}

void GameplayScreenPresenter::activate()
{
    if (model->getGameState() == STATE_GAME_OVER) {
        model->startNewGame();
    }
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

void GameplayScreenPresenter::tick()
{
    model->tick();
}

float GameplayScreenPresenter::getBulletX() const
{
    return model->getBulletX();
}

float GameplayScreenPresenter::getBulletY() const
{
    return model->getBulletY();
}

int GameplayScreenPresenter::getCurrentColor() const
{
    return model->getCurrentColor();
}

int GameplayScreenPresenter::getNextColor() const
{
    return model->getNextColor();
}

DinoState GameplayScreenPresenter::getDinoState() const
{
    return model->getDinoState();
}

bool GameplayScreenPresenter::isBulletVisible() const
{
    return model->isBulletVisible();
}

GameState GameplayScreenPresenter::getGameState() const
{
    return model->getGameState();
}

bool GameplayScreenPresenter::isCollisionAt(float x, float y) const
{
    return model->isCollisionAt(x, y);
}

float GameplayScreenPresenter::getGlobalOffsetY() const
{
    return model->getGlobalOffsetY();
}

int GameplayScreenPresenter::getGridParityOffset() const
{
    return model->getGridParityOffset();
}

void GameplayScreenPresenter::notifyGameOver()
{
    // Báo cho UI chuyển sang màn hình GameOver (nếu có)
    // Hiện tại có thể chưa có màn hình GameOverScreen, ta cứ báo cho View xử lý
    // hoặc tạm thời bỏ qua nếu View chưa implement chuyển màn
    // (View sẽ tự đọc gameState và hiện text GameOver)
}

void GameplayScreenPresenter::notifyScoreUpdated(int newScore)
{
    view.updateScore(newScore);
}

void GameplayScreenPresenter::notifyGridShifted()
{
    view.updateFullGridUI();
}
