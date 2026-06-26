#ifndef STM32BUTTONCONTROLLER_HPP
#define STM32BUTTONCONTROLLER_HPP

#include <platform/driver/button/ButtonController.hpp>

enum class ButtonKeycode : uint8_t { Left = 1, Right = 2, Fire = 3, Swap = 4 };

class STM32ButtonController : public touchgfx::ButtonController
{
public:
    virtual void init();
    virtual bool sample(uint8_t& key);

    uint8_t previousStateKey = 0;
    uint8_t lockedKey = 0;
    uint8_t counter = 0;
    static const uint8_t DEBOUNCE_THRESHOLD = 3;
};

#endif // STM32BUTTONCONTROLLER_HPP
