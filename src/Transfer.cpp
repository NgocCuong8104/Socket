#include "../include/Transfer.hpp"
#include "../include/Config.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

// Hàm phụ trợ cho Server: Tính kích thước file hiện tại
static long getExistingFileSize(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        long size = file.tellg();
        file.close();
        return size;
    }
    return 0; // File chưa tồn tại
}

// Hàm phụ trợ cho Client: Tính tổng dung lượng file gốc
static long getTotalFileSize(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        long size = file.tellg();
        file.close();
        return size;
    }
    return -1;
}

void receiveFileWithResume(int client_socket) {
    char buffer[BUFFER_SIZE];
    
    // 1. Nhận tên file từ Client
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(client_socket, buffer, BUFFER_SIZE, 0) <= 0) {
        std::cerr << "[-] Client đã ngắt kết nối trước khi gửi tên file.\n";
        return;
    }
    std::string filename(buffer);
    std::string save_path = "received_" + filename;

    // 2. Tính toán Offset (Kích thước file đang tải dở)
    long offset = getExistingFileSize(save_path);
    std::cout << "[Server] Yêu cầu file: " << filename << "\n";
    std::cout << "[Server] Đã có: " << offset << " bytes. Yêu cầu Client gửi tiếp...\n";

    // 3. Gửi Offset lại cho Client
    std::string offset_str = std::to_string(offset);
    send(client_socket, offset_str.c_str(), offset_str.length(), 0);

    // 4. Mở file để GHI NỐI TIẾP (ios::app)
    std::ofstream outfile(save_path, std::ios::binary | std::ios::app);
    if (!outfile.is_open()) {
        std::cerr << "[-] Lỗi: Không thể mở/tạo file để lưu!\n";
        return;
    }

    // 5. Liên tục nhận dữ liệu
    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        outfile.write(buffer, bytes_received);
    }

    std::cout << "[Server] Hoàn tất nhận file hoặc Client vừa ngắt kết nối.\n";
    outfile.close();
}

void sendFileWithResume(int server_socket, const std::string& filepath) {
    char buffer[BUFFER_SIZE];

    // 1. Gửi tên file cho Server (chỉ lấy tên, bỏ qua đường dẫn thư mục)
    std::string filename = filepath.substr(filepath.find_last_of("/\\") + 1);
    send(server_socket, filename.c_str(), filename.length(), 0);

    // 2. Nhận Offset từ Server
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(server_socket, buffer, BUFFER_SIZE, 0) <= 0) {
        std::cerr << "[-] Lỗi: Không nhận được phản hồi Offset từ Server.\n";
        return;
    }
    long offset = std::stol(buffer);

    long total_size = getTotalFileSize(filepath);
    if (total_size == -1) {
        std::cerr << "[-] Lỗi: Không tìm thấy file gốc: " << filepath << "\n";
        return;
    }
    if (offset >= total_size) {
        std::cout << "[Client] Server đã có đủ 100% file này (" << total_size << " bytes). Không cần gửi lại!\n";
        return;
    }

    std::cout << "[Client] Tổng dung lượng: " << total_size << " bytes.\n";
    std::cout << "[Client] Server đã có: " << offset << " bytes. Bắt đầu Resume...\n";

    std::ifstream infile(filepath, std::ios::binary);
    infile.seekg(offset, std::ios::beg);

    std::cout << "[Client] Đang truyền tải... (Nhấn Ctrl+C để giả lập đứt mạng)\n";
    
    // 5. Gửi phần còn lại
    long total_sent = offset;
    while (infile.read(buffer, BUFFER_SIZE) || infile.gcount() > 0) {
        int bytes_to_send = infile.gcount();
        int sent = send(server_socket, buffer, bytes_to_send, 0);
        
        if (sent < 0) {
            std::cerr << "\n[-] Đứt kết nối mạng! Tiến trình tạm dừng tại: " << total_sent << " bytes\n";
            break;
        }

        total_sent += sent;

        std::cout << "\r[Client] Tiến độ: " << total_sent << " / " << total_size << " bytes" << std::flush;
        
        usleep(50000); 
    }

    if (total_sent == total_size) {
        std::cout << "\n[Client] Truyền tải thành công 100%!\n";
    }
    infile.close();
}