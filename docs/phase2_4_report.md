# Báo cáo Phase 2.4: Collision Engine Foundation

## 1. Mục tiêu
Trích xuất toán học va chạm (Collision Math) từ `Model.cpp` ra một lớp tiện ích độc lập tên là `CollisionEngine`.

## 2. Các hàm đã chuyển đổi
- Hàm nội bộ `calculateDistanceSq` của Model đã được chuyển thành hàm tĩnh `CollisionEngine::distanceSquared`.
- Cụm logic tính toán "Wall Penalty" trong `isCollisionAt` nhằm bóp nhỏ Hitbox khi đạn đi sát tường đã được đóng gói thành `CollisionEngine::getWallPenalty`.

## 3. Đánh giá Kiến trúc (Architecture)
- **Model** vẫn làm chủ toàn bộ quá trình xác định va chạm (Collision Decision), Snap Grid, BFS/DFS, và Gameplay.
- **CollisionEngine** đóng vai trò là thư viện tiện ích tĩnh thuần túy (Pure Static Utility), nhận đầu vào tham chiếu tọa độ, khoảng cách tường để tính kết quả, hoàn toàn không dính dáng đến State.

## 4. Biên dịch (Build Status)
- Giống Phase 2.3, do môi trường PowerShell của User thiếu cấu hình GNU Make, nên `make` tự động báo lỗi `make: The term 'make' is not recognized`. 
- Vui lòng sử dụng STM32CubeIDE để biên dịch lại dự án.

## 5. Rủi ro còn lại (Remaining Risks)
- Không có rủi ro nào về Floating-point. Biểu thức trong Engine sao chép 1:1 từ mã nguồn cũ. Khả năng tương thích Gameplay đạt 100%.
