import os
import librosa
import numpy as np

def main():
    raw_dir = 'Audio_Asssets/Raw'
    out_dir = 'Core/Inc'
    
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if not os.path.exists(raw_dir):
        print(f"Directory {raw_dir} does not exist.")
        return

    for filename in os.listdir(raw_dir):
        file_path = os.path.join(raw_dir, filename)
        if not os.path.isfile(file_path):
            continue
            
        print(f"Processing {filename}...")
        try:
            data, sr = librosa.load(file_path, sr=16000, mono=True)
            
            # Limit length
            if 'bgm' in filename.lower():
                max_samples = 8 * 16000
                data = data[:max_samples]
            elif 'sfx' in filename.lower():
                max_samples = int(1.5 * 16000)
                data = data[:max_samples]
                
            # Convert to 16-bit integers
            data_int16 = np.int16(data * 32767)
            # Convert to unsigned 16-bit for C array
            data_uint16 = data_int16.astype(np.uint16)
            
            # Generate .h filename
            base_name = os.path.splitext(filename)[0]
            h_filename = f"audio_{base_name}.h"
            h_path = os.path.join(out_dir, h_filename)
            
            array_name = f"audio_{base_name}"
            
            with open(h_path, 'w') as f:
                f.write(f"// Auto-generated from {filename}\n")
                f.write(f"#ifndef AUDIO_{base_name.upper()}_H\n")
                f.write(f"#define AUDIO_{base_name.upper()}_H\n\n")
                f.write(f"#include <stdint.h>\n\n")
                f.write(f"const uint16_t {array_name}[] = {{\n")
                
                # Write data in chunks to avoid extremely long lines
                for i in range(0, len(data_uint16), 12):
                    chunk = data_uint16[i:i+12]
                    f.write("    " + ", ".join([f"0x{val:04X}" for val in chunk]) + ",\n")
                    
                f.write(f"}};\n\n")
                f.write(f"const uint32_t {array_name}_length = {len(data_uint16)};\n\n")
                f.write(f"#endif // AUDIO_{base_name.upper()}_H\n")
                
            print(f"Saved {h_path}")
        except Exception as e:
            print(f"Error processing {filename}: {e}")

if __name__ == '__main__':
    main()
