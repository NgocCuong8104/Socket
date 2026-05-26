# 📡 Socket File Transfer - Client/Server Project

## 📋 Tổng Quan Dự Án

Đây là một ứng dụng **Truyền File qua Socket TCP** (Client-Server) có hỗ trợ **Resume** - tức là nếu kết nối bị mất, có thể tiếp tục truyền từ vị trí dừng lại mà không cần gửi lại từ đầu.

**Ngôn ngữ:** C (POSIX)  
**Giao thức:** TCP/IP  
**Cổng mặc định:** 5001  
**Kích thước buffer:** 1024 bytes

---

## 🔧 Nguyên Lí Hoạt Động

### Kiến Trúc

```
CLIENT                          SERVER
   ↓                               ↓
[Gửi file]  ←→ Socket TCP ←→  [Nhận file]
```

### Quy Trình Chi Tiết

#### 1️⃣ Thiết lập kết nối
- **Server**: Khởi tạo socket trên cổng **5001** - lắng nghe các client kết nối
- **Client**: Kết nối tới `127.0.0.1:5001` (localhost)

#### 2️⃣ Gửi tên file
- Client → gửi tên file (ví dụ: `test_large_file.txt`)
- Server → nhận tên file, thêm tiền tố `received_` để lưu với tên `received_test_large_file.txt`

#### 3️⃣ Resume Logic (Điểm chính)
- Server → kiểm tra xem file đã tồn tại chưa, tính kích thước hiện có (`offset`)
- Server → gửi offset cho client
- Client → nhận offset, **seek (nhảy) tới vị trí đó** trong file cần gửi
- Client → chỉ gửi dữ liệu từ offset đó trở đi

#### 4️⃣ Truyền dữ liệu
- Client → đọc file từng chunk 1024 bytes từ offset
- Server → nhận từng chunk và ghi nối tiếp vào file (`"ab"` mode)
- Hiển thị tiến độ: `Đã gửi: X bytes`

#### 5️⃣ Kết thúc
- Client gửi hết file → đóng socket
- Server phát hiện client ngắt → lưu file, đóng socket, quay lại chờ client mới

---

## 🚀 Cách Vận Hành

### Bước 1: Chuẩn Bị File Test

Tạo file `test_large_file.txt` trong thư mục dự án:

```bash
dd if=/dev/zero of=test_large_file.txt bs=1M count=10   # Tạo file 10MB
```

### Bước 2: Biên Dịch

```bash
gcc -o server server.c network.c transfer.c
gcc -o client client.c network.c transfer.c
```

### Bước 3: Chạy Server (Terminal 1)

```bash
./server
```

**Output:**
```
[*] Server đang lắng nghe trên cổng 5001...
```

### Bước 4: Chạy Client (Terminal 2)

```bash
./client
```

**Output:**
```
[+] Đã kết nối tới Server!
[*] Server yêu cầu bắt đầu gửi từ byte: 0
[*] Đang gửi...
Đã gửi: 10485760 bytes
[*] Truyền tải kết thúc.
```

### Bước 5: Test Resume (Tính Năng Chính)

1. Chạy client, giữa quá trình gửi → bấm **Ctrl+C** (ngắt kết nối)
2. Chạy client lại → nó sẽ:
   - Gửi tên file
   - Server trả về offset (ví dụ: `5000000`)
   - Client tiếp tục gửi từ byte thứ 5000001
   - ✅ File được nhận đầy đủ mà không gửi lại từ đầu

---

## 📁 Cấu Trúc File

| File | Mục đích |
|------|---------|
| **config.h** | Định nghĩa hằng số: PORT (5001), BUFFER_SIZE (1024) |
| **network.h** | Khai báo hàm khởi tạo và kết nối socket |
| **network.c** | Cài đặt: `init_server_socket()`, `connect_to_server()` |
| **transfer.h** | Khai báo hàm gửi/nhận file |
| **transfer.c** | Cài đặt logic truyền file với resume |
| **server.c** | Chương trình server chính - lắng nghe & nhận file |
| **client.c** | Chương trình client chính - kết nối & gửi file |
| **README.md** | Tài liệu hướng dẫn (file này) |

---

## ⚙️ Tính Năng Chính

✅ **Truyền file qua TCP** - sử dụng socket stream  
✅ **Hỗ trợ Resume** - tiếp tục từ vị trí dừng lại  
✅ **Hiển thị tiến độ** - theo dõi bytes đã gửi  
✅ **Xử lý lỗi kết nối** - phát hiện client ngắt kết nối  
✅ **Buffer 1024 bytes** - tối ưu cho mạng  
✅ **Server đa client** - xử lý từng client tuần tự  

---

## 📊 Luồng Hoạt Động

### Server Flow
```
1. init_server_socket(5001)
   ↓
2. accept() - chờ client
   ↓
3. recv(filename)
   ↓
4. get_current_offset() - kiểm tra file tồn tại?
   ↓
5. send(offset)
   ↓
6. recv(data) - lặp nhận từng chunk
   ↓
7. fwrite() - ghi vào file
   ↓
8. close() - đóng socket, quay lại bước 2
```

### Client Flow
```
1. connect_to_server("127.0.0.1", 5001)
   ↓
2. send(filename)
   ↓
3. recv(offset)
   ↓
4. fseek(offset) - nhảy tới vị trí
   ↓
5. fread() - đọc chunk
   ↓
6. send(data) - gửi chunk
   ↓
7. Lặp bước 5-6 cho đến hết file
   ↓
8. close() - đóng socket
```

---

## 🐛 Tính Năng Nâng Cao (Có Thể Thêm)

- [ ] Hiển thị tốc độ truyền (bytes/second)
- [ ] Hỗ trợ truyền nhiều file cùng lúc (threading)
- [ ] Mã hóa truyền (SSL/TLS)
- [ ] Xác thực client
- [ ] Báo cáo lỗi chi tiết hơn
- [ ] Hỗ trợ IPv6

---

## 📝 Lưu Ý

- Server chỉ xử lý 1 client mỗi lần (tuần tự)
- File nhận được được lưu với tiền tố `received_`
- Nếu client bị ngắt, offset vẫn giữ nguyên cho lần kết nối tiếp theo
- Buffer size 1024 bytes có thể tăng để truyền nhanh hơn (cần chỉnh CONFIG_H)

---

**Tác giả:** Dự án Socket File Transfer  
**Ngày tạo:** 2026  
**Phiên bản:** 1.0
