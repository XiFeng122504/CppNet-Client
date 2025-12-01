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
QtClient/
├── core/          # 核心模块（协议、网络）
├── services/      # 业务逻辑层
├── models/        # 数据模型
├── ui/            # 用户界面
├── draft/         # 测试代码
├── docs/          # 文档（git submodule）
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
- [x] 协议层实现
- [x] 网络通信层实现
- [x] 认证服务实现
- [x] 文件上传服务实现
- [x] Echo服务实现
- [ ] UI界面实现
- [ ] 集成测试

## 协议说明

CppNet 使用自定义二进制协议：

```
[1字节: MessageType] [4字节: Payload长度] [N字节: Payload]
```

消息类型：
- `1` - LoginRequest (客户端→服务器)
- `2` - EchoRequest (客户端→服务器)
- `3` - FileUploadBegin (客户端→服务器)
- `4` - FileUploadChunk (客户端→服务器)
- `5` - FileUploadEnd (客户端→服务器)
- `100` - Ack (服务器→客户端)
- `101` - Error (服务器→客户端)

## 文档

详细的学习文档位于 `docs/QtCSLearn/QtClient/` 目录：
- `AI-GUIDE.md` - AI 助手指导文档
- `LEARNING-LOG.md` - 详细学习日志
- `OVERVIEW.md` - 项目总览

## 许可证

GPL-3.0 License
