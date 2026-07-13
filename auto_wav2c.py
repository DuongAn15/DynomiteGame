import os
import librosa
import numpy as np

def main():
    raw_dir = 'Audio_Assets'
    out_dir = 'Core/Inc'
    if not os.path.exists(out_dir): os.makedirs(out_dir)

    for filename in os.listdir(raw_dir):
        file_path = os.path.join(raw_dir, filename)
        if not os.path.isfile(file_path): continue
            
        print(f"Processing {filename}...")
        try:
            data, sr = librosa.load(file_path, sr=16000, mono=True)
            data, _ = librosa.effects.trim(data, top_db=30)
            
            name_lower = filename.lower()
            target_duration = 1.0 # default
            if 'shoot' in name_lower or 'button' in name_lower: target_duration = 0.15
            elif 'hit' in name_lower: target_duration = 0.20
            elif 'match' in name_lower: target_duration = 0.40
            elif 'drop' in name_lower: target_duration = 0.60
            elif 'gameover' in name_lower: target_duration = 4.0
            elif 'bgm_menu' in name_lower: target_duration = 5.0
            elif 'bgm_gameplay' in name_lower: target_duration = 10.0
                
            max_samples = int(target_duration * 16000)
            if len(data) > max_samples:
                data = data[:max_samples]
                
            fade_samples = int(len(data) * 0.1)
            if fade_samples > 0:
                fade_curve = np.linspace(1.0, 0.0, fade_samples)
                data[-fade_samples:] *= fade_curve
                
            data = np.clip(data, -1.0, 1.0)
            data_int16 = np.int16(data * 32767)
            data_int16 = np.repeat(data_int16, 2)
            data_uint16 = data_int16.astype(np.uint16)
            
            base_name = os.path.splitext(filename)[0]
            h_path = os.path.join(out_dir, f"audio_{base_name}.h")
            array_name = f"audio_{base_name}"
            
            with open(h_path, 'w') as f:
                f.write(f"// Auto-generated from {filename}\n")
                f.write(f"// Duration: {target_duration}s, Trimmed, Faded\n")
                f.write(f"#ifndef AUDIO_{base_name.upper()}_H\n#define AUDIO_{base_name.upper()}_H\n\n#include <stdint.h>\n\n")
                f.write(f"const uint16_t {array_name}[] = {{\n")
                for i in range(0, len(data_uint16), 12):
                    chunk = data_uint16[i:i+12]
                    f.write("    " + ", ".join([f"0x{val:04X}" for val in chunk]) + ",\n")
                f.write(f"}};\n\nconst uint32_t {array_name}_length = {len(data_uint16)};\n\n#endif\n")
                
            print(f"Saved {h_path} ({len(data_uint16)} samples)")
        except Exception as e:
            print(f"Error: {e}")

if __name__ == '__main__': main()
