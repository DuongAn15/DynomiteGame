#include <gui/common/AudioManager.hpp>
#include "main.h"

// Các file mảng âm thanh nằm trong Core/Inc nên ta include trực tiếp bằng nháy kép vì compiler đã có path này:
#include "audio_game_play.h"
#include "audio_bgm_menu.h"
#include "audio_sfx_button.h"
#include "audio_sfx_drop.h"
#include "audio_sfx_gameover.h"
#include "audio_sfx_hit.h"
#include "audio_sfx_match.h"
#include "audio_sfx_shoot.h"

extern I2S_HandleTypeDef hi2s3;

#define AUDIO_BUFFER_SIZE 4096
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

struct AudioChannel {
    const uint16_t* data;
    uint32_t length;
    uint32_t position;
    bool active;
    bool looping;
};

static AudioChannel bgmChannel = {nullptr, 0, 0, false, true};
static AudioChannel sfxChannels[3] = {
    {nullptr, 0, 0, false, false},
    {nullptr, 0, 0, false, false},
    {nullptr, 0, 0, false, false}
};

static bool initialized = false;

void AudioManager::init() {
    if (!initialized) {
        for (int i = 0; i < AUDIO_BUFFER_SIZE; ++i) {
            audio_buffer[i] = 0;
        }
        HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio_buffer, AUDIO_BUFFER_SIZE);
        initialized = true;
    }
}

void AudioManager::playBGM(const uint16_t* audio, uint32_t length) {
    if (!initialized) {
        init();
    }
    bgmChannel.data = audio;
    bgmChannel.length = length;
    bgmChannel.position = 0;
    bgmChannel.active = true;
    bgmChannel.looping = true;
}

void AudioManager::stopBGM() {
    HAL_I2S_DMAStop(&hi2s3);
    bgmChannel.active = false;
    initialized = false;
}

void AudioManager::playSFX(const uint16_t* audio, uint32_t length) {
    if (!initialized) {
        init();
    }
    for (int i = 0; i < 3; ++i) {
        if (!sfxChannels[i].active) {
            sfxChannels[i].data = audio;
            sfxChannels[i].length = length;
            sfxChannels[i].position = 0;
            sfxChannels[i].active = true;
            sfxChannels[i].looping = false;
            return;
        }
    }
    // Override first SFX channel if all full
    sfxChannels[0].data = audio;
    sfxChannels[0].length = length;
    sfxChannels[0].position = 0;
    sfxChannels[0].active = true;
    sfxChannels[0].looping = false;
}

static void fillAudioBuffer(uint16_t* buffer, uint32_t size) {
    for (uint32_t i = 0; i < size; ++i) {
        int32_t mixed_sample = 0;
        
        if (bgmChannel.active && bgmChannel.data) {
            int16_t sample = (int16_t)bgmChannel.data[bgmChannel.position];
            mixed_sample += sample;
            bgmChannel.position++;
            if (bgmChannel.position >= bgmChannel.length) {
                if (bgmChannel.looping) {
                    bgmChannel.position = 0;
                } else {
                    bgmChannel.active = false;
                }
            }
        }
        
        for (int ch = 0; ch < 3; ++ch) {
            if (sfxChannels[ch].active && sfxChannels[ch].data) {
                int16_t sample = (int16_t)sfxChannels[ch].data[sfxChannels[ch].position];
                mixed_sample += sample;
                sfxChannels[ch].position++;
                if (sfxChannels[ch].position >= sfxChannels[ch].length) {
                    sfxChannels[ch].active = false;
                }
            }
        }
        
        if (mixed_sample > 32767) mixed_sample = 32767;
        else if (mixed_sample < -32768) mixed_sample = -32768;
        
        buffer[i] = (uint16_t)(int16_t)mixed_sample;
    }
}

extern "C" void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI3) {
        fillAudioBuffer(&audio_buffer[0], AUDIO_BUFFER_SIZE / 2);
    }
}

extern "C" void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
    if (hi2s->Instance == SPI3) {
        fillAudioBuffer(&audio_buffer[AUDIO_BUFFER_SIZE / 2], AUDIO_BUFFER_SIZE / 2);
    }
}
