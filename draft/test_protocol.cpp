#include <QCoreApplication>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <Protocol.h>  // 使用正式的Protocol.h

// 辅助函数：打印字节数组的16进制表示
void printHex(const QString& label, const QByteArray& data) {
    qDebug().noquote() << label << data.toHex(' ');
}

// 辅助函数：验证编码结果
bool verifyEncoding(const QByteArray& encoded,
                    quint8 expectedType,
                    quint32 expectedLength,
                    const QByteArray& expectedPayload)
{
    qDebug() << "\n=== 验证编码结果 ===";

    // 1. 检查总长度
    int expectedTotalSize = 1 + 4 + expectedPayload.size();  // 类型(1) + 长度(4) + 数据
    if (encoded.size() != expectedTotalSize) {
        qDebug() << "❌ 总长度错误！";
        qDebug() << "  期望:" << expectedTotalSize << "字节";
        qDebug() << "  实际:" << encoded.size() << "字节";
        return false;
    }
    qDebug() << "✅ 总长度正确:" << encoded.size() << "字节";

    // 2. 检查消息类型（第1字节）
    quint8 actualType = static_cast<quint8>(encoded[0]);
    if (actualType != expectedType) {
        qDebug() << "❌ 消息类型错误！";
        qDebug() << "  期望:" << expectedType;
        qDebug() << "  实际:" << actualType;
        return false;
    }
    qDebug() << "✅ 消息类型正确:" << actualType;

    // 3. 检查长度字段（第2-5字节，大端序）
    QByteArray lengthBytes = encoded.mid(1, 4);
    QDataStream lengthStream(lengthBytes);
    lengthStream.setByteOrder(QDataStream::BigEndian);
    quint32 actualLength;
    lengthStream >> actualLength;

    if (actualLength != expectedLength) {
        qDebug() << "❌ 长度字段错误！";
        qDebug() << "  期望:" << expectedLength;
        qDebug() << "  实际:" << actualLength;
        return false;
    }
    qDebug() << "✅ 长度字段正确:" << actualLength;

    // 4. 检查Payload数据（第6字节开始）
    QByteArray actualPayload = encoded.mid(5);
    if (actualPayload != expectedPayload) {
        qDebug() << "❌ Payload数据错误！";
        printHex("  期望:", expectedPayload);
        printHex("  实际:", actualPayload);
        return false;
    }
    qDebug() << "✅ Payload数据正确";

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "========================================";
    qDebug() << "   Protocol::encodeMessage() 测试";
    qDebug() << "========================================\n";

    // ========== 测试1: 登录消息 ==========
    qDebug() << "\n【测试1】登录消息编码";
    qDebug() << "--------------------";

    QByteArray loginPayload = "alice:password123";
    Message loginMsg(MessageType::LoginRequest, loginPayload);

    qDebug() << "输入:";
    qDebug() << "  类型: LoginRequest (1)";
    qDebug() << "  Payload:" << loginPayload;
    qDebug() << "  Payload长度:" << loginPayload.size() << "字节";

    QByteArray loginEncoded = Protocol::encodeMessage(loginMsg);

    qDebug() << "\n编码结果:";
    printHex("  16进制:", loginEncoded);
    qDebug() << "  总长度:" << loginEncoded.size() << "字节";

    // 验证
    bool test1Pass = verifyEncoding(
        loginEncoded,
        1,  // LoginRequest
        loginPayload.size(), // "alice:password123" 长度
        loginPayload
    );

    if (test1Pass) {
        qDebug() << "\n✅ 测试1通过！\n";
    } else {
        qDebug() << "\n❌ 测试1失败！\n";
    }

    // ========== 测试2: Echo消息 ==========
    qDebug() << "\n【测试2】Echo消息编码";
    qDebug() << "--------------------";

    QByteArray echoPayload = "Hello, Server!";
    Message echoMsg(MessageType::EchoRequest, echoPayload);

    qDebug() << "输入:";
    qDebug() << "  类型: EchoRequest (2)";
    qDebug() << "  Payload:" << echoPayload;
    qDebug() << "  Payload长度:" << echoPayload.size() << "字节";

    QByteArray echoEncoded = Protocol::encodeMessage(echoMsg);

    qDebug() << "\n编码结果:";
    printHex("  16进制:", echoEncoded);
    qDebug() << "  总长度:" << echoEncoded.size() << "字节";

    // 验证
    bool test2Pass = verifyEncoding(
        echoEncoded,
        2,  // EchoRequest
        14, // "Hello, Server!" 长度
        echoPayload
    );

    if (test2Pass) {
        qDebug() << "\n✅ 测试2通过！\n";
    } else {
        qDebug() << "\n❌ 测试2失败！\n";
    }

    // ========== 测试3: 空Payload ==========
    qDebug() << "\n【测试3】空Payload编码";
    qDebug() << "--------------------";

    QByteArray emptyPayload = "";
    Message emptyMsg(MessageType::Ack, emptyPayload);

    qDebug() << "输入:";
    qDebug() << "  类型: Ack (100)";
    qDebug() << "  Payload: (空)";
    qDebug() << "  Payload长度:" << emptyPayload.size() << "字节";

    QByteArray emptyEncoded = Protocol::encodeMessage(emptyMsg);

    qDebug() << "\n编码结果:";
    printHex("  16进制:", emptyEncoded);
    qDebug() << "  总长度:" << emptyEncoded.size() << "字节";

    // 验证
    bool test3Pass = verifyEncoding(
        emptyEncoded,
        100,  // Ack
        0,    // 空Payload
        emptyPayload
    );

    if (test3Pass) {
        qDebug() << "\n✅ 测试3通过！\n";
    } else {
        qDebug() << "\n❌ 测试3失败！\n";
    }

    // ========== 测试4: 大Payload（测试quint32） ==========
    qDebug() << "\n【测试4】大Payload编码（1KB数据）";
    qDebug() << "--------------------";

    // 创建1KB的数据
    QByteArray bigPayload(1024, 'X');  // 1024个'X'
    Message bigMsg(MessageType::FileUploadChunk, bigPayload);

    qDebug() << "输入:";
    qDebug() << "  类型: FileUploadChunk (4)";
    qDebug() << "  Payload长度:" << bigPayload.size() << "字节 (1KB)";

    QByteArray bigEncoded = Protocol::encodeMessage(bigMsg);

    qDebug() << "\n编码结果:";
    qDebug() << "  总长度:" << bigEncoded.size() << "字节";
    printHex("  前20字节:", bigEncoded.left(20));

    // 验证
    bool test4Pass = verifyEncoding(
        bigEncoded,
        4,     // FileUploadChunk
        1024,  // 1KB
        bigPayload
    );

    if (test4Pass) {
        qDebug() << "\n✅ 测试4通过！\n";
    } else {
        qDebug() << "\n❌ 测试4失败！\n";
    }

    // ========== 总结 ==========
    qDebug() << "\n========================================";
    qDebug() << "   测试总结";
    qDebug() << "========================================";

    int passCount = test1Pass + test2Pass + test3Pass + test4Pass;
    qDebug() << "通过:" << passCount << "/ 4";

    if (passCount == 4) {
        qDebug() << "\n🎉 所有测试通过！encodeMessage() 实现正确！";
        qDebug() << "\n下一步：实现 decodeMessage()";
    } else {
        qDebug() << "\n⚠️  部分测试失败，请检查 encodeMessage() 实现";
    }

    qDebug() << "========================================\n";

    return 0;
}
