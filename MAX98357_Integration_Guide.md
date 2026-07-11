# Hướng dẫn Tích hợp Mô-đun Âm thanh MAX98357

Tài liệu này cung cấp hướng dẫn chi tiết về cách kết nối và sử dụng mô-đun khuếch đại âm thanh I2S MAX98357 với kit phát triển STM32F429I-DISCO.

## 1. Sơ đồ Chân Kết nối

Bảng dưới đây mô tả cách kết nối các chân của mô-đun MAX98357 với vi điều khiển STM32F429I-DISCO, sử dụng bộ ngoại vi **I2S2**.

| Chân trên MAX98357 | Chân trên STM32F429I-DISCO | Chức năng (I2S2) | Ghi chú |
| :--- | :--- | :--- | :--- |
| **LRC** (Left/Right Clock) | **PB12** | I2S2_WS | Word Select (chuyển kênh trái/phải) |
| **BCLK** (Bit Clock) | **PB13** | I2S2_CK | Xung nhịp đồng bộ bit |
| **DIN** (Data In) | **PB15** | I2S2_SD | Dữ liệu âm thanh nối tiếp |
| **VIN** / **VCC** | **5V** | Nguồn cấp | Cấp nguồn 5V từ kit |
| **GND** | **GND** | Nối đất | Nối chung mass hệ thống |

> **Lưu ý:** Các cấu hình I2S trên STM32 cần được thiết lập trùng khớp với chuẩn giao tiếp của MAX98357 (thường là Philips I2S standard, 16-bit hoặc 32-bit data).

## 2. Ý tưởng Nạp Nhạc và Phát với "Zero CPU Overhead"

Để phát âm thanh mà không làm ảnh hưởng đến các tác vụ khác của vi điều khiển (đặc biệt là logic game), chúng ta áp dụng phương pháp phát trực tiếp từ bộ nhớ Flash thông qua **I2S + DMA**:

1. **Lưu trữ tĩnh:** File âm thanh (.wav) được chuyển đổi trước thành một mảng hằng số (hằng số mảng byte/word trong C) và biên dịch trực tiếp vào bộ nhớ Flash của vi điều khiển.
2. **Cấu hình DMA:** Cấu hình bộ điều khiển DMA (Direct Memory Access) để tự động đọc dữ liệu từ mảng âm thanh trong Flash và ghi thẳng vào thanh ghi dữ liệu của bộ I2S.
3. **Phát âm thanh liên tục:** Khi DMA được kích hoạt, nó sẽ liên tục bơm dữ liệu sang I2S một cách độc lập. Vi xử lý (CPU) không cần can thiệp vào quá trình truyền từng byte/word.
4. **Zero CPU Overhead:** Nhờ DMA, CPU được hoàn toàn giải phóng để thực hiện các công việc khác như xử lý đồ họa, tính toán va chạm hay xử lý nút bấm trong game. CPU chỉ cần cấp lệnh "Bắt đầu" và nhận ngắt khi "Hoàn thành".

## 3. Hướng dẫn Sử dụng Script `wav2c.py`

Vì STM32 không thể đọc trực tiếp định dạng file `.wav` từ máy tính, ta cần sử dụng script `wav2c.py` để chuyển đổi file âm thanh sang định dạng mảng hex C (thường là mảng số nguyên 16-bit).

### Ý nghĩa của Script:
- Đọc file `.wav` (tốt nhất là mono, sample rate phù hợp với cấu hình I2S, ví dụ: 8kHz, 16kHz hoặc 44.1kHz).
- Bỏ qua các header của định dạng WAV.
- Lấy dữ liệu PCM (Pulse-Code Modulation) thô và xuất ra dưới dạng văn bản (text) là một mảng hằng số C, cho phép dán trực tiếp vào file `.c` hoặc `.h`.

### Ví dụ Cú pháp Lệnh:

```bash
python wav2c.py <input_file.wav> <output_file.c> <array_name>
```

**Ví dụ thực tế:**
Nếu bạn có một file nhạc nền tên là `bgm.wav` và muốn tạo một mảng tên là `audio_bgm` trong file `bgm_data.c`, bạn chạy lệnh:

```bash
python wav2c.py bgm.wav bgm_data.c audio_bgm
```

Sau khi chạy, file `bgm_data.c` sẽ chứa một cấu trúc dạng:
```c
const uint16_t audio_bgm[] = {
    0x0000, 0x012A, 0x025F, ...
};
const uint32_t audio_bgm_length = 45000; // Số lượng phần tử
```
Bạn chỉ cần include dữ liệu này vào chương trình chính để DMA sử dụng.
