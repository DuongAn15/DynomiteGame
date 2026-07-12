import sys
import wave
import os

step_table = [
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
]

index_table = [
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
]

class AdpcmState:
    def __init__(self):
        self.predictor = 0
        self.step_index = 0

def encode_sample(sample, state):
    diff = sample - state.predictor
    nibble = 0
    if diff < 0:
        nibble = 8
        diff = -diff
    
    step = step_table[state.step_index]
    vpdiff = step >> 3
    
    if diff >= step:
        nibble |= 4
        diff -= step
        vpdiff += step
        
    step >>= 1
    if diff >= step:
        nibble |= 2
        diff -= step
        vpdiff += step
        
    step >>= 1
    if diff >= step:
        nibble |= 1
        vpdiff += step

    if nibble & 8:
        state.predictor -= vpdiff
    else:
        state.predictor += vpdiff

    if state.predictor > 32767:
        state.predictor = 32767
    elif state.predictor < -32768:
        state.predictor = -32768

    state.step_index += index_table[nibble]
    if state.step_index < 0:
        state.step_index = 0
    elif state.step_index > 88:
        state.step_index = 88

    return nibble

def process(input_wav, output_h):
    with wave.open(input_wav, 'rb') as w:
        if w.getsampwidth() != 2:
            print("Wav must be 16-bit")
            return
        nchannels = w.getnchannels()
        framerate = w.getframerate()
        frames = w.readframes(w.getnframes())

    raw_samples = []
    for i in range(0, len(frames), 2):
        s = frames[i] | (frames[i+1] << 8)
        if s > 32767:
            s -= 65536
        raw_samples.append(s)

    if nchannels == 2:
        mono_samples = []
        for i in range(0, len(raw_samples) - 1, 2):
            mono_samples.append((raw_samples[i] + raw_samples[i+1]) // 2)
        raw_samples = mono_samples
    elif nchannels > 2:
        print("Only mono or stereo wav files are supported")
        return

    if framerate != 16000:
        samples = []
        dst_len = int(len(raw_samples) * 16000 / framerate)
        for i in range(dst_len):
            src_index = int(i * framerate / 16000)
            if src_index >= len(raw_samples):
                src_index = len(raw_samples) - 1
            samples.append(raw_samples[src_index])
    else:
        samples = raw_samples

    if len(samples) > 32000:
        samples = samples[:-32000]


    state = AdpcmState()
    adpcm_data = []
    for i in range(0, len(samples), 2):
        s1 = samples[i]
        n1 = encode_sample(s1, state)
        
        n2 = 0
        if i + 1 < len(samples):
            s2 = samples[i+1]
            n2 = encode_sample(s2, state)
            
        byte = (n2 << 4) | n1
        adpcm_data.append(byte)

    array_name = os.path.splitext(os.path.basename(output_h))[0]
    
    with open(output_h, 'w') as f:
        f.write(f'#ifndef {array_name.upper()}_H\n')
        f.write(f'#define {array_name.upper()}_H\n\n')
        f.write(f'#include <stdint.h>\n\n')
        f.write(f'const uint32_t {array_name}_length = {len(adpcm_data)};\n')
        f.write(f'const uint8_t {array_name}[{len(adpcm_data)}] = {{\n')
        
        for i in range(0, len(adpcm_data), 12):
            chunk = adpcm_data[i:i+12]
            f.write('    ' + ', '.join([f'0x{b:02x}' for b in chunk]) + ',\n')
            
        f.write('};\n\n')
        f.write(f'#endif\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: python wav2adpcm.py input.wav output.h")
        sys.exit(1)
    process(sys.argv[1], sys.argv[2])
