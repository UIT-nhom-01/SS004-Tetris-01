# Checklist regression trước khi review

Checklist này áp dụng sau khi sửa code, test hoặc tài liệu của bản Tetris
terminal. Kết quả QA đã ghi cho bản tích hợp nằm trong
[QA Gameplay Report](QA_GAMEPLAY_REPORT.md); checklist này là cách chạy lại
kiểm tra cho một thay đổi mới.

## Build và test tự động

Tạo một thư mục build mới để tránh dùng CMake cache của checkout khác:

```sh
cmake -S . -B build-verify
cmake --build build-verify
ctest --test-dir build-verify --output-on-failure
```

Trên Windows với generator nhiều cấu hình, thêm `--config Release` khi build
và `-C Release` khi chạy CTest. CTest hiện có sáu target: `core_test`,
`tetromino_test`, `collision_test`, `scoring_test`, `game_state_test` và
`integration_test`. Một lần pass xác nhận regression tự động; khi sửa input
hoặc random spawn, cần thử thêm trên terminal tương tác.

## Kiểm tra gameplay trong terminal

| Bước | Điều cần quan sát |
| --- | --- |
| Khởi động | Board 10 x 20, Score 0, active piece, Next Piece và controls xuất hiện |
| `A`/`D` và `←`/`→` | Khối di chuyển trong biên; thao tác ra ngoài biên bị bỏ qua |
| `W` và `↑` | Khối xoay khi vị trí mới hợp lệ; stack không bị ghi đè |
| `S` và `↓` | Khối xuống, khóa khi chạm đáy/stack, preview chuyển thành active |
| Xóa hàng | Hàng đầy biến mất, hàng trên dồn xuống và Score tăng theo bảng điểm |
| Game Over | Khối không thể spawn không phủ lên block đã khóa; gameplay dừng |
| `R` | Board và điểm về trạng thái đầu; gravity và input chạy lại |
| `Q` | Game thoát và trả terminal về screen/cursor ban đầu |

## Trước khi giao thay đổi

1. Xem diff để xác nhận thay đổi chỉ nằm trong phạm vi dự định.
2. Xác nhận build không có warning mới và cả sáu CTest pass.
3. Nếu sửa luồng terminal, thực hiện smoke test tương tác trên hệ điều hành liên quan.
4. Nếu sửa hành vi game, cập nhật mô tả và kiểm thử tương ứng trong cùng phạm vi review.
