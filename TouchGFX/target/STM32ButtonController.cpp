#include "STM32ButtonController.hpp"
#include "stm32f4xx_hal.h"

void STM32ButtonController::init()
{
}

bool STM32ButtonController::sample(uint8_t& key)
{
    uint8_t currentKey = 0;

    // LƯU Ý: Đã gỡ bỏ cấu hình đọc GPIO sai (PC3, PA1, PA2, PA3) vì chúng xung đột với LTDC và FMC.
    // Nếu có shield gắn ngoài, hãy cấu hình đúng chân và ghi vào đây.
    // Tạm thời trả về 0 (không có phím cứng) để dùng Touch.
    currentKey = 0;

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
