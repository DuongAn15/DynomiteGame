import os
import librosa
import numpy as np

raw_dir = "Audio_Asssets/Raw"
out_dir = "Core/Inc"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

for file in os.listdir(raw_dir):
    if file.endswith((".wav", ".mp3", ".ogg", ".flac")):
        file_path = os.path.join(raw_dir, file)
        # librosa.load returns float32 array in range [-1.0, 1.0]
        data, sr = librosa.load(file_path, sr=16000, mono=True)
        
        name = os.path.splitext(file)[0]
        name_lower = name.lower()
        if 'bgm' in name_lower:
            data = data[:int(8 * 16000)]
        elif 'sfx' in name_lower:
            data = data[:int(1.5 * 16000)]
            
        # Convert to uint16_t using given formula
        data_uint16 = (data * 32767).astype(np.uint16)
        
        out_file = os.path.join(out_dir, f"audio_{name}.h")
        with open(out_file, "w") as f:
            f.write(f"#ifndef AUDIO_{name.upper()}_H\n")
            f.write(f"#define AUDIO_{name.upper()}_H\n\n")
            f.write("#include <stdint.h>\n\n")
            f.write(f"const uint16_t audio_{name}[{len(data_uint16)}] = {{\n")
            
            hex_data = [f"0x{d:04x}" for d in data_uint16]
            for i in range(0, len(hex_data), 10):
                f.write("    " + ", ".join(hex_data[i:i+10]) + ",\n")
            
            f.write("};\n\n")
            f.write(f"const uint32_t audio_{name}_length = {len(data_uint16)};\n\n")
            f.write(f"#endif // AUDIO_{name.upper()}_H\n")

print("Generated audio arrays successfully.")
