#include <gui/common/AudioManager.hpp>
#include "main.h"

// Các file mảng âm thanh nằm trong Core/Inc nên ta include trực tiếp bằng nháy kép vì compiler đã có path này:
#include "audio_bgm_gameplay.h"
#include "audio_bgm_menu.h"
#include "audio_sfx_button.h"
#include "audio_sfx_drop.h"
#include "audio_sfx_gameover.h"
#include "audio_sfx_hit.h"
#include "audio_sfx_match.h"
#include "audio_sfx_shoot.h"

extern I2S_HandleTypeDef hi2s3;

void AudioManager::playBGM(const uint16_t* audio, uint32_t length) {
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio, length);
}
void AudioManager::playSFX(const uint16_t* audio, uint32_t length) {
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)audio, length);
}
