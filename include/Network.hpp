#pragma once

int init_server_socket(int port);
int connect_to_server(const char* ip, int port);