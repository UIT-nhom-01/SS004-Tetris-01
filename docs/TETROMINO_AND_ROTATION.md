# Tetromino, spawn và rotation

`Tetromino` tạo các khối I, O, T, S, Z, J, L. Factory nhận một loại cụ thể để
test hoặc chọn ngẫu nhiên một trong bảy loại cho game. Mỗi piece được tạo ở
`RotationState::Spawn`, có một pivot và đúng bốn tọa độ block tuyệt đối.

## Spawn và Next Piece

Các block spawn nằm trong ba hàng đầu và trong chiều rộng board 10 cột.
`Game::restart()` tạo active piece và Next Piece. Khi một piece bị khóa, Next
Piece được đưa lên làm active piece, rồi factory sinh preview mới. Sau đó
`Collision::canPlace()` kiểm tra spawn; nếu vị trí bị stack chiếm, game chuyển
sang Game Over và renderer không vẽ piece không thể spawn lên board.

Next Piece chỉ là dữ liệu xem trước; nó không chiếm ô board. Board cũng không
lưu piece đang rơi. Điều này cho phép renderer hiển thị preview mà không làm
thay đổi kết quả collision.

## Xoay theo chiều kim đồng hồ

`getRotated()` trả một bản sao candidate. Hướng xoay tuần tự là `Spawn` →
`Right` → `Reverse` → `Left` → `Spawn`. Với trục `y` tăng xuống dưới, offset
`(dx, dy)` quanh pivot đổi thành `(-dy, dx)`. Pivot giữ nguyên; bốn lần xoay
phải đưa mọi shape về tọa độ ban đầu. Riêng khối O cập nhật hướng xoay nhưng
giữ nguyên các block vì hình vuông không đổi khi xoay.

Factory không biết board hoặc stack. `Game` chỉ áp dụng candidate sau khi
`Collision::canPlace()` xác nhận tất cả block hợp lệ. Nếu candidate ra ngoài
board hoặc đè lên block đã khóa, thao tác xoay bị bỏ qua và trạng thái active
piece giữ nguyên. Bản game hiện tại không áp dụng wall kick.

## Regression cần giữ

- Cả bảy shape spawn với bốn block khác nhau và nằm trong board.
- Cả bảy shape đi hết bốn hướng, giữ pivot và trở lại bố cục spawn.
- Candidate xoay không sửa piece đầu vào; candidate bị chặn không sửa game.
- Preview được chuyển thành active piece sau lock, trước khi kiểm tra Game Over.
