#ifndef MATCHENGINE_HPP
#define MATCHENGINE_HPP

#include <stdint.h>

class MatchEngine {
public:
    static int getPhysicalIndex(int logicalRow, int col, int headRowIndex);

    // Thực hiện BFS để tìm các trứng cùng màu liên kết với trứng ở (startRow, startCol).
    // Trả về số lượng trứng (matchCount). Các tọa độ được lưu trong matchGroup dưới dạng (r << 8) | c.
    static int findMatches(
        const uint8_t* grid,
        int headRowIndex,
        int gridParityOffset,
        int startRow,
        int startCol,
        int* matchGroup,
        bool* visited,
        int* algoQueueStack);

    // Thực hiện DFS/Flood Fill từ hàng trên cùng (row 0) để tìm các trứng dính tường.
    // Nhứng trứng không dính tường (orphan/floating) sẽ bị xóa trực tiếp (gán EMPTY_COLOR) trên grid.
    // Trả về số lượng trứng đã bị rơi (dropCount).
    static int dropFloatingEggs(
        uint8_t* grid,
        int headRowIndex,
        int gridParityOffset,
        bool* connected,
        int* algoQueueStack);
};

#endif // MATCHENGINE_HPP
