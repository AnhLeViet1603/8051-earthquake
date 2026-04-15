# Earthquake Detector Project (AT89S52)

Đây là dự án hệ thống phát hiện động đất sử dụng vi điều khiển họ 8051 (AT89S52) kết hợp với cảm biến gia tốc ADXL345 và còi báo động (buzzer).

*(Lưu ý: Màn hình OLED SSD1306 hiện đã được vô hiệu hóa mã nguồn và tắt trong Makefile do giới hạn quá mức về phần cứng RAM nội AT89S52).*

## Tính năng

*   **Nhận diện rung động:** Đọc dữ liệu gia tốc 3 trục (X, Y, Z) từ ADXL345 qua giao thức I2C.
*   **Xử lý tín hiệu:** Sử dụng bộ lọc EMA (Exponential Moving Average) Low-Pass và thuật toán High-Pass filter đơn giản để loại bỏ nhiễu và tác động của trọng lực tĩnh.
*   **Thuật toán phát hiện động đất:** Dựa trên tổng độ lớn gia tốc (Magnitude) và thuật toán debounce (tránh phát hiện sai do gai tín hiệu nhiễu).
*   **Cảnh báo:** Kích hoạt Buzzer tức thì khi phát hiện độ rung đạt ngưỡng kích hoạt động đất.
*   **Được tối ưu cho cấu hình siêu thấp:** Triển khai luồng đọc/ghi cảm biến bằng các Shared Globals nhằm cắt giảm tuyệt đối bộ nhớ truyền tham số Stack/Param giữa các context function.

## Tối ưu hoá cực đại cho bộ nhớ 8051 (RAM Optimization)
Vì vi điều khiển AT89S52 chỉ có **256 byte RAM nội**, việc chia sẻ bộ nhớ cho thuật toán Lọc - Tính toán cũng như điều khiển màn hình dẫn đến tràn bộ nhớ (Overflow/stack overlap). Dự án đã áp dụng các tối ưu kiến trúc:
- **Zero-Parameters & Global Variables:** Thiết lập biến toàn cục `g_ax`, `g_ay`, `g_az`, `g_mag` ở định dạng shared (extern) để làm lõi vận chuyển dữ liệu mà không tốn các stack/tham số cho các hàm `adxl_read`, `filter` và `detect`.
- **Biến Static (`static buffer`):** Cưỡng ép mảng giao tiếp bộ đệm I2C tĩnh (`static`) nhằm hỗ trợ Linker ghi đè (Memory Overlay) một cách hiệu quả các node ram tạm tính.
- **Vòng lặp phẳng (Flat Call Chain):** Gọi hàm tuần tự mà rỗng param chặn hoàn toàn hiện tượng chain rác trên stack.

## Sơ đồ kết nối phần cứng

| Thành phần | Chân 8051 (AT89S52) | Mô tả / Ghi chú |
| :--- | :--- | :--- |
| **I2C SCL** | `P1.2` | Giao tiếp I2C SCL kết nối với SCL của ADXL345. *(Cần điện trở pull-up 4.7k - 10k lên VCC)* |
| **I2C SDA** | `P1.3` | Giao tiếp I2C SDA kết nối với SDA của ADXL345. *(Cần điện trở pull-up 4.7k - 10k lên VCC)* |
| **ADXL345 INT1** | `P3.2` (INT0) | Chân báo ngắt DATA_READY từ cảm biến. |
| **ADXL345 SDO** | `GND` | Cài đặt địa chỉ I2C cho ADXL345 là `0x53`. |
| **Buzzer** | `P2.7` | Cấp mức logic HIGH kích hoạt Buzzer (qua Transistor NPN như 2N2222 hay BC547). |
| **Thạch anh** | XTAL1/XTAL2 | 12MHz (chân 18, 19) kèm tụ 22pF xuông GND. |

## Môi trường phát triển & Nạp chương trình

*   **Compiler:** SDCC (Small Device C Compiler) >= 4.0.
*   **Tools:** `make` (GNU Make), `packihx` (đi kèm SDCC).
*   **Nạp vào MCU:** Để nạp `.hex` vào chip, sử dụng công cụ Python đi kèm là `stcflash.py`:
    ```bash
    pip install pyserial
    python tools/stcflash.py -p COM3 build/Debug/89c52_sdcc_demo.hex
    ```

## Biên dịch

Dự án này sử dụng thẻ `make` và đã được thiết lập để kết hợp việc biên dịch trong một lệnh duy nhất. Cấu trúc sẽ biên dịch `src/*.c` qua `.rel` rồi tổng hợp.

Mở terminal ở thư mục root của dự án và chạy:

```bash
make
```

*Lệnh dọn dẹp các tệp build tạm (Windows tương thích):* Dùng `make clean` nếu muốn dọn sạch không gian.

## Cấu trúc thư mục

*   `inc/`: Chứa tất cả tệp tiêu đề (Header files) gồm driver ngoại vi (`adxl345.h`, `buzzer.h`, `i2c.h`) và cài đặt hệ thống (`config.h`), giải thuật (`filter.h`, `detect.h`). *(Driver oled nằm yên tại đây nhưng tạm vô hiệu hóa).*
*   `src/`: Chứa tất cả tệp nguồn C (Source files) thực hiện thuật toán xử lý dữ liệu rung tần số thấp và nháy còi.
*   `Makefile`: Chứa quy tắc biên dịch (`make`).
