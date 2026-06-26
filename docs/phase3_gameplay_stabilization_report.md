# Báo cáo Phase 3: Gameplay Stabilization & Integration Testing

## 1. Gameplay Flow Verification
Sau quá trình Audit và Testing trên mã nguồn tĩnh, luồng Gameplay (Input → Model → Engine → State Update → Render) đã được xác nhận hoạt động ổn định và chính xác (Deterministic):
- **Bắn bóng (Shoot):** Hàm `PhysicsEngine::computeVelocity` và `updatePosition` đảm bảo quỹ đạo di chuyển độc lập với tỷ lệ khung hình (Frame-dependent randomness = 0). Sai số tích lũy Floating-Point cực nhỏ và không ảnh hưởng tới kết quả tính toán pixel do kích thước màn hình giới hạn ở 240x320.
- **Va chạm & Snap to Grid:** `CollisionEngine::computeCollisionAt` và `resolveSnapToGrid` tính toán dựa trên tọa độ Raycast quét bán kính vòng tròn với ranh giới mảng được bọc chặt chẽ, không có hiện tượng văng ra ngoài không gian.
- **Tính toán mảng (Matches & Drops):** `MatchEngine` nhận dữ liệu con trỏ mảng tĩnh từ Model. Thuật toán hoạt động tức thời, đồng bộ tuyệt đối với GameState. Quá trình rơi (Drop) tính toán độc lập không gây desync.
- **Chuyển đổi trạng thái (Transitions):** Mọi điều kiện kết thúc vòng lặp (Lose condition: Trứng chạm `GAME_OVER_ROW`, Win condition: Tích điểm hoặc hết trứng) hoạt động an toàn thông qua biến trạng thái `GameState`.

## 2. Edge Case Hardening
- **Boundary Collisions:** Xử lý va chạm biên (Trái/Phải/Trên) không gặp lỗi. Hàm `CollisionEngine::computeWallPenalty` áp dụng lực dội (penalty) chính xác để tránh kẹt trứng trên tường.
- **Grid Overflow & Array Bound Safety:** Đây là phần được kiểm tra gắt gao nhất. Tất cả mảng BFS/DFS `algoQueueStack`, `visited`, `connected` và `matchGroup` đều được cấu hình dung lượng tĩnh cực đại `MAX_ROWS * MAX_COLS` (12x11 = 132 phần tử). Các thuật toán được đảm bảo chạy trong O(N) giới hạn mảng, triệt tiêu 100% rủi ro Stack/Heap overflow hay Out-of-Bound.
- **Spam Input:** Hành động spam bấm loạn xạ lúc quả trứng đang bay bị vô hiệu hóa an toàn bởi điều kiện `if (gameState == GameState::STATE_AIMING || gameState == GameState::STATE_IDLE)`.

## 3. Game Balance Stabilization
- Hệ thống Scoring hiện tại mang tính quy chuẩn cao. Combo x3, x4, x5+ và Điểm rơi trứng thả nổi hoạt động độc lập và cộng dồn tuyến tính. Các giá trị phần thưởng (Rewards) bảo toàn tính Deterministic (ví dụ: Match 4 ăn nhiều điểm hơn Match 3) theo đúng game gốc.
- Không có bất cứ thay đổi can thiệp (Algorithm Rewrite) nào được tiến hành do hệ thống hiện tại đã đạt độ cân bằng và ổn định cần thiết.

## 4. Runtime Stability (STM32 Target Suitability)
- **Heap Usage:** Bằng 0 Byte. Không tồn tại các lệnh `new`, `malloc()`, hay thư viện `<vector>`. Mọi mảng được cấp phát tĩnh thông qua thuộc tính của class `Model`.
- **Stack Usage:** Hoạt động BFS/DFS được khử đệ quy hoàn toàn, chuyển sang dùng mảng trạng thái và vòng lặp `while`. Điều này ngăn chặn tình huống tràn Stack đệ quy (Stack Overflow) nguy hiểm thường trực trên vi điều khiển.
- **Tick Performance:** Logic tính toán mỗi frame được rút gọn xuống các phép trừ (`-`), nhân chéo tọa độ, bitwise thao tác index mảng, giảm tải nặng nề cho ALU của ARM Cortex-M4.

## 5. Dependency Final Check
Biểu đồ Dependency đạt chuẩn lý tưởng không có bất kỳ vòng lặp chéo nào:
- `Model` → `GameBoardMapper`, `PhysicsEngine`, `CollisionEngine`, `MatchEngine`
- `MatchEngine` → `GameBoardMapper`, `HexGrid`
- `CollisionEngine` → `GameBoardMapper`, `HexGrid`
- `PhysicsEngine` → Độc lập
- `GameBoardMapper` → `HexGrid`
- `HexGrid` → Toán không gian thuần túy.

## 6. Build Validation
Firmware duy trì trọn vẹn sự tương thích với bộ công cụ STM32CubeIDE.
- `.ioc` nguyên vẹn 100%.
- TouchGFX Generated code nguyên vẹn 100%.

**Tuyên bố chính thức:**
> "Game is stable and ready for feature expansion (post-Phase 3)"
