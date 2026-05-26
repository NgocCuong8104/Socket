#include "../include/Config.hpp"
#include "../include/Network.hpp"
#include "../include/Transfer.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>

int main() {
    int server_fd = init_server_socket(PORT);
    std::cout << "[*] Server đang lắng nghe trên cổng " << PORT << "...\n";

    while (true) {
        int client_socket = accept(server_fd, nullptr, nullptr);
        if (client_socket < 0) continue;
        
        std::cout << "\n[+] Client đã kết nối.\n";
        
        // Gọi hàm của bạn!
        receiveFileWithResume(client_socket);
        
        close(client_socket);
    }
    return 0;
}