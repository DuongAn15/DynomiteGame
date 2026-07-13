#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <stdint.h>

class AudioManager {
public:
    static void init();
    static void playBGM(const uint8_t* audio, uint32_t length);
    static void playSFX(const uint16_t* audio, uint32_t length);
    static void stopBGM();
};

#endif // AUDIOMANAGER_HPP
