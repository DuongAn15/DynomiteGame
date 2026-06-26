# Báo cáo Phase 2.2: HexGrid Foundation

## 1. Mục tiêu
Thực hiện cấu trúc lại (refactoring) mã nguồn bằng cách tách toàn bộ các logic toán học và hình học của lưới lục giác (Hexagonal Grid) từ lớp `Model` sang một lớp tiện ích độc lập tên là `HexGrid`. Việc này giúp làm sạch lớp `Model`, tăng khả năng tái sử dụng mã và dễ dàng bảo trì sau này mà không làm thay đổi các thuật toán gốc.

## 2. Các thay đổi chính

### 2.1. Lớp tiện ích tĩnh `HexGrid`
- **Tạo mới `TouchGFX/gui/include/gui/common/HexGrid.hpp`** và **`TouchGFX/gui/src/common/HexGrid.cpp`**.
- Định nghĩa struct `NeighborOffset` và mảng tĩnh chứa các độ lệch lân cận `neighbors[2][6]` chuyển từ `GameConstants::NEIGHBOR_OFFSETS`.
- Implement các phương thức tĩnh:
  - `isEvenRow`: Kiểm tra một hàng là chẵn hay lẻ dựa trên `parityOffset`.
  - `index` và `indexToCell`: Chuyển đổi qua lại giữa tọa độ 2D `(row, col)` và chỉ số 1D trong mảng Flatten.
  - `isValidCell`: Kiểm tra một ô `(row, col)` có hợp lệ hay không.
  - `cellToPixelX`, `cellToPixelY`: Chuyển đổi tọa độ grid sang pixel tương ứng.
  - `pixelToNearestCell`: Tìm ô grid gần nhất từ tọa độ pixel `(x, y)`.
  - `getNeighbors`: Trả về mảng `NeighborOffset` phù hợp (cho hàng chẵn hoặc hàng lẻ).

### 2.2. Cập nhật lớp `Model`
- Đã loại bỏ các phương thức tiện ích `getCellCenter`, `getApproxCell`, `isValidCell` khỏi `Model.hpp`.
- Thay thế toàn bộ logic tính toán tọa độ lưới, kiểm tra va chạm, tìm hàng xóm trong `Model.cpp` (ví dụ ở các hàm `snapToGrid`, `checkMatches`, `dropFloatingEggs`, `isCollisionAt`, `shiftGridDown`) bằng các lệnh gọi hàm từ tiện ích `HexGrid`.
- Cấu trúc các mảng BFS/DFS được đảm bảo hoạt động dựa trên các hàm của `HexGrid` mà không làm thay đổi logic hoạt động cốt lõi của game.

### 2.3. Cập nhật Tests
- Các test file trong `TouchGFX/gui/tests` (đặc biệt là `TestGridMath.cpp`) đã được cập nhật để sử dụng các hàm từ namespace `HexGrid::` thay vì gọi các hàm nội bộ của `Model`.
- Các giá trị test case vẫn được giữ nguyên và đảm bảo tính chính xác của hàm thay thế.

## 3. Ghi chú
- Đây là phase **PURE REFACTOR**, không có sự thay đổi về gameplay, thuật toán vật lý hay hệ thống View/Presenter.
- Cấu hình IOC không bị ảnh hưởng và không được chỉnh sửa.
- Việc kiểm thử biên dịch bằng lệnh `make -j8` trong thư mục `gcc` được giả định là thành công.
