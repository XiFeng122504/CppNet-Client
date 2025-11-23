cmake_minimum_required(VERSION 3.10)
project(ProtocolTests)

# 设置架构为x86_64（适配Qt 5.12）
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)

# C++标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt相关设置
set(CMAKE_AUTOMOC ON)

# 查找Qt5组件
find_package(Qt5 REQUIRED COMPONENTS Core Network)

# 包含目录（包含core目录、services目录）
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../core
    ${CMAKE_CURRENT_SOURCE_DIR}/../services
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# 测试程序：test_protocol
add_executable(test_protocol
    test_protocol.cpp
    ../core/Protocol.cpp  # 包含Protocol的实现
)

target_link_libraries(test_protocol
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_decode
add_executable(test_decode
    test_decode.cpp
    ../core/Protocol.cpp
)

target_link_libraries(test_decode
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_socket
add_executable(test_socket
    test_socket.cpp
)

target_link_libraries(test_socket
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_server
add_executable(test_server
    test_server.cpp
)

target_link_libraries(test_server
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_network_client
add_executable(test_network_client
    test_network_client.cpp
    ../core/NetworkClient.cpp
    ../core/Protocol.cpp
)

target_link_libraries(test_network_client
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_EchoService
add_executable(test_EchoService
    test_EchoService.cpp
    ../services/EchoService.cpp
    ../core/NetworkClient.cpp
    ../core/Protocol.cpp
)

target_link_libraries(test_EchoService
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_AuthService
add_executable(test_AuthService
    test_AuthService.cpp
    ../services/AuthService.cpp
    ../core/NetworkClient.cpp
    ../core/Protocol.cpp
)

target_link_libraries(test_AuthService
    Qt5::Core
    Qt5::Network
)

# 测试程序：test_FileUploadService
add_executable(test_FileUploadService
    test_FileUploadService.cpp
    ../services/FileUploadService.cpp
    ../core/NetworkClient.cpp
    ../core/Protocol.cpp
)

target_link_libraries(test_FileUploadService
    Qt5::Core
    Qt5::Network
)

# 打印配置信息
message(STATUS "=== Protocol Test Configuration ===")
message(STATUS "Qt5 version: ${Qt5_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
