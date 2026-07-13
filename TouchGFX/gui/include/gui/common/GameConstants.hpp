#ifndef GAME_CONSTANTS_HPP
#define GAME_CONSTANTS_HPP

#include <stdint.h>

namespace GameConstants {
    // Grid & Matrix (Lưới & Ma trận)
    constexpr int MAX_ROWS = 10; // Số hàng tối đa trên lưới trứng
    constexpr int MAX_COLS = 9; // Số cột tối đa trên lưới trứng
    constexpr int MAX_CELLS = MAX_ROWS * MAX_COLS; // Tổng số ô tối đa trên lưới
    
    // UI Layout & Grid (Bố cục giao diện & Lưới)
    constexpr int SCREEN_WIDTH = 240; // Chiều rộng màn hình
    constexpr int EGG_GRID_WIDTH = 228; // Chiều rộng của khu vực lưới trứng
    constexpr float CELL_WIDTH = 24.0f; // Chiều rộng của một ô lưới (tọa độ thực)
    constexpr float CELL_HEIGHT = 28.0f; // Chiều cao của một ô lưới (tọa độ thực)
    constexpr float INV_CELL_HEIGHT = 1.0f / CELL_HEIGHT; // Hằng số nghịch đảo chiều cao ô lưới để thay phép chia bằng phép nhân
    static const int PHYSICS_SUB_STEPS = 3; // Số bước chia nhỏ (sub-stepping) để chống đạn bay xuyên tường
    static const int MAX_TRAJECTORY_STEPS = 300; // Giới hạn số điểm tối đa của tia raycast đường ngắm
    static const int GRID_UI_OFFSET_Y = 22; // Khoảng cách bù Y (offset) khi vẽ lưới trứng lên màn hình
    constexpr float GRID_START_X = 12.0f; // Tọa độ X bắt đầu của lưới
    constexpr float GRID_START_Y = -8.0f; // Tọa độ Y bắt đầu của lưới (Dịch lên 28.0f từ 20.0f)
    constexpr int UI_CELL_WIDTH = 24; // Chiều rộng của một ô lưới trên UI (pixel)
    constexpr int UI_CELL_HEIGHT = 21; // Chiều cao của một ô lưới trên UI (pixel)
    constexpr int UI_CELL_OFFSET_X = 12; // Độ lệch X cho các hàng lẻ trên UI

    // Colors (Màu sắc)
    constexpr int EMPTY_COLOR = 0; // Giá trị đại diện cho ô trống (không có trứng)
    constexpr int NUM_COLORS = 7; // Tổng số màu trứng khả dụng trong game

    // Egg / Bullet Props (Thuộc tính Trứng / Đạn)
    constexpr float EGG_RADIUS = 12.0f; // Bán kính của quả trứng
    constexpr float EGG_WIDTH = 24.0f; // Chiều rộng quả trứng
    constexpr float EGG_HEIGHT = 32.0f; // Chiều cao quả trứng
    constexpr float BULLET_START_X = 120.0f; // Tọa độ X khởi điểm của viên đạn trứng
    constexpr float BULLET_START_Y = 243.0f; // Tọa độ Y khởi điểm của viên đạn trứng
    constexpr float BULLET_SPEED = 9.0f; // Tốc độ bay của viên đạn trứng

    // Bounds & Walls (Giới hạn & Tường)
    constexpr float LEFT_WALL = 8.0f; // Giới hạn tường bên trái
    constexpr float RIGHT_WALL = 208.0f; // Giới hạn tường bên phải
    constexpr float TOP_WALL = 16.0f; // Giới hạn trần (tường phía trên)
    
    // Animations & Timing (Hoạt ảnh & Thời gian)
    constexpr int DINO_THROW_FRAMES = 8; // Số khung hình cho hoạt ảnh khủng long ném trứng
    constexpr int DINO_DONE_FRAMES = 6; // Số khung hình cho hoạt ảnh sau khi khủng long ném xong
    
    // Physics & Aiming (Vật lý & Nhắm bắn)
    constexpr float AIM_SMOOTH_FACTOR = 0.3f; // Hệ số làm mượt khi nhắm bắn
    constexpr float AIM_MIN_DY = -10.0f; // Giới hạn độ dốc tối thiểu của tia nhắm bắn (tránh bắn ngang quá)
    constexpr float AIM_START_Y_OFFSET = 50.0f; // Khoảng lệch Y khi bắt đầu vẽ tia nhắm bắn
    
    constexpr int TRAJECTORY_DOTS_COUNT = 15; // Số lượng chấm hiển thị trên đường dự đoán quỹ đạo
    constexpr int16_t TRAJECTORY_DOT_SIZE = 4; // Kích thước của chấm quỹ đạo (pixel)
    constexpr int16_t TRAJECTORY_DOT_OFFSET = 2; // Độ lệch (tâm) của chấm quỹ đạo
    constexpr float TRAJECTORY_DOT_START_DIST = 30.0f; // Khoảng cách từ điểm bắn đến chấm quỹ đạo đầu tiên
    constexpr float TRAJECTORY_DOT_SPACING = 20.0f; // Khoảng cách giữa các chấm trên đường quỹ đạo
    constexpr uint8_t TRAJECTORY_DOT_COLOR_R = 0; // Giá trị màu Đỏ của chấm quỹ đạo
    constexpr uint8_t TRAJECTORY_DOT_COLOR_G = 0; // Giá trị màu Xanh lá của chấm quỹ đạo
    constexpr uint8_t TRAJECTORY_DOT_COLOR_B = 0; // Giá trị màu Xanh dương của chấm quỹ đạo

    // Hitbox logic (Logic va chạm Hitbox)
    constexpr float HITBOX_RADIUS = 18.04f; // Bán kính vùng va chạm của trứng
    constexpr float HITBOX_PENALTY_MAX = 1.4f; // Mức phạt lớn nhất khi tính toán khoảng cách va chạm (góc xấu)
    constexpr float HITBOX_PENALTY_MIN = 1.0f; // Mức phạt nhỏ nhất khi tính toán khoảng cách va chạm
    constexpr float HITBOX_WALL_DIST = 30.0f; // Khoảng cách tối đa với tường để áp dụng logic phạt va chạm
    
    // Render Invalidation padding (Đệm vùng làm mới vẽ lại)
    constexpr int16_t INVALIDATE_PADDING = 10; // Khoảng đệm thêm xung quanh vùng cần vẽ lại
    constexpr int16_t INVALIDATE_WIDTH = 44; // Chiều rộng cơ bản của vùng vẽ lại cho một quả trứng
    constexpr int16_t INVALIDATE_HEIGHT = 52; // Chiều cao cơ bản của vùng vẽ lại cho một quả trứng
    constexpr uint8_t SHADOW_GRID_INIT_VAL = 0xFF; // Giá trị khởi tạo cho lưới bóng (chưa có trứng)
    constexpr float CAMERA_DISTANCE_Z = 1000.0f; // Khoảng cách camera giả lập trục Z trong vẽ phối cảnh
    constexpr int DINO_COLOR_OFFSET = 100; // Khoảng dịch ID màu dành cho khủng long

    // Scoring (Tính điểm)
    constexpr int SCORE_MATCH_3 = 3; // Điểm khi ăn được 3 quả trứng
    constexpr int SCORE_MATCH_4 = 5; // Điểm khi ăn được 4 quả trứng
    constexpr int SCORE_MATCH_5 = 8; // Điểm khi ăn được từ 5 quả trứng trở lên
    constexpr int SCORE_DROP_ORPHAN = 1; // Điểm cho mỗi quả trứng rớt do mất liên kết (mồ côi)

    // Game Logic (Logic trò chơi)
    constexpr uint32_t RNG_INITIAL_SEED = 12345; // Hạt giống ban đầu cho bộ sinh số ngẫu nhiên
    constexpr int MAX_SPAWN_LUT_INDEX = 31; // Chỉ số lớn nhất của mảng tốc độ sinh trứng mới
    constexpr int CLEARING_FRAMES_DELAY = 30; // Số khung hình chờ khi đang xóa trứng (hiệu ứng nổ)
    constexpr int SPAWN_INTERVAL_START = 1556; // Khoảng thời gian (số khung hình) ban đầu giữa các lần đẩy lưới trứng
    constexpr int SPAWN_INTERVAL_LUT[32] = { // Mảng tra cứu thời gian giãn cách đẩy lưới trứng (nhanh dần)
        1556,1556,1556,1556,1556,1556,1556,1556,1556,1556,
        1444,1444,1444,1444,1444,1444,1444,1444,
        1333,1333,1333,1333,1333,1333,
        1222,1222,1222,1222,
        1111,1111,1111,1111
    };
    constexpr uint32_t RNG_MULTIPLIER = 1664525; // Hệ số nhân cho thuật toán sinh số ngẫu nhiên LCG
    constexpr uint32_t RNG_INCREMENT = 1013904223; // Hằng số cộng cho thuật toán sinh số ngẫu nhiên LCG
    constexpr int INITIAL_ROWS = 3; // Số hàng trứng khởi tạo lúc bắt đầu màn chơi
    constexpr int GAME_OVER_ROW = 9; // Chỉ số hàng (vạch đích) nếu trứng chạm tới sẽ thua game
    constexpr int MIN_BALLS_THRESHOLD = 12; // Ngưỡng số trứng tối thiểu để kích hoạt logic đặc biệt (hoặc tăng độ khó)
    constexpr float DROP_SPEED_START = 0.027f; // Tốc độ trượt xuống của lưới trứng lúc đầu (pixel mỗi khung hình)
    constexpr float DROP_SPEED_INCREMENT = 0.018f; // Mức tăng tốc độ trượt lưới theo độ khó
    constexpr float DROP_SPEED_MAX = 0.27f; // Tốc độ trượt lưới tối đa
    
    constexpr int HEX_NEIGHBORS_COUNT = 6; // Số lượng hàng xóm tối đa trong lưới lục giác
    constexpr int COLLISION_SCAN_RADIUS = 1; // Bán kính quét va chạm xung quanh viên đạn
    constexpr float MAX_DIST_SQ_INIT = 999999.0f; // Giá trị bình phương khoảng cách rất lớn để khởi tạo tìm min
    constexpr int MIN_MATCH_COUNT = 3; // Số lượng trứng cùng màu tối thiểu nối nhau để được tính là ăn điểm

    // Color IDs (ID các màu trứng)
    constexpr int COLOR_BLUE = 1; // Trứng màu Xanh dương
    constexpr int COLOR_GREEN = 2; // Trứng màu Xanh lá
    constexpr int COLOR_ORANGE = 3; // Trứng màu Cam
    constexpr int COLOR_PINK = 4; // Trứng màu Hồng
    constexpr int COLOR_PURPLE = 5; // Trứng màu Tím
    constexpr int COLOR_RED = 6; // Trứng màu Đỏ
    constexpr int COLOR_YELLOW = 7; // Trứng màu Vàng

    // LUTs (Pre-computed Lookup Tables - Mảng tra cứu tính sẵn)
    constexpr int NEIGHBOR_OFFSETS[2][HEX_NEIGHBORS_COUNT][2] = { // Mảng tra cứu độ lệch tọa độ 6 hàng xóm trong lưới lục giác
        {{-1,-1}, { 0,-1}, {-1, 0}, { 1, 0}, {-1, 1}, { 0, 1}}, // Cho các hàng chẵn (EVEN)
        {{ 0,-1}, { 1,-1}, {-1, 0}, { 1, 0}, { 0, 1}, { 1, 1}}  // Cho các hàng lẻ (ODD)
    };

    constexpr float MODEL_CELL_Y[MAX_ROWS] = { // Mảng tọa độ Y thực của các ô trên lưới
        GRID_START_Y + 0 * CELL_HEIGHT, GRID_START_Y + 1 * CELL_HEIGHT,
        GRID_START_Y + 2 * CELL_HEIGHT, GRID_START_Y + 3 * CELL_HEIGHT,
        GRID_START_Y + 4 * CELL_HEIGHT, GRID_START_Y + 5 * CELL_HEIGHT,
        GRID_START_Y + 6 * CELL_HEIGHT, GRID_START_Y + 7 * CELL_HEIGHT,
        GRID_START_Y + 8 * CELL_HEIGHT, GRID_START_Y + 9 * CELL_HEIGHT
    };

    constexpr float MODEL_CELL_X_EVEN[MAX_COLS] = { // Mảng tọa độ X thực của các ô trên lưới (hàng chẵn)
        GRID_START_X + 0 * CELL_WIDTH, GRID_START_X + 1 * CELL_WIDTH,
        GRID_START_X + 2 * CELL_WIDTH, GRID_START_X + 3 * CELL_WIDTH,
        GRID_START_X + 4 * CELL_WIDTH, GRID_START_X + 5 * CELL_WIDTH,
        GRID_START_X + 6 * CELL_WIDTH, GRID_START_X + 7 * CELL_WIDTH,
        GRID_START_X + 8 * CELL_WIDTH
    };

    constexpr float MODEL_CELL_X_ODD[MAX_COLS] = { // Mảng tọa độ X thực của các ô trên lưới (hàng lẻ - lệch nửa ô)
        GRID_START_X + 0 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 1 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 2 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 3 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 4 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 5 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 6 * CELL_WIDTH + (CELL_WIDTH / 2.0f), GRID_START_X + 7 * CELL_WIDTH + (CELL_WIDTH / 2.0f),
        GRID_START_X + 8 * CELL_WIDTH + (CELL_WIDTH / 2.0f)
    };

    constexpr int UI_CELL_Y[MAX_ROWS] = { // Mảng tọa độ Y trên giao diện đồ họa (pixel)
        0 * UI_CELL_HEIGHT, 1 * UI_CELL_HEIGHT,
        2 * UI_CELL_HEIGHT, 3 * UI_CELL_HEIGHT,
        4 * UI_CELL_HEIGHT, 5 * UI_CELL_HEIGHT,
        6 * UI_CELL_HEIGHT, 7 * UI_CELL_HEIGHT,
        8 * UI_CELL_HEIGHT, 9 * UI_CELL_HEIGHT
    };

    constexpr int UI_CELL_X_EVEN[MAX_COLS] = { // Mảng tọa độ X trên giao diện đồ họa cho hàng chẵn (pixel)
        0 * UI_CELL_WIDTH, 1 * UI_CELL_WIDTH,
        2 * UI_CELL_WIDTH, 3 * UI_CELL_WIDTH,
        4 * UI_CELL_WIDTH, 5 * UI_CELL_WIDTH,
        6 * UI_CELL_WIDTH, 7 * UI_CELL_WIDTH,
        8 * UI_CELL_WIDTH
    };

    constexpr int UI_CELL_X_ODD[MAX_COLS] = { // Mảng tọa độ X trên giao diện đồ họa cho hàng lẻ (pixel)
        0 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 1 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        2 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 3 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        4 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 5 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        6 * UI_CELL_WIDTH + UI_CELL_OFFSET_X, 7 * UI_CELL_WIDTH + UI_CELL_OFFSET_X,
        8 * UI_CELL_WIDTH + UI_CELL_OFFSET_X
    };

    // Đóng gói Hàng và Cột vào một số nguyên 16-bit
    inline uint16_t packHex(uint8_t row, uint8_t col) { return (row << 8) | col; }
    // Lấy giá trị Hàng từ số nguyên 16-bit
    inline uint8_t unpackRow(uint16_t val) { return val >> 8; }
    // Lấy giá trị Cột từ số nguyên 16-bit
    inline uint8_t unpackCol(uint16_t val) { return val & 0xFF; }
}

#endif // GAME_CONSTANTS_HPP
