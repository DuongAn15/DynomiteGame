#include <gui/homescreen_screen/HomeScreenView.hpp>
#include "stm32f4xx_hal.h"

HomeScreenView::HomeScreenView()
{
    prevShoot = false;
}

void HomeScreenView::setupScreen()
{
    HomeScreenViewBase::setupScreen();
}

void HomeScreenView::tearDownScreen()
{
    HomeScreenViewBase::tearDownScreen();
}

void HomeScreenView::handleTickEvent()
{
    HomeScreenViewBase::handleTickEvent();

    bool isShoot = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_RESET);
    
    if (isShoot && !prevShoot) {
        application().gotoGameplayScreenScreenCoverTransitionEast();
    }
    prevShoot = isShoot;
}
