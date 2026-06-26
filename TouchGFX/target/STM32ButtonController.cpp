#include "STM32ButtonController.hpp"
#include "stm32f4xx_hal.h"

void STM32ButtonController::init()
{
}

bool STM32ButtonController::sample(uint8_t& key)
{
    uint8_t currentKey = 0;

    if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3) == GPIO_PIN_RESET) {
        currentKey = static_cast<uint8_t>(ButtonKeycode::Left);
    } else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET) {
        currentKey = static_cast<uint8_t>(ButtonKeycode::Right);
    } else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET) {
        currentKey = static_cast<uint8_t>(ButtonKeycode::Fire);
    } else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET) {
        currentKey = static_cast<uint8_t>(ButtonKeycode::Swap);
    }

    if (currentKey == 0)
    {
        lockedKey = 0;
        counter = 0;
        previousStateKey = 0;
        return false;
    }

    if (currentKey == lockedKey)
    {
        return false; // Wait for release
    }

    if (currentKey == previousStateKey)
    {
        counter++;
        if (counter >= DEBOUNCE_THRESHOLD)
        {
            key = currentKey;
            lockedKey = currentKey;
            counter = 0;
            return true;
        }
    }
    else
    {
        previousStateKey = currentKey;
        counter = 1;
    }

    return false;
}
