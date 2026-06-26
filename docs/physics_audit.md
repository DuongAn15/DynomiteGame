# Báo cáo Audit Cấu trúc Vật lý (Physics Audit) - Phase 2.3

## 1. Các công thức vật lý bị trùng lặp (Duplicated Formulas)

### Formula 1: Tính toán Vận tốc (Compute Velocity)
- **Source file:** `TouchGFX/gui/src/model/Model.cpp`
- **Function:** `handleTouchShoot()`
- **Lines:** 139-143
- **Copied formula:**
  ```cpp
  float length = sqrtf(dx*dx + dy*dy);
  float speed = BULLET_SPEED;
  bullet.vx = (dx / length) * speed;
  bullet.vy = (dy / length) * speed;
  ```
- **Purpose:** Chuẩn hóa vector hướng ngắm (dx, dy) và nhân với tốc độ đạn để phân rã ra vận tốc theo trục X và Y.
- **Duplicated in:** `TouchGFX/gui/src/gameplayscreen_screen/GameplayScreenView.cpp`
  - **Function:** `handleTickEvent()`
  - **Lines:** 159-166

### Formula 2: Di chuyển vị trí (Advance Position)
- **Source file:** `TouchGFX/gui/src/model/Model.cpp`
- **Function:** `updateFlyingPhysics()`
- **Lines:** 159-160
- **Copied formula:**
  ```cpp
  bullet.x += bullet.vx;
  bullet.y += bullet.vy;
  ```
- **Purpose:** Tích phân vận tốc theo khung hình (Frame) để cập nhật vị trí đạn.
- **Duplicated in:** `TouchGFX/gui/src/gameplayscreen_screen/GameplayScreenView.cpp`
  - **Function:** `handleTickEvent()`
  - **Lines:** 175-176 (`simX += simVx; simY += simVy;`)

### Formula 3: Phản xạ tường (Wall Reflection / Bounce)
- **Source file:** `TouchGFX/gui/src/model/Model.cpp`
- **Function:** `updateFlyingPhysics()`
- **Lines:** 163-170
- **Copied formula:**
  ```cpp
  if (bullet.x <= LEFT_WALL && bullet.vx < 0) {
      bullet.x = LEFT_WALL + (LEFT_WALL - bullet.x);
      bullet.vx = -bullet.vx;
  }
  else if (bullet.x >= RIGHT_WALL && bullet.vx > 0) {
      bullet.x = RIGHT_WALL - (bullet.x - RIGHT_WALL);
      bullet.vx = -bullet.vx;
  }
  ```
- **Purpose:** Xử lý va chạm biên (nảy lại), đảo ngược Vector X và đẩy vị trí đạn ra ngoài để tránh hiện tượng dính tường (penetration).
- **Duplicated in:** `TouchGFX/gui/src/gameplayscreen_screen/GameplayScreenView.cpp`
  - **Function:** `handleTickEvent()`
  - **Lines:** 180-186

---

## 2. Bảng Duplication (Duplication Table)

| Formula | Model.cpp | GameplayScreenView.cpp | Other files | Can move to PhysicsEngine? | Reason |
| --- | --- | --- | --- | --- | --- |
| Compute Velocity | `handleTouchShoot()` | `handleTickEvent()` | Không | Yes | Hàm toán học thuần túy nội suy vx, vy từ dx, dy. |
| Advance Position | `updateFlyingPhysics()` | `handleTickEvent()` | Không | Yes | Phép cộng tọa độ cơ bản, không tạo side-effect (side-effects-free). |
| Wall Reflection | `updateFlyingPhysics()` | `handleTickEvent()` | Không | Yes | Thuần túy kiểm tra biên và đảo dấu vector vx. |

---

## 3. Các hằng số Vật lý (Physics-related Constants)

| Tên hằng số | Vị trí hiện tại | Nơi sử dụng | Should move to PhysicsEngine? | Justification (Lý do) |
| --- | --- | --- | --- | --- |
| `BULLET_SPEED` | `GameConstants.hpp` | `Model`, `View` | **No** | Tốc độ là thông số cấu hình Gameplay (gameplay tuning), có thể thay đổi/nâng cấp qua các Level. `PhysicsEngine` chỉ nhận speed làm tham số đầu vào. |
| `LEFT_WALL`, `RIGHT_WALL` | `GameConstants.hpp` | `Model`, `View`, `Tests` | **No** | Vị trí tường là thuộc tính kích thước không gian Grid/UI của Game. `PhysicsEngine` phải mang tính tổng quát và nhận bounds làm tham số. |
| `TOP_WALL` | `GameConstants.hpp` | - | **No** | Tương tự tường trái/phải. |
| `AIM_SMOOTH_FACTOR`, `AIM_MIN_DY` | `GameConstants.hpp` | `Model`, `View` | **No** | Cấu hình Input của UI (tránh giật lag cảm ứng màn hình). Không thuộc về Physics logic. |
| `HITBOX_RADIUS`, `HITBOX_WALL_DIST` | `GameConstants.hpp` | `Model` | **No** | Thuộc về cấu hình kích thước nhân vật/quả trứng. |

---

## 4. Proposed PhysicsEngine API

Toàn bộ các hàm sau đều nằm trong `TouchGFX/gui/include/gui/common/PhysicsEngine.hpp`.

**Function 1:**
- **Signature:** `static void computeVelocity(float dx, float dy, float speed, float& vx, float& vy);`
- **Inputs:** `dx`, `dy`, `speed`
- **Outputs:** `vx`, `vy` (qua tham chiếu)
- **Pure function?** Yes
- **Used by:** `Model::handleTouchShoot`, `GameplayScreenView::handleTickEvent`

**Function 2:**
- **Signature:** `static void advance(float& x, float& y, float vx, float vy);`
- **Inputs:** `x`, `y` (tham chiếu), `vx`, `vy`
- **Outputs:** Cập nhật lại giá trị cho `x`, `y`
- **Pure function?** Yes
- **Used by:** `Model::updateFlyingPhysics`, `GameplayScreenView::handleTickEvent`

**Function 3:**
- **Signature:** `static void reflect(float& x, float& vx, float leftWall, float rightWall);`
- **Inputs:** `x`, `vx` (tham chiếu), `leftWall`, `rightWall`
- **Outputs:** Cập nhật lại giá trị cho `x`, `vx` nếu có va chạm
- **Pure function?** Yes
- **Used by:** `Model::updateFlyingPhysics`, `GameplayScreenView::handleTickEvent`

---

## 5. Repository-wide verification (Xác minh toàn mã nguồn)

Thông qua lệnh `grep_search`, toán học vật lý được xác nhận chỉ tồn tại ở các khu vực sau:
- **Model:** `TouchGFX/gui/src/model/Model.cpp`
- **View:** `TouchGFX/gui/src/gameplayscreen_screen/GameplayScreenView.cpp`
- **Tests:** `TestCollision.cpp` (chỉ gán giá trị `m.bullet.x = GameConstants::LEFT_WALL; m.bullet.vx = -5.0f;` để chạy test, không tự thực hiện phép toán vật lý).
- **Presenter:** Hoàn toàn KHÔNG chứa toán học vật lý.
- **Common utilities:** KHÔNG.
- **Generated code:** KHÔNG.

---

## 6. Risk analysis (Phân tích Rủi ro)

- **Vì sao Gameplay behavior không thay đổi?** Cấu trúc trạng thái biến (`bullet.x`, `simX`) vẫn do `Model` và `View` làm chủ. `PhysicsEngine` chỉ đơn thuần đóng gói cụm lệnh `A + B` thành `add(A, B)`.
- **Vì sao kết quả Floating-point không đổi?** Các phép toán số học dấu phẩy động (cộng dồn `x += vx`, căn bậc hai `sqrtf`, nhân hệ số) được chép y nguyên (bit-identical). Không có sự thay đổi về trình tự thực thi (Order of Operations) dẫn đến sai số (Precision drift) khác so với bản gốc.
- **Thứ tự Function call có thay đổi không?** Không. Lệnh gọi `PhysicsEngine::advance` sẽ được chèn đúng vào dòng code hiện tại của phép `x += vx`.
- **Điều kiện Branch (rẽ nhánh) có đổi không?** Không. Câu lệnh `if (bullet.x <= LEFT_WALL && bullet.vx < 0)` được chuyển vào hàm `reflect()` mà không bị thay đổi bất kỳ logic điều kiện nào. Hành vi của nhánh vẫn giữ nguyên 100%.

**TRẠNG THÁI HIỆN TẠI: CHƯA SỬA ĐỔI SOURCE CODE. ĐANG ĐỢI USER APPROVAL CHO STEP 2.**
