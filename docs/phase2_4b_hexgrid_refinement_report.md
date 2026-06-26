# Báo cáo Phase 2.4B: HexGrid Role Refinement

## 1. Vi phạm phân quyền của HexGrid
Trước Phase 2.4B, `HexGrid` được thiết kế như một thư viện hình học tĩnh. Tuy nhiên, nó đã bị "rò rỉ" (leak) trách nhiệm khi tiếp nhận các logic phụ thuộc vào luật chơi (Game Rules) và trạng thái trò chơi (Game State):
- **Tham số `parityOffset`:** Hàm `isEvenRow(row, parityOffset)` và `pixelToNearestCell(..., parityOffset)` đòi hỏi phải biết giá trị bù lưới (parity offset) của ván chơi. Trong một lưới lục giác thuần túy, hàng chẵn/lẻ chỉ phụ thuộc vào toạ độ hình học `(row & 1)`.
- **Logic Ring Buffer:** Hàm `computePhysicalIndex(..., headRowIndex)` giải quyết ánh xạ mảng vật lý với cơ chế cuộn mảng (Ring Buffer) của trò chơi thông qua `headRowIndex`. Đây hoàn toàn là logic tổ chức bộ nhớ Game, không phải toán học không gian lưới.

## 2. Cách khắc phục (Fixes Applied)
- **Rút gọn HexGrid:** Đưa `HexGrid` về đúng vai trò "Toán hình học không gian" (Pure Space Math). Xoá bỏ hoàn toàn các biến nhận diện luật chơi như `parityOffset` hay `headRowIndex`. Các hàm còn lại chỉ là `index(r, c)`, `isValidCell(r, c, isEven)`, `cellToPixelX/Y`, và `pixelToRowCol(x, y)`.
- **Khởi tạo GameBoardMapper:** Thêm một lớp điều phối mỏng (Lightweight Layer) `GameBoardMapper`. Lớp này tiếp quản các phép chiếu logic-vật lý:
  - `isLogicalRowEven(logicalRow, gridParityOffset)`
  - `computePhysicalIndex(logicalRow, col, headRowIndex)`
  - `pixelToNearestCell(px, py_minus_globalOffsetY, gridParityOffset, outRow, outCol)`

## 3. Dependency Graph mới

- **`HexGrid`**: Tầng đáy. Toán hình học thuần túy. KHÔNG có bất kỳ dependency nào ngoài hằng số lưới.
- **`GameBoardMapper`**: Nằm trên HexGrid. Dùng HexGrid để nội suy mảng vật lý với luật Game.
- **`MatchEngine` & `CollisionEngine`**: Gọi `GameBoardMapper` để giải mã tọa độ, sau đó dùng hàm của `HexGrid` để duyệt không gian. Hai Engine này tách biệt, không hề gọi sang nhau hay gọi sang PhysicsEngine.
- **`PhysicsEngine`**: Độc lập tuyệt đối.
- **`Model`**: Gọi các tầng dưới, không chứa bất kỳ logic tính toán tọa độ hay vật lý thủ công nào bên trong thân lớp.

## 4. Xác nhận "No Gameplay Change"
- Toàn bộ refactor này là thao tác **cắt-dán hàm (function displacement)**. Các công thức toán (`roundf`, cộng trừ khoảng cách) và các luồng rẽ nhánh logic `if/else` của BFS/DFS/Collision/Physics được giữ nguyên đến từng bit.
- Sự thay đổi duy nhất là vị trí cấp phát con trỏ hàm, đảm bảo Kiến trúc chuẩn MVP và Stateless Utility.
- Không có bất kỳ thay đổi nào tác động đến UI hay file cấu hình hệ thống STM32 (CubeMX/.ioc).
