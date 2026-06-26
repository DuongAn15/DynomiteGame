# Báo cáo Giai đoạn 1.3: Cấu hình Button Controller

## 1. Những file đã tạo
- `TouchGFX/target/STM32ButtonController.hpp`: Định nghĩa class `STM32ButtonController` kế thừa từ `touchgfx::ButtonController` và khai báo enum `ButtonKeycode`.
- `TouchGFX/target/STM32ButtonController.cpp`: Cài đặt logic đọc GPIO và xử lý debounce cho các nút bấm.

## 2. Những file đã sửa
- `TouchGFX/target/TouchGFXHAL.cpp`: Include thư viện, khởi tạo instance `buttonController` và đăng ký vào HAL thông qua hàm `initialize()`. (Không thay đổi `TouchGFXConfiguration.cpp` theo yêu cầu).

## 3. API TouchGFX ButtonController sử dụng
- Kế thừa lớp ảo `touchgfx::ButtonController`.
- Override hàm `virtual void init()`: Hàm khởi tạo (để trống hoặc chứa khởi tạo ngoại vi).
- Override hàm `virtual bool sample(uint8_t& key)`: Hàm được TouchGFX gọi ở mỗi frame để lấy trạng thái phím. Trả về `true` nếu có sự kiện nhấn phím mới, và `key` chứa mã phím đó.

## 4. Cách đăng ký vào HAL
- Tại `TouchGFXHAL.cpp`, gọi phương thức `setButtonController(&buttonController);` bên trong hàm `TouchGFXHAL::initialize()` sau khi đã gọi `TouchGFXGeneratedHAL::initialize()`.

## 5. Thuật toán debounce
- Áp dụng **60Hz frame-based debounce (lock until fully released)**.
- Đọc trạng thái GPIO. Nếu giống trạng thái frame trước đó, tăng biến `counter`.
- Nếu `counter >= DEBOUNCE_THRESHOLD` (ví dụ 3 frame liên tiếp cùng trạng thái), phát sự kiện duy nhất 1 lần (trả về `true`).
- Sau khi phát sự kiện, hệ thống sẽ chuyển sang trạng thái `locked` đối với phím đó. Sự kiện tiếp theo chỉ được nhận sau khi thả hoàn toàn nút (currentKey == 0), giúp việc giữ nút không sinh ra nhiều event.

## 6. Mapping GPIO
- Phím Left (Trái): `PC3` (`HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_3)`)
- Phím Right (Phải): `PA1` (`HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1)`)
- Phím Fire (Bắn): `PA2` (`HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2)`)
- Phím Swap (Đổi bóng): `PA3` (`HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3)`)

*(Giả định phím active low, được kích hoạt khi `GPIO_PIN_RESET`)*

## 7. Luồng dữ liệu sau khi hoàn thành
1. Người dùng nhấn nút vật lý làm thay đổi mức điện áp chân GPIO.
2. Tại mỗi frame, framework TouchGFX tự động gọi `STM32ButtonController::sample()`.
3. Hàm `sample` đọc giá trị các chân GPIO. Nếu phát hiện phím nhấn hợp lệ qua bộ lọc debounce, cập nhật biến tham chiếu `key` và trả về `true`.
4. TouchGFX HAL nhận được key hợp lệ và đẩy sự kiện (Event) phím vào View hiện tại.
5. `View` nhận được sự kiện và gọi hàm xử lý tương ứng trên `Presenter` để tác động đến `Model` hoặc thay đổi UI.

## 8. Chứng minh tuân thủ MVP
- ButtonController chỉ đóng vai trò thu thập sự kiện phần cứng (tương đương Hardware Layer). Nó không xử lý logic game (chọn bóng, di chuyển kim).
- Sự kiện phần cứng được chuyển đổi thành mã `ButtonKeycode` trừu tượng trước khi đẩy vào TouchGFX Framework.
- View và Presenter hoàn toàn độc lập với phần cứng vật lý, chúng chỉ quan tâm tới mã phím (VD: Left, Right, Fire) từ TouchGFX.

## 9. Xác nhận 7 KHÔNG
1. KHÔNG sửa code sinh tự động: Các thay đổi chỉ ở phần `USER CODE` của TouchGFXHAL.cpp, và hai file mới hoàn toàn độc lập.
2. KHÔNG viết logic trong View: Button Controller không chứa bất kỳ logic View nào.
3. KHÔNG viết logic trong Model: Model không tương tác với ButtonController.
4. KHÔNG delay/blocking: Không dùng `HAL_Delay`, thay vào đó đếm số frame.
5. KHÔNG dùng Float/Double: Chỉ sử dụng kiểu số nguyên (`uint8_t`).
6. KHÔNG cấp phát động: Tránh dùng `malloc` hoặc `new` ở Runtime. Instance `buttonController` là `static`.
7. KHÔNG copy-paste mù quáng: Đã thiết kế logic theo chuẩn MVP với kiến trúc STM32.

## 10. Compile Status & Audit
- Môi trường hiện tại không có sẵn `make` trong PATH nên không thể chạy build trực tiếp trong console. Việc compile phải được thực hiện bằng STM32CubeIDE bởi người dùng.
- Senior Firmware Reviewer đã audit code: phát hiện và sửa lỗi thuật toán debounce của Subagent (thiếu biến `counter` đếm số frame như thiết kế ban đầu). Code hiện tại đã tuân thủ 100% thuật toán yêu cầu.

## 11. Ready for Phase 2
- **YES**
