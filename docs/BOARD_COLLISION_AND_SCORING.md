# Board, collision, xóa hàng và tính điểm

`GameBoard` là lưới 10 x 20. Một ô là `Empty` hoặc mang loại khối đã khóa.
Tọa độ hợp lệ có `x` trong `[0, 9]` và `y` trong `[0, 19]`. `getCell()` và
`setCell()` ném `std::out_of_range` khi tọa độ ở ngoài lưới, để lỗi truy cập
không âm thầm sửa ô khác.

## Kiểm tra và khóa khối

`Collision::canPlace()` chỉ trả `true` khi cả bốn block nằm trong board và
tất cả ô tương ứng còn trống. Hàm này không sửa board hoặc piece. `Game` dùng
nó cho spawn, dịch chuyển, xoay và gravity candidate. Chỉ khi khối không thể
rơi thêm, `Collision::lockPiece()` ghi bốn block vào board bằng `CellState`
cùng loại với piece; loại này giúp màu của block được giữ sau khi khóa.

## Xóa hàng đầy

Một hàng chỉ đầy khi cả 10 ô đều đã bị chiếm. `clearCompletedLines()` duyệt
từ đáy lên, bỏ qua các hàng đầy và dồn các hàng còn sống xuống dưới mà không
đảo thứ tự hay đổi loại ô. Vùng hàng trống mới ở phía trên được ghi lại thành
`Empty`. Hàm trả về tổng số hàng bị xóa trong lần khóa đó.

Các trường hợp biên cần kiểm thử gồm: board không có hàng đầy phải giữ mọi
ô; một hàng ở đáy; hàng ở trên cùng; nhiều hàng liên tiếp; và nhiều hàng đầy
cách nhau bởi hàng không đầy. Sau khi dồn, các ô còn sống phải giữ loại khối
để renderer tiếp tục dùng đúng màu.

## Tính điểm và phiên chơi

`Game` đưa số hàng vừa xóa cho `Scoring::addLines()`. Điểm được cộng theo
toàn bộ sự kiện khóa, không cộng riêng từng hàng:

| Số hàng xóa | Điểm cộng |
| --- | ---: |
| 0 | 0 |
| 1 | 100 |
| 2 | 300 |
| 3 | 500 |
| 4 | 800 |

Điểm tích lũy giữa các lần khóa. Input ngoài khoảng 0–4 ném
`std::invalid_argument` và không thay đổi điểm đã có. `Game::restart()` reset
score về 0 cùng với board và Game Over state để bắt đầu một lượt chơi mới.
