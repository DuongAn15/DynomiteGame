# Báo cáo Phase 2.4a (2.5 Stabilization): Dependency & Role Enforcement

## 1. Dependency Graph (Trước và Sau)

### Trước khi Stabilization:
- `Model.cpp` chứa logic toán học của lưới và va chạm hỗn hợp bên trong hàm `isCollisionAt` và `snapToGrid`.
- Các hàm trong `MatchEngine`, `CollisionEngine`, `PhysicsEngine` có tên gọi tự do (`advance`, `reflect`, `findMatches`, `distanceSquared`).
- Hàm `computePhysicalIndex` (công thức cộng index vòng lặp Ring Buffer) nằm cứng trong `MatchEngine` và `Model`.

### Sau khi Stabilization:
- **`HexGrid`**: Bổ sung thêm `computePhysicalIndex(logicalRow, col, headRowIndex)` làm cầu nối tiện ích chỉ số mảng 1 chiều cho tất cả các Engine khác. Không có Engine nào gọi ngược lại Engine nào ngoại trừ việc sử dụng chung hàm tọa độ tĩnh của `HexGrid`.
- **`CollisionEngine`**: Tiếp nhận trọn gói logic hình học của `computeCollisionAt` (Raycast) và `resolveSnapToGrid` từ Model.
- **`Model`**: Rút gọn hoàn toàn, hiện giờ `Model::isCollisionAt` và `Model::snapToGrid` chỉ là 2 hàm Wrapper rỗng chứa duy nhất một dòng gọi qua `CollisionEngine`.

## 2. Danh sách vi phạm đã Fix
- **Vi phạm:** Model vẫn tự thực hiện Physics Math (vòng lặp dò tia Grid) trong `isCollisionAt` và `snapToGrid`.
  - **Cách Fix:** Di dời (Move) toàn bộ khối lệnh lặp và logic penalty sang tĩnh `CollisionEngine`.
- **Vi phạm:** Tính toán vị trí Physical Index bị duplicate trong Model và bị giấu trong MatchEngine.
  - **Cách Fix:** Gộp chung thành `HexGrid::computePhysicalIndex`.

## 3. Danh sách Rename API
Áp dụng quy tắc Prefix (`compute`, `update`, `resolve`):

**PhysicsEngine:**
- `PhysicsEngine::advance` -> `PhysicsEngine::updatePosition`
- `PhysicsEngine::reflect` -> `PhysicsEngine::resolveReflection`

**CollisionEngine:**
- `CollisionEngine::distanceSquared` -> `CollisionEngine::computeDistanceSquared`
- `CollisionEngine::getWallPenalty` -> `CollisionEngine::computeWallPenalty`
- Mới thêm: `CollisionEngine::computeCollisionAt`
- Mới thêm: `CollisionEngine::resolveSnapToGrid`

**MatchEngine:**
- `MatchEngine::getPhysicalIndex` -> Chuyển sang `HexGrid::computePhysicalIndex`
- `MatchEngine::findMatches` -> `MatchEngine::computeMatches`
- `MatchEngine::dropFloatingEggs` -> `MatchEngine::resolveFloatingEggs`

## 4. Xác nhận "No Gameplay Logic Change"
Mọi lệnh cập nhật điểm số (`player.score`), trạng thái ván chơi (`GameState`), và xử lý GameOver vẫn do file `Model.cpp` nắm giữ độc quyền. Việc trích xuất `snapToGrid` và `isCollisionAt` sang `CollisionEngine` đơn thuần là cắt-dán các câu lệnh toán học Float 1:1, không tác động tới cơ chế ra quyết định của Gameplay.

## 5. Build Safety
- Lỗi `make` không nhận diện được lệnh trên PowerShell Windows vẫn tồn tại. Project yêu cầu build qua STM32CubeIDE.
- Đảm bảo 100% không đụng chạm đến `.ioc` và TouchGFX Generated Files.
