# Báo cáo Phase 2.5: Collision Engine Foundation -> MatchEngine Foundation

## 1. Mục tiêu
Tiến hành chia tách (Refactor) thuật toán đồ thị BFS (Breadth-First Search) và DFS (Depth-First Search) từ file `Model.cpp` sang lớp toán học đồ thị tĩnh `MatchEngine` nhằm bảo toàn kiến trúc Single Responsibility.

## 2. Các hàm đã chuyển đổi
- BFS trong `checkMatches` (tìm trứng cùng màu) đã được chuyển thành hàm `MatchEngine::findMatches`.
- DFS/Flood Fill trong `dropFloatingEggs` (tìm cụm trứng mồ côi) đã được chuyển thành hàm `MatchEngine::dropFloatingEggs`.

## 3. Quyết định Kiến trúc (Architecture)
- Tránh cấp phát động (Heap) và hạn chế tiêu thụ Stack: Để duy trì giới hạn phần cứng STM32F429, `MatchEngine` hoàn toàn không tự cấp phát bộ nhớ. Toàn bộ mảng lưu trữ (`visited`, `connected`, `algoQueueStack`, `matchGroup`) đều do Model nắm giữ và truyền mượn (pass by pointer) cho Engine mượn tính toán.
- Trả về số lượng (Count), Model lo Logic Game: Model dùng kết quả BFS/DFS để cộng điểm `player.score` và quyết định Game Over / Win.

## 4. Kiểm tra mã nguồn (Verification)
- Không có bất kỳ file Generated, Presenter, hay View nào bị thay đổi.
- Cấu hình IOC không bị thay đổi.
- Hàm `Model::checkMatches` và `Model::dropFloatingEggs` đã ngắn gọn hơn rất nhiều, hoàn toàn thoát khỏi mớ bòng bong code vòng lặp tọa độ.

## 5. Biên dịch (Build Status)
- Lệnh: `make -j8`
- Thông báo lỗi: `make: The term 'make' is not recognized as the name of a cmdlet, function, script file, or operable program.`
- Khuyến nghị: Cần môi trường MSYS2/MinGW cho PowerShell hoặc dùng trực tiếp STM32CubeIDE.
