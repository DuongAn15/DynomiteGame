import sys
import wave
import struct
import os

def wav_to_c_array(wav_file_path, h_file_path, array_name):
    try:
        with wave.open(wav_file_path, 'rb') as wav_file:
            num_channels = wav_file.getnchannels()
            sample_width = wav_file.getsampwidth()
            frame_rate = wav_file.getframerate()
            num_frames = wav_file.getnframes()
            
            print(f"Channels: {num_channels}, Sample Width: {sample_width}, Frame Rate: {frame_rate}, Frames: {num_frames}")
            
            if sample_width != 2:
                print("Error: Only 16-bit WAV files are supported.")
                return

            frames = wav_file.readframes(num_frames)
            
            # Unpack the binary data into integers (little-endian 16-bit)
            samples = struct.unpack(f"<{num_frames * num_channels}h", frames)
            
            with open(h_file_path, 'w') as h_file:
                h_file.write(f"/* Generated from {os.path.basename(wav_file_path)} */\n")
                h_file.write(f"#ifndef {array_name.upper()}_H\n")
                h_file.write(f"#define {array_name.upper()}_H\n\n")
                h_file.write("#include <stdint.h>\n\n")
                h_file.write(f"const uint32_t {array_name}_sample_rate = {frame_rate};\n")
                h_file.write(f"const uint32_t {array_name}_length = {len(samples)};\n\n")
                h_file.write(f"const uint16_t {array_name}[] = {{\n    ")
                
                # Convert samples to uint16_t (0-65535 instead of -32768 to 32767)
                for i, sample in enumerate(samples):
                    unsigned_sample = sample + 32768 if sample < 0 else sample
                    unsigned_sample = unsigned_sample & 0xFFFF
                    
                    h_file.write(f"0x{unsigned_sample:04X}")
                    if i < len(samples) - 1:
                        h_file.write(", ")
                    
                    if (i + 1) % 12 == 0:
                        h_file.write("\n    ")
                
                h_file.write("\n};\n\n")
                h_file.write(f"#endif /* {array_name.upper()}_H */\n")
                
        print(f"Successfully converted {wav_file_path} to {h_file_path}")

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: python wav2c.py <input.wav> <output.h> <array_name>")
        sys.exit(1)
        
    wav_to_c_array(sys.argv[1], sys.argv[2], sys.argv[3])
