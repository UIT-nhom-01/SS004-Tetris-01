# Input và hiển thị terminal

Game nhận action logic từ `Input`, sau đó `Game` quyết định di chuyển, xoay,
restart hoặc quit. Bảng phím người chơi nằm trong [README](../README.md#5-hướng-dẫn-chơi).

## Ánh xạ phím

| Action | Phím chữ | Phím mũi tên |
| --- | --- | --- |
| Sang trái | `A` hoặc `a` | `←` |
| Sang phải | `D` hoặc `d` | `→` |
| Xuống nhanh | `S` hoặc `s` | `↓` |
| Xoay | `W` hoặc `w` | `↑` |
| Chơi lại | `R` hoặc `r` | Không có |
| Thoát | `Q` hoặc `q` | Không có |

Phím không được nhận diện trở thành `InputAction::None`. Trên macOS/Linux,
`Input` đọc từng byte và giải mã chuỗi mũi tên `ESC [` hoặc `ESC O` theo thứ
tự các byte nhận được. Trên Windows, input mũi tên dùng mã phím mở rộng của
`_getch()`. Game poll input không blocking để gravity tiếp tục chạy theo thời
gian ngay cả khi người chơi không nhấn phím.

## Vòng đời terminal

Khi stdin là TTY trên macOS/Linux, `Input` lưu cấu hình `termios` cũ và bật
chế độ đọc tức thời. Destructor khôi phục cấu hình đó khi game thoát. Khi stdout
là TTY, `Game` dùng alternate screen, ẩn con trỏ và vẽ lại frame tại góc trên
trái. Khi thoát, game trả lại screen và con trỏ.

Renderer chỉ phát mã màu ANSI khi stdout là TTY. Nếu output được chuyển vào
file hoặc pipe, frame dùng ký hiệu `[]` cho block và không có mã màu. Frame
không màu có 22 dòng, mỗi dòng rộng 94 ký tự. Terminal tương tác cần đủ 94 cột
và 22 dòng để không bị xuống dòng ngoài ý muốn.

## Kiểm tra thủ công

1. Chạy `./build/tetris` trong terminal tương tác và thử cả phím chữ lẫn mũi tên.
2. Nhấn `Q`; xác nhận màn hình cũ và con trỏ xuất hiện lại.
3. Nếu dùng macOS/Linux, so sánh `stty -g` trước và sau một lượt chơi.
4. Khi debug bằng pipe, đọc output không màu và kiểm tra không có chuỗi `ESC [`.

Kiểm thử TTY và `termios` là kiểm tra trên máy chạy thật; CTest kiểm tra mapping
ký tự và renderer mà không thay thế thử nghiệm terminal tương tác.
