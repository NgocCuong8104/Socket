# 📡 Socket File Transfer - Client/Server Project

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

## 🚀 Các Bước Thực Hiện Truyền File

### Bước 1: Biên Dịch Dự Án

Sử dụng CMake để biên dịch:

```bash
cd /home/cuong/Socket
mkdir -p build && cd build
cmake ..
make
```

**Output:**
```
[100%] Built target server_app
[100%] Built target client_app
```

Hai file executable sẽ được tạo:
- `build/server_app` - Chương trình server
- `build/client_app` - Chương trình client

---

### Bước 2: Chuẩn Bị File Test

Trong thư mục `build`, tạo file test để truyền:

```bash
cd build
dd if=/dev/zero of=test_file.txt bs=1M count=10   # Tạo file 10MB
```

---

### Bước 3: Chạy Server (Terminal 1)

Khởi động server lắng nghe trên cổng 5001:

```bash
cd /home/cuong/Socket/build
./server_app
```

**Output mong đợi:**
```
[*] Server đang lắng nghe trên cổng 5001...
[*] Chờ client kết nối...
```

---

### Bước 4: Chạy Client (Terminal 2)

Khởi động client để gửi file:

```bash
cd /home/cuong/Socket/build
./client_app
```

**Output mong đợi:**
```
[+] Đã kết nối tới Server!
[*] Gửi tên file: test_file.txt
[*] Server yêu cầu bắt đầu gửi từ byte: 0
[*] Đang gửi...
Đã gửi: 10485760 bytes
[*] Truyền tải kết thúc.
```

**Trên Server:**
```
[+] Client đã kết nối!
[*] Nhận tên file: test_file.txt
[*] Offset hiện tại: 0 bytes
[*] Bắt đầu nhận dữ liệu...
[+] Truyền tải thành công!
[*] File được lưu: received_test_file.txt
```

---

### Bước 5: Test Tính Năng Resume (Điểm Chính)

**Mục đích:** Kiểm tra khả năng tiếp tục truyền sau khi bị ngắt kết nối

**Quy trình:**

1. **Chạy client, sau đó ngắt giữa quá trình:**
   - Khởi động client (Terminal 2)
   - Chờ vài giây, bấm **Ctrl+C** để ngắt kết nối
   - Quan sát: Server phát hiện client ngắt và quay lại chờ client mới

2. **Chạy client lại:**
   - Gõ `./client_app` lại trong Terminal 2
   - **Kết quả mong đợi:**
     ```
     [*] Server yêu cầu bắt đầu gửi từ byte: 5242880
     ```
     (Nếu đã truyền 5MB, client sẽ được yêu cầu tiếp tục từ byte 5242880, không phải từ 0)

3. **File được nhận đầy đủ:**
   - Trong thư mục `build`, kiểm tra file `received_test_file.txt`
   - Kích thước: 10485760 bytes (đầy đủ)
   - ✅ Truyền hoàn thành mà **không cần gửi lại từ đầu**

---

## 📁 Cấu Trúc Dự Án

```
Socket/
├── CMakeLists.txt          # Cấu hình CMake
├── README.md               # Tài liệu này
├── include/                # Thư mục header
│   ├── Config.hpp
│   ├── Network.hpp
│   └── Transfer.hpp
├── src/                    # Thư mục source
│   ├── server.cpp
│   ├── client.cpp
│   ├── Network.cpp
│   └── Transfer.cpp
└── build/                  # Thư mục build (được tạo sau khi chạy cmake)
    ├── server_app          # Executable server
    ├── client_app          # Executable client
    └── received_*.txt      # File nhận được
```