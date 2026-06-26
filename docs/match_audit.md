# Match Audit

## 1. Duplicated Algorithms & Traversal Code
1. `checkMatches` (BFS)
   - **File:** `TouchGFX/gui/src/model/Model.cpp`
   - **Line:** 268-302
   - **Algorithm:** Breadth-First Search (BFS).
   - **Duplicated Neighbor Iteration:** Duyệt 6 hướng của lưới lục giác. Vòng lặp `for (int i = 0; i < HEX_NEIGHBORS_COUNT; i++)` giống hệt phần duyệt DFS.
   - **Action:** Tách phần BFS thuần túy thành `MatchEngine::findMatches`.

2. `dropFloatingEggs` (DFS / Connected Components)
   - **File:** `TouchGFX/gui/src/model/Model.cpp`
   - **Line:** 328-359
   - **Algorithm:** Depth-First Search (DFS) / Tràn ngập (Flood Fill).
   - **Duplicated Neighbor Iteration:** Duyệt 6 hướng giống BFS.
   - **Action:** Tách phần tìm cụm trứng dính tường thành `MatchEngine::findFloatingEggs`.

## 2. Queue & Stack Usage
- Cả hai thuật toán đều dùng chung `algoQueueStack` (mảng 1 chiều) làm Queue (BFS) và Stack (DFS).
- Để giữ nguyên thiết kế No State, No Heap, mảng này (cùng `visited`, `connected`, `matchGroup`) vẫn sẽ do `Model` nắm giữ, và chỉ truyền tham chiếu/con trỏ vào `MatchEngine`.

## 3. Architecture Decision
- KHÔNG yêu cầu quyết định kiến trúc nào. BFS và DFS là các hàm toán học thuần túy trên đồ thị (Graph Math). `MatchEngine` sẽ nhận con trỏ của lưới `grid` và các buffer, trả về số lượng trứng match hoặc số lượng trứng mồ côi (floating), giữ nguyên 100% logic Gameplay cho Model.
