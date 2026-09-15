# Contract kiến trúc của game

Tài liệu này ghi các ranh giới cần giữ khi sửa hoặc kiểm thử bản Tetris terminal.
Sơ đồ tổng thể và public API nằm trong [README](../README.md#6-kiến-trúc-dự-án).

## Quyền sở hữu trạng thái

| Thành phần | Trạng thái hoặc trách nhiệm sở hữu |
| --- | --- |
| `Game` | Vòng lặp, active/next piece và thứ tự gọi các module |
| `GameBoard` | Lưới 10 x 20 chỉ chứa block đã khóa |
| `Tetromino` | Hình dạng, vị trí spawn và candidate xoay |
| `Collision` | Kiểm tra vị trí, khóa block và xóa hàng |
| `Scoring` | Điểm của một lượt chơi |
| `GameState` | Cờ Game Over của một lượt chơi |
| `Input` | Chuyển phím thành `InputAction` |
| `ConsoleRenderer` | Tạo frame từ trạng thái được truyền vào |

`Game::board()` trả về tham chiếu chỉ đọc. Trong luồng chạy thật, game chỉ thay
đổi các ô đã khóa thông qua `Collision`; active piece không nằm trong board.
Renderer ghép hai nguồn dữ liệu này khi dựng frame và không sửa chúng.

## Thứ tự một gravity tick

1. `Game` tạo candidate dịch xuống một ô và hỏi `Collision::canPlace()`.
2. Nếu hợp lệ, candidate trở thành active piece; board và score giữ nguyên.
3. Nếu bị chặn, `Collision` khóa piece đang rơi, xóa các hàng đầy và trả số hàng.
4. `Scoring` cộng điểm; Next Piece được đưa lên làm active piece.
5. `Tetromino` sinh preview mới; `GameState` đánh giá vị trí spawn của active piece.

Di chuyển ngang và xoay cũng dùng candidate. Candidate bị từ chối phải giữ
nguyên active piece, board và score. Sau Game Over, input gameplay và gravity
không làm thay đổi phiên chơi; Restart reset board, score, cờ Game Over và hai
piece mà không khởi động lại tiến trình.

## Contract dữ liệu chung

`Position` dùng tọa độ tuyệt đối, gốc `(0, 0)` tại góc trên trái. `ActivePiece`
gồm loại khối, hướng xoay, pivot và bốn block. `CellState` giữ loại của block đã
khóa để renderer dùng lại màu tương ứng. Các feature dùng kiểu chung trong
`include/core/Types.hpp`; chúng không tự đọc terminal hoặc tự render.
