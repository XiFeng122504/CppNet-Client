cmake_minimum_required(VERSION 3.10)
project(VimUsingClient VERSION 1.0.0 LANGUAGES CXX)

# 设置架构为x86_64（适配Qt 5.12）
set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)

# C++标准设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt相关设置
set(CMAKE_AUTOMOC ON)  # 自动处理Qt的moc（元对象编译器）
set(CMAKE_AUTORCC ON)  # 自动处理资源文件
set(CMAKE_AUTOUIC ON)  # 自动处理UI文件

# 查找Qt5组件
find_package(Qt5 REQUIRED COMPONENTS
    Core      # Qt核心模块
    Network   # 网络模块（QTcpSocket）
    Widgets   # 界面组件
)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/services
    ${CMAKE_CURRENT_SOURCE_DIR}/models
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

# 源文件列表（会逐步添加）
set(SOURCES
    main.cpp
    # 其他文件会在实现时添加
)

# 头文件列表
set(HEADERS
    # 头文件会在实现时添加
)

# UI文件列表
set(UI_FILES
    # UI文件会在实现时添加
)

# 创建可执行文件
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Widgets
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 打印配置信息
message(STATUS "=== VimUsing Client Configuration ===")
message(STATUS "Qt5 version: ${Qt5_VERSION}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
