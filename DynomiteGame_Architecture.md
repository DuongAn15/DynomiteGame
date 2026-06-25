# Báo Cáo Kiến Trúc & Tối Ưu Hóa - DynomiteGame (STM32 + TouchGFX)

Tài liệu này tổng hợp các phân tích, thuật toán và giải pháp kỹ thuật tối ưu nhất để triển khai logic game bắn trứng (Dynomite/Bust-A-Move) trên nền tảng vi điều khiển STM32F429I-DISCO.

---

## 1. Tổng Quan Hệ Thống

- **Nền tảng Hardware:** ARM Cortex-M4 (STM32F429), màn hình LCD 240x320 (Portrait), cảm ứng điện dung.
- **Graphic Stack:** TouchGFX + FreeRTOS + DMA2D (Chrom-ART Accelerator) + LTDC.
- **Kiến trúc UI:** Theo chuẩn Model-View-Presenter (MVP). Có 3 màn hình:
  - `HomeScreen`: Màn hình chờ.
  - `GameplayScreen`: Màn hình chơi game chính.
  - `EndScreen`: Màn hình kết thúc, lưu điểm.

---

## 2. Nén Dữ Liệu "Zero CPU Overhead" (Phần Cứng & Hình Ảnh)

Để tối ưu hóa dung lượng Flash/RAM mà không làm giảm tốc độ tính toán của CPU (Zero CPU overhead), dự án áp dụng chiến lược tận dụng phần cứng DMA2D:

- **Định dạng ảnh L8 (Color Palette):** Nén ảnh xuống còn 1 byte/pixel (giảm 75% dung lượng so với ARGB8888).
- **Cơ chế DMA2D:** DMA2D có tích hợp sẵn bảng màu (CLUT). Khi vẽ, phần cứng DMA2D sẽ tự động tra bảng màu và blend pixel xuất ra màn hình. CPU hoàn toàn rảnh rỗi 100% trong quá trình này.
- **Khuyến nghị áp dụng:** 
  - Đổi định dạng các sprite nhỏ (Trứng, Khủng long, Nút bấm) sang **L8_ARGB8888**.
  - Tuyệt đối **KHÔNG** bật nén bằng phần mềm (`l8_compression: "no"`) như L4, RLE, hay LZW9 vì CPU sẽ phải tốn chu kỳ máy để giải nén.
  - Giữ ảnh nền lớn (`Background 240x320`) ở định dạng **RGB565**.

---

## 3. Cấu Trúc Dữ Liệu Logic Game (Core Game)

Trong lập trình nhúng, việc cố gắng nén bit (bit-packing) sẽ cắn lẹm chu kỳ CPU (do phép dịch bit và che bit). Ưu tiên tốc độ CPU thay vì tiết kiệm một vài byte RAM cực nhỏ.

### 3.1. Lưới Lục Giác (Hexagonal Grid)
Sử dụng mảng 1 chiều phẳng (Flatten Array) nguyên mẫu byte (`uint8_t`) để đạt tốc độ truy xuất O(1) với đúng 1 chu kỳ máy.
```cpp
#define MAX_ROWS 12
#define MAX_COLS 11
#define EMPTY_COLOR 0

// Lưới 132 ô (chỉ tốn 132 Bytes RAM)
uint8_t grid[MAX_ROWS * MAX_COLS]; 
```

**Offset Tìm Láng Giềng (Neighbor Offsets):**
Do tính chất xếp so le của lưới lục giác, láng giềng phụ thuộc vào việc ô đang ở hàng chẵn hay lẻ:
```cpp
// Offset {dcol, drow}
const int EVEN_NEIGHBORS[6][2] = {{-1,-1}, { 0,-1}, {-1, 0}, { 1, 0}, {-1, 1}, { 0, 1}};
const int ODD_NEIGHBORS[6][2]  = {{ 0,-1}, { 1,-1}, {-1, 0}, { 1, 0}, { 0, 1}, { 1, 1}};
```

### 3.2. Tọa độ và Vật lý
STM32F429 sở hữu phần cứng FPU (Floating Point Unit), nên việc tính toán số thực diễn ra cực nhanh (1-3 chu kỳ).
- **Tọa độ:** Sử dụng kiểu `float` cho vị trí và vận tốc (vx, vy).
- **Tính vận tốc ban đầu:**
  ```cpp
  vx = speed * cos(aimAngle);
  vy = -speed * sin(aimAngle);
  ```

---

## 4. Thuật Toán Cốt Lõi Game Bắn Trứng

### 4.1. Cơ Chế Va Chạm & Di Chuyển (Game Loop `Model::tick()`)
1. **Di chuyển:** `bulletX += vx; bulletY += vy;`
2. **Đập tường (Wall bounce):** Phản xạ hoàn hảo.
   ```cpp
   if (đụng_tường_trái || đụng_tường_phải) { vx = -vx; }
   ```
3. **Va chạm trứng (Circle Collision):**
   Nếu khoảng cách tâm `(bullet, egg) < 2 * EGG_RADIUS`, xảy ra va chạm.
4. **Hút vào lưới (Snap-to-Grid):**
   Tìm ô trống (`EMPTY_COLOR`) nằm gần nhất tọa độ của quả trứng vừa va chạm. Khóa (snap) quả trứng vào tọa độ của ô lưới đó.

### 4.2. Tìm & Phá Trứng Cùng Màu (BFS - Flood Fill)
Khi một quả trứng gắn vào lưới, kiểm tra màu các quả láng giềng.
- **Sử dụng:** Breadth-First Search (BFS) kết hợp với Queue tĩnh (Mảng) để tránh cấp phát động `malloc`.
- **Logic:** Duyệt mảng `visited`, nếu đếm được tổng số trứng kết nối cùng màu `>= 3`, tiến hành xóa toàn bộ.

### 4.3. Loại Bỏ Trứng Lơ Lửng (DFS - Floater Detection)
Xảy ra ngay sau khi xóa cụm trứng. Một số quả có thể bị rụng khỏi "trần nhà".
- **Sử dụng:** Depth-First Search (DFS) bắt đầu từ hàng trên cùng (Row 0).
- **Logic:** Quét tất cả các quả trứng, đánh dấu `connected = true` cho bất kỳ quả trứng nào dính với Row 0.
- Những quả trứng không được đánh dấu sẽ bị rơi (chuyển màu về 0 và kích hoạt animation rơi).

---

## 5. Tích Hợp Ngoại Vi Hardware (Zero CPU Overhead)

### 5.1. 4 Nút Bấm Vật Lý (Trái, Phải, Bắn, Swap)
Tránh sử dụng ngắt ngoài (EXTI) để tránh dội bom ngắt (interrupt storm) do nhiễu cơ học (bouncing) làm tốn CPU.
- **Giải pháp:** Kế thừa interface `touchgfx::ButtonController`.
- **Hoạt động:** Hàm `sample()` sẽ đọc trực tiếp trạng thái GPIO. Do TouchGFX Engine tự động gọi hàm này đồng bộ với tần số quét UI (60Hz = 16.6ms), khoảng thời gian này là hoàn hảo để khử nhiễu (debounce) bằng phần mềm mà không tốn tài nguyên. Tín hiệu phím sau đó sẽ được tự động chuyển thành `handleKeyEvent` và đẩy thẳng vào file View.

### 5.2. Loa MAX98357 (Âm thanh I2S)
MAX98357 là mạch amply Class D nhận tín hiệu kỹ thuật số I2S. Tuyệt đối không dùng CPU để đẩy tuần tự từng mẫu âm thanh (sample) vào I2S bằng vòng lặp (vô cùng ngốn CPU).
- **Giải pháp:** Sử dụng **I2S kết hợp bộ nhớ truy cập trực tiếp DMA**.
- **Hoạt động:** Lưu trữ các âm thanh sfx (bắn trứng, vỡ trứng, nhạc nền) dưới dạng mảng C tĩnh (`const uint16_t`) trực tiếp trên bộ nhớ Flash (định dạng PCM không nén để tránh phải decode). Khởi tạo DMA đọc mảng đó nhét thẳng vào I2S. Xuyên suốt quá trình phát tiếng, DMA làm việc 100% độc lập, CPU hoàn toàn không phải tham gia. Với nhạc nền dài, cấu hình DMA ở chế độ Ping-Pong (Circular) buffer.

---

## 6. Tổng Kết

Bằng việc kết hợp kiến trúc phần cứng DMA2D (Zero CPU image rendering) và các thuật toán Mảng phẳng (Zero CPU bit-shifting), game sẽ chạy cực kỳ mượt mà ở 60 FPS trên bộ nhớ và chip khiêm tốn của STM32, dồn toàn bộ sức mạnh cho logic game (Physics và Matching algorithms).
