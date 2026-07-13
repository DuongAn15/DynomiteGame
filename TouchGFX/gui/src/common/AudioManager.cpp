#include <gui/common/AudioManager.hpp>
#include "main.h"
#include <string.h>

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

struct BgmChannel {
    const uint8_t* data;
    uint32_t length;
    uint32_t position; // byte index
    bool active;
    bool looping;
    int16_t predictor;
    int8_t step_index;
    bool is_nibble_high;
};

struct SfxChannel {
    const uint16_t* data;
    uint32_t length;
    uint32_t position;
    bool active;
    bool looping;
};

static BgmChannel bgmChannel = {nullptr, 0, 0, false, true, 0, 0, false};
static SfxChannel sfxChannels[MAX_SFX_CHANNELS] = {
    {nullptr, 0, 0, false, false},
    {nullptr, 0, 0, false, false},
    {nullptr, 0, 0, false, false}
};

static const int16_t step_table[89] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static const int8_t index_table[16] = {
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static int16_t adpcm_decode(uint8_t nibble, int16_t& predictor, int8_t& step_index) {
    int16_t step = step_table[step_index];
    int16_t vpdiff = step >> 3;

    if (nibble & 4) vpdiff += step;
    if (nibble & 2) vpdiff += (step >> 1);
    if (nibble & 1) vpdiff += (step >> 2);

    if (nibble & 8) {
        predictor -= vpdiff;
    } else {
        predictor += vpdiff;
    }

    if (predictor > 32767) predictor = 32767;
    else if (predictor < -32768) predictor = -32768;

    step_index += index_table[nibble];
    if (step_index < 0) step_index = 0;
    else if (step_index > 88) step_index = 88;

    return predictor;
}

static bool initialized = false;

void AudioManager::init() {
    if (!initialized) {
        memset(audio_buffer, 0, sizeof(audio_buffer));
        HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio_buffer, AUDIO_BUFFER_SIZE);
        initialized = true;
    }
}

void AudioManager::playBGM(const uint8_t* audio, uint32_t length) {
    if (!initialized) {
        init();
    }
    bgmChannel.data = audio;
    bgmChannel.length = length;
    bgmChannel.position = 0;
    bgmChannel.predictor = 0;
    bgmChannel.step_index = 0;
    bgmChannel.is_nibble_high = false;
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
    for (int i = 0; i < MAX_SFX_CHANNELS; ++i) {
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
    for (uint32_t i = 0; i < size; i += 2) {
        int32_t mixed_sample = 0;
        
        if (bgmChannel.active && bgmChannel.data) {
            uint8_t byte = bgmChannel.data[bgmChannel.position];
            uint8_t nibble = bgmChannel.is_nibble_high ? (byte >> 4) : (byte & 0x0F);
            
            int16_t sample = adpcm_decode(nibble, bgmChannel.predictor, bgmChannel.step_index);
            mixed_sample += sample;
            
            if (bgmChannel.is_nibble_high) {
                bgmChannel.position++;
                bgmChannel.is_nibble_high = false;
            } else {
                bgmChannel.is_nibble_high = true;
            }
            
            if (bgmChannel.position >= bgmChannel.length) {
                if (bgmChannel.looping) {
                    bgmChannel.position = 0;
                    bgmChannel.predictor = 0;
                    bgmChannel.step_index = 0;
                    bgmChannel.is_nibble_high = false;
                } else {
                    bgmChannel.active = false;
                }
            }
        }
        
        for (int ch = 0; ch < MAX_SFX_CHANNELS; ++ch) {
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
        if (i + 1 < size) {
            buffer[i + 1] = (uint16_t)(int16_t)mixed_sample;
        }
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
