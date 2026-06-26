# Collision Audit

## 1. Duplicated Formulas & Helper Functions
1. `calculateDistanceSq`
   - **File:** `TouchGFX/gui/src/model/Model.cpp`
   - **Line:** 380-384
   - **Purpose:** Tính bình phương khoảng cách giữa 2 điểm `(x1,y1)` và `(x2,y2)`.
   - **Duplication:** Logic toán học thuần túy, có thể tái sử dụng.
   - **Action:** Trích xuất thành `CollisionEngine::distanceSquared`.

2. Hitbox Wall Penalty Logic
   - **File:** `TouchGFX/gui/src/model/Model.cpp`
   - **Line:** 415-423 (trong hàm `isCollisionAt`)
   - **Purpose:** Tính hệ số Penalty X khi đạn bay gần sát tường, giúp đạn dễ lách qua khe hẹp hơn.
   - **Duplication:** Phụ thuộc vào tọa độ x, `LEFT_WALL`, `RIGHT_WALL`. Logic tính toán thuần túy.
   - **Action:** Trích xuất thành `CollisionEngine::getWallPenalty`.

## 2. Constants
- `HITBOX_RADIUS`
- `HITBOX_PENALTY_MAX`
- `HITBOX_PENALTY_MIN`
- `HITBOX_WALL_DIST`
Tất cả đang nằm trong `GameConstants.hpp`. Không được duplicate hay di chuyển vì chúng là hằng số cấu hình Gameplay.

## 3. Architecture Decision
- KHÔNG yêu cầu quyết định kiến trúc nào, vì `distanceSquared` và `getWallPenalty` là các hàm số học thuần túy (Pure Math). Việc tách ra hoàn toàn tuân thủ thiết kế No State / No Global của Phase 2.4.
