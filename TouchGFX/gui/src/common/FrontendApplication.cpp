#include <gui/common/FrontendApplication.hpp>

FrontendApplication::FrontendApplication(Model& m, FrontendHeap& heap)
    : FrontendApplicationBase(m, heap)
{

}

#include <gui/common/FrontendHeap.hpp>
#include <gui/endscreen_screen/EndScreenView.hpp>
#include <gui/endscreen_screen/EndScreenPresenter.hpp>
#include <touchgfx/transitions/NoTransition.hpp>

void FrontendApplication::gotoEndScreenScreenNoTransition()
{
    customTransitionCallback = touchgfx::Callback<FrontendApplication>(this, &FrontendApplication::gotoEndScreenScreenNoTransitionImpl);
    pendingScreenTransitionCallback = &customTransitionCallback;
}

void FrontendApplication::gotoEndScreenScreenNoTransitionImpl()
{
    touchgfx::makeTransition<EndScreenView, EndScreenPresenter, touchgfx::NoTransition, Model >(&currentScreen, &currentPresenter, frontendHeap, &currentTransition, &model);
}
