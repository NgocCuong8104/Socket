#include "../include/Config.hpp"
#include "../include/Network.hpp"
#include "../include/Transfer.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

int main() {
    int sock = connect_to_server("127.0.0.1", PORT);
    if (sock < 0) {
        cerr << "[-] Không thể kết nối tới Server. Đã bật Server chưa?\n";
        return -1;
    }
    
    cout << "[+] Kết nối thành công.\n";
    
    string filepath;
    cout << "Nhập đường dẫn file cần gửi (ví dụ: test_data.txt): ";
    getline(cin, filepath);

   if (!filepath.empty() && filepath.front() == '"' && filepath.back() == '"') {
        filepath = filepath.substr(1, filepath.length() - 2);
    }
    if (!filepath.empty() && filepath.front() == '\'' && filepath.back() == '\'') {
        filepath = filepath.substr(1, filepath.length() - 2);
    }

    // Gọi hàm của bạn! (Nhớ tạo một file tên data.txt hoặc mp4 để test)
    sendFileWithResume(sock, filepath);
    
    close(sock);
    return 0;
}