#include <gui/endscreen_screen/EndScreenView.hpp>

EndScreenView::EndScreenView()
{

}

void EndScreenView::setupScreen()
{
    EndScreenViewBase::setupScreen();
    txtScore.setWildcard(txtScoreBuffer);
    txtHighScore.setWildcard(txtHighScoreBuffer);
}

void EndScreenView::tearDownScreen()
{
    EndScreenViewBase::tearDownScreen();
}

void EndScreenView::setScores(int score, int high)
{
    touchgfx::Unicode::snprintf(txtScoreBuffer, TXT_BUFFER_SIZE, "%d", score);
    touchgfx::Unicode::snprintf(txtHighScoreBuffer, TXT_BUFFER_SIZE, "%d", high);
    txtScore.invalidate();
    txtHighScore.invalidate();
}
