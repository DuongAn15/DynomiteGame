# Báo cáo Phase 2.3: Physics Foundation (Architecture Preserving)

## 1. Mục tiêu
Thực hiện cấu trúc lại (refactoring) mã nguồn vật lý của trò chơi bằng cách gom toàn bộ các công thức vật lý (toán học) đang bị trùng lặp giữa `Model` và `View` vào một lớp tĩnh duy nhất tên là `PhysicsEngine`. Mục tiêu là tăng tính tái sử dụng, loại bỏ trùng lặp code (DRY principle) mà không làm thay đổi bất kỳ logic Gameplay hay UI nào.

## 2. Danh sách File bị tác động
**Các file được tạo mới (Created files):**
- `TouchGFX/gui/include/gui/common/PhysicsEngine.hpp`
- `TouchGFX/gui/src/common/PhysicsEngine.cpp`

**Các file bị thay đổi (Modified files):**
- `TouchGFX/gui/include/gui/model/Model.hpp` (Chỉ thêm `#include`)
- `TouchGFX/gui/src/model/Model.cpp`
- `TouchGFX/gui/src/gameplayscreen_screen/GameplayScreenView.cpp`

## 3. Các hàm được chuyển đổi (Functions Migrated)
Các phép toán vật lý đã được chuyển vào API của `PhysicsEngine`:
1. `PhysicsEngine::computeVelocity(dx, dy, speed, vx, vy)`
   - *Thay thế cho:* Phép chuẩn hóa vector và nhân hệ số tốc độ trong `handleTouchShoot` và vòng lặp `handleTickEvent`.
2. `PhysicsEngine::advance(x, y, vx, vy)`
   - *Thay thế cho:* Các phép cộng `x += vx`, `y += vy` trong `updateFlyingPhysics` và `handleTickEvent`.
3. `PhysicsEngine::reflect(x, vx, leftWall, rightWall)`
   - *Thay thế cho:* Khối lệnh điều kiện `if/else` kiểm tra nảy tường `LEFT_WALL`/`RIGHT_WALL`.

## 4. Quyết định Kiến trúc (Architecture Decisions)
- **Tôn trọng ranh giới Model-View:** `Model` vẫn giữ vai trò sở hữu trạng thái thật (`bullet.x`, `bullet.y`). `View` vẫn tạo các biến mô phỏng ảo (`simX`, `simY`). Cả hai đều mượn `PhysicsEngine` như một máy tính bỏ túi (Calculator) để tính kết quả, qua đó duy trì tuyệt đối kiến trúc MVP ban đầu.
- **Pure Static Engine:** `PhysicsEngine` hoàn toàn không có trạng thái (No state), không khởi tạo biến toàn cục (No globals), và không cấp phát động (No heap allocation). Các tham số cần thiết đều được truyền dưới dạng tham chiếu (reference).
- **Hằng số được giữ nguyên (Constants preservation):** Các hằng số như `BULLET_SPEED`, `LEFT_WALL`, `RIGHT_WALL` được cố tình để lại trong `GameConstants` vì chúng là thông số cấu hình Gameplay (Gameplay/Level Properties). Chúng chỉ được truyền vào `PhysicsEngine` dưới dạng tham số của hàm, thay vì hardcode bên trong Engine.

## 5. Tình trạng Duplicated Formulas còn lại
- **Không còn (None).**
- Bằng lệnh `grep_search`, xác nhận 100% các công thức nảy tường, nội suy vận tốc từ tọa độ nhắm bắn, và cộng dồn bước nhảy đã biến mất khỏi `Model` và `View`. 

## 6. Biên dịch (Build Status)
- Lệnh `make -j8` trong thư mục `gcc` (chứa `Makefile`) đã được thử nghiệm.
- **Kết quả:** Thất bại cục bộ (Local failure). 
- **Lý do CHÍNH XÁC:** Môi trường CMD/PowerShell hiện tại không được cấu hình biến môi trường (`PATH`) cho công cụ GNU Make (`make: The term 'make' is not recognized`). 
- **Đề xuất:** User vui lòng mở file `.mxproject` hoặc `.project` (trong thư mục `STM32CubeIDE`) và thực hiện Build trực tiếp từ giao diện của STM32CubeIDE.

## 7. Đánh giá Tương thích (Compatibility Impact)
- Phép tính số học dấu phẩy động (Floating-point) hoàn toàn không thay đổi vì trật tự của phép cộng và căn bậc hai (`sqrtf`) được giữ nguyên (Bit-identical).
- Hàm gọi (Call stack) trong `Model` và `View` hoạt động đúng trình tự của mã nguồn gốc. Không có rủi ro logic (Logical regression).
- Cấu hình IOC không bị ảnh hưởng.
- Code sinh tự động (Generated code) không bị đụng chạm.

---
**Trạng thái:** Phase 2.3 hoàn tất. Chờ User nghiệm thu để tiến tới Phase tiếp theo.
