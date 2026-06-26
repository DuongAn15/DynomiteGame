#ifndef FRONTENDAPPLICATION_HPP
#define FRONTENDAPPLICATION_HPP

#include <gui_generated/common/FrontendApplicationBase.hpp>

class FrontendHeap;

using namespace touchgfx;

class FrontendApplication : public FrontendApplicationBase
{
public:
    FrontendApplication(Model& m, FrontendHeap& heap);
    virtual ~FrontendApplication() { }

    virtual void handleTickEvent()
    {
        FrontendApplicationBase::handleTickEvent();
    }

    void gotoEndScreenScreenNoTransition();
    void gotoEndScreenScreenNoTransitionImpl();

private:
    touchgfx::Callback<FrontendApplication> customTransitionCallback;
};

#endif // FRONTENDAPPLICATION_HPP
