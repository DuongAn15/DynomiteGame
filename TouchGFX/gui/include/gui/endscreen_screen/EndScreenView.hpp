#ifndef ENDSCREENVIEW_HPP
#define ENDSCREENVIEW_HPP

#include <gui_generated/endscreen_screen/EndScreenViewBase.hpp>
#include <gui/endscreen_screen/EndScreenPresenter.hpp>

class EndScreenView : public EndScreenViewBase
{
public:
    EndScreenView();
    virtual ~EndScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    void setScores(int score, int high);
protected:
    static const uint16_t TXT_BUFFER_SIZE = 10;
    touchgfx::Unicode::UnicodeChar txtScoreBuffer[TXT_BUFFER_SIZE];
    touchgfx::Unicode::UnicodeChar txtHighScoreBuffer[TXT_BUFFER_SIZE];
};

#endif // ENDSCREENVIEW_HPP
