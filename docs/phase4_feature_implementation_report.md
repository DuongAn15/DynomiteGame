# Báo cáo Phase 4: Feature Implementation (Production Mode)

## 1. Các tính năng đã triển khai (Features Implemented)

- **Combo System:** Thêm bộ đếm `combo` và `multiplier` vào `PlayerData`. Hệ thống ghi nhận số lần ăn điểm liên tiếp, hệ số nhân từ x1 đến x4 (Capped). Combo sẽ reset về 0 nếu người chơi bắn trượt (Snap thất bại) hoặc viên đạn bám vào lưới nhưng không tạo ra Match.
- **Scoring Expansion:** Tích hợp hệ số nhân `multiplier` vào công thức cộng điểm. Khi có Match 3, 4, 5+ hoặc khi có chùm trứng rơi (Drop), điểm số nhận được sẽ nhân với hệ số combo hiện tại, trong khi vẫn bảo toàn các hằng số điểm gốc (`SCORE_MATCH_3`, v.v.).
- **Game Difficulty Scaling:** Cấp độ khó (`difficultyLevel`) tự động tăng theo mốc 2000 điểm. Giá trị này được cộng trực tiếp vào `idx` khi tra cứu bảng `SPAWN_INTERVAL_LUT`, giúp lưới trứng rớt xuống nhanh dần khi người chơi đạt điểm càng cao.
- **Special Events Hook System:** Định nghĩa và kích hoạt hệ thống sự kiện UI:
  - `notifyOnShoot()`: Kích hoạt khi người chơi bắn trứng.
  - `notifyOnMatch(matchCount, combo)`: Kích hoạt khi có trứng nổ, gửi kèm số trứng và số combo.
  - `notifyOnDrop(dropCount)`: Kích hoạt khi có trứng rơi lơ lửng.

## 2. Các tệp đã sửa đổi (Files Modified)
- `TouchGFX/gui/include/gui/model/Model.hpp`: Thêm `combo`, `multiplier` vào `PlayerData`; thêm `difficultyLevel`.
- `TouchGFX/gui/src/model/Model.cpp`: Chèn logic cộng/reset combo, cập nhật điểm, tính toán độ khó dựa trên điểm số, gọi các Hooks.
- `TouchGFX/gui/include/gui/model/ModelListener.hpp`: Khai báo 3 virtual methods mới cho Hook System.

## 3. Event Flow Description
Mô tả luồng tương tác thực tế (Smooth Integration):
1. **Input:** Nút bắn được nhấn `handleTouchShoot` -> kích hoạt `notifyOnShoot()`.
2. **Physics/Collision:** Đạn bay, bật tường, và bám vào lưới. Nếu không Match -> Reset Combo & Multiplier về mặc định (0 và 1).
3. **Match & Score:** Thuật toán đếm số bóng. Nếu có Match -> Tăng Combo -> Tính hệ số nhân x1~x4 -> Cộng điểm gốc nhân hệ số -> Kích hoạt `notifyOnMatch(count, combo)` -> Cập nhật `notifyScoreUpdated`.
4. **Drop:** Thuật toán duyệt bóng rớt -> Cộng điểm bóng rớt nhân hệ số -> Kích hoạt `notifyOnDrop(count)` -> Cập nhật UI Score.

## 4. Xác nhận ổn định hệ thống (Stability & Rules Confirmation)
- **No Architecture Changes:** Cấu trúc 4 lớp (Model, Mapper, HexGrid, Engines) hoàn toàn giữ nguyên bản. Mọi tính năng bổ sung đều nằm bên trong lớp `Model.cpp`, đúng vai trò quản lý State và Orchestrator của nó.
- **STM32 Safe:** Không có lớp Engine mới, không cấp phát bộ nhớ động (`new`/`malloc`), không làm thay đổi các mảng BFS tĩnh (`algoQueueStack`). Gameplay duy trì tính độc lập khung hình (Deterministic).
- Dự án sẵn sàng tích hợp thẳng vào GUI (TouchGFX UI) vì API đã hoàn thiện.
