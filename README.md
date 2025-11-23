# CppNet-Client

基于Qt5的CppNet-Client服务器客户端程序。

## 功能特性

- 用户登录认证
- 文件上传
- 实时消息通信

## 技术栈

- Qt 5.12
- C++17
- CMake 3.10+

## 项目结构

```
client/
├── core/          # 核心模块（协议、网络）
├── services/      # 业务逻辑层
├── models/        # 数据模型
├── ui/            # 用户界面
└── main.cpp       # 程序入口
```

## 构建步骤

### 1. 配置环境变量

```bash
export Qt5_DIR=/path/to/your/qt5.12/xx_64
export PATH=$Qt5_DIR/bin:$PATH
```

### 2. 创建构建目录

```bash
mkdir build
cd build
```

### 3. 运行CMake配置

```bash
cmake ..
```

### 4. 编译项目

```bash
make
```

### 5. 运行程序

```bash
./bin/VimUsingClient
```

## 开发进度

- [x] 项目结构搭建
- [ ] 协议层实现
- [ ] 网络通信层实现
- [ ] 认证服务实现
- [ ] 文件上传服务实现
- [ ] UI界面实现

## 协议说明

VimUsing使用自定义二进制协议：

```
[1字节: MessageType] [4字节: Payload长度] [N字节: Payload]
```

消息类型：
- `1` - LoginRequest
- `2` - EchoRequest
- `3` - FileUploadBegin
- `4` - FileUploadChunk
- `5` - FileUploadEnd
- `100` - Ack
- `101` - Error

## 许可证

GPL-3.0 License
