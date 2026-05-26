cmake_minimum_required(VERSION 3.10)

project(SOCKET VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# Tìm thư viện Đa luồng (Threads) của hệ điều hành
find_package(Threads REQUIRED)

add_executable(server_app src/server.cpp src/Network.cpp src/Transfer.cpp)
# Liên kết thư viện luồng cho Server
target_link_libraries(server_app Threads::Threads)

add_executable(client_app src/client.cpp src/Network.cpp src/Transfer.cpp)
# Liên kết thư viện luồng cho Client
target_link_libraries(client_app Threads::Threads)