#pragma once
#include <string>

// Server gọi hàm này để nhận file và phản hồi Offset
void receiveFileWithResume(int client_socket);

// Client gọi hàm này để gửi file, tự động tua qua Offset
void sendFileWithResume(int server_socket, const std::string& filepath);