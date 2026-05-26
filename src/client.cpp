#include "../include/Config.hpp"
#include "../include/Network.hpp"
#include "../include/Transfer.hpp"
#include <iostream>
#include <unistd.h>

int main() {
    int sock = connect_to_server("127.0.0.1", PORT);
    if (sock < 0) {
        std::cerr << "[-] Không thể kết nối tới Server. Đã bật Server chưa?\n";
        return -1;
    }
    
    std::cout << "[+] Kết nối thành công.\n";
    
    // Gọi hàm của bạn! (Nhớ tạo một file tên data.txt hoặc mp4 để test)
    sendFileWithResume(sock, "test_data.txt");
    
    close(sock);
    return 0;
}