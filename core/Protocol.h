#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QByteArray>
#include <QString>
#include <cstdint>

// 消息类型枚举（和服务器保持一致）
enum class MessageType : quint8 {
    LoginRequest = 1,      // 登录请求
    EchoRequest = 2,       // Echo测试
    FileUploadBegin = 3,   // 开始上传文件
    FileUploadChunk = 4,   // 文件数据块
    FileUploadEnd = 5,     // 完成上传
    Ack = 100,            // 成功响应
    Error = 101           // 错误响应
};

// 消息结构
struct Message {
    MessageType type;      // 消息类型
    QByteArray payload;    // 消息内容（可以是任何数据）

    // 构造函数
    Message() : type(MessageType::EchoRequest) {}
    Message(MessageType t, const QByteArray& p) : type(t), payload(p) {}
};

// Protocol类 - 负责编码和解码消息
class Protocol {
public:
    // ========== 核心功能 ==========

    // 功能1: 把Message打包成可以发送的字节流
    // 例如: Message{LoginRequest, "alice:password123"}
    //       → [1][0][0][0][18][alice:password123...]
    static QByteArray encodeMessage(const Message& message);

    // 功能2: 从接收到的字节流中提取完整的Message
    // 例如: [1][0][0][0][18][alice:password123...]
    //       → Message{LoginRequest, "alice:password123"}
    static bool decodeMessage(QByteArray& buffer, Message& outMessage);

    // 功能3: 辅助函数 - 快速创建登录消息
    static Message createLoginMessage(const QString& username, const QString& password);

    // 功能4: 辅助函数 - 从响应消息中提取文本
    static QString getPayloadAsString(const Message& message);

private:
    // 协议头大小: 1字节类型 + 4字节长度 = 5字节
    static constexpr int HEADER_SIZE = 5;
};

#endif // PROTOCOL_H