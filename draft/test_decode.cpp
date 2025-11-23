#include <QCoreApplication>
#include <QByteArray>
#include <QDebug>
#include <Protocol.h>

void printHex(const QString& label, const QByteArray& data) {
    qDebug().noquote() << label << data.toHex(' ');
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "========================================";
    qDebug() << "   Protocol::decodeMessage() 测试";
    qDebug() << "========================================\n";

    // ========== 测试1: 完整的单条消息 ==========
    qDebug() << "\n【测试1】解码完整的单条消息";
    qDebug() << "--------------------";

    // 先编码一个消息
    Message msg1(MessageType::LoginRequest, "alice:password123");
    QByteArray encoded1 = Protocol::encodeMessage(msg1);

    qDebug() << "编码后的数据:";
    printHex("  ", encoded1);
    qDebug() << "  总长度:" << encoded1.size() << "字节";

    // 解码
    QByteArray buffer1 = encoded1;
    Message decoded1;
    bool success1 = Protocol::decodeMessage(buffer1, decoded1);

    if (success1) {
        qDebug() << "\n✅ 解码成功！";
        qDebug() << "  类型:" << static_cast<int>(decoded1.type);
        qDebug() << "  Payload:" << QString::fromUtf8(decoded1.payload);
        qDebug() << "  Buffer剩余:" << buffer1.size() << "字节";

        if (decoded1.type == msg1.type && decoded1.payload == msg1.payload) {
            qDebug() << "✅ 测试1通过：编解码一致！";
        } else {
            qDebug() << "❌ 测试1失败：数据不匹配";
        }
    } else {
        qDebug() << "❌ 测试1失败：解码失败";
    }

    // ========== 测试2: 数据不完整（只有3字节）==========
    qDebug() << "\n【测试2】数据不完整（只有3字节）";
    qDebug() << "--------------------";

    QByteArray buffer2 = encoded1.left(3);  // 只取前3字节
    qDebug() << "Buffer内容:";
    printHex("  ", buffer2);
    qDebug() << "  长度:" << buffer2.size() << "字节（需要至少5字节）";

    Message decoded2;
    bool success2 = Protocol::decodeMessage(buffer2, decoded2);

    if (!success2) {
        qDebug() << "✅ 测试2通过：正确返回false（数据不完整）";
        qDebug() << "  Buffer保持不变:" << buffer2.size() << "字节";
    } else {
        qDebug() << "❌ 测试2失败：不应该解码成功";
    }

    // ========== 测试3: Payload不完整 ==========
    qDebug() << "\n【测试3】Payload不完整";
    qDebug() << "--------------------";

    QByteArray buffer3 = encoded1.left(10);  // 头部5字节+部分Payload
    qDebug() << "Buffer内容:";
    printHex("  ", buffer3);
    qDebug() << "  长度:" << buffer3.size() << "字节（完整需要" << encoded1.size() << "字节）";

    Message decoded3;
    bool success3 = Protocol::decodeMessage(buffer3, decoded3);

    if (!success3) {
        qDebug() << "✅ 测试3通过：正确返回false（Payload不完整）";
        qDebug() << "  Buffer保持不变:" << buffer3.size() << "字节";
    } else {
        qDebug() << "❌ 测试3失败：不应该解码成功";
    }

    // ========== 测试4: 粘包（两条消息） ==========
    qDebug() << "\n【测试4】粘包（两条消息）";
    qDebug() << "--------------------";

    Message msg4a(MessageType::EchoRequest, "Hello");
    Message msg4b(MessageType::Ack, "OK");

    QByteArray buffer4 = Protocol::encodeMessage(msg4a);
    buffer4.append(Protocol::encodeMessage(msg4b));

    qDebug() << "Buffer包含2条消息:";
    printHex("  ", buffer4);
    qDebug() << "  总长度:" << buffer4.size() << "字节";

    // 解码第1条
    Message decoded4a;
    bool success4a = Protocol::decodeMessage(buffer4, decoded4a);

    if (success4a) {
        qDebug() << "\n✅ 第1条消息解码成功";
        qDebug() << "  类型:" << static_cast<int>(decoded4a.type);
        qDebug() << "  Payload:" << QString::fromUtf8(decoded4a.payload);
        qDebug() << "  Buffer剩余:" << buffer4.size() << "字节";
    } else {
        qDebug() << "❌ 第1条消息解码失败";
    }

    // 解码第2条
    Message decoded4b;
    bool success4b = Protocol::decodeMessage(buffer4, decoded4b);

    if (success4b) {
        qDebug() << "\n✅ 第2条消息解码成功";
        qDebug() << "  类型:" << static_cast<int>(decoded4b.type);
        qDebug() << "  Payload:" << QString::fromUtf8(decoded4b.payload);
        qDebug() << "  Buffer剩余:" << buffer4.size() << "字节";
    } else {
        qDebug() << "❌ 第2条消息解码失败";
    }

    if (success4a && success4b &&
        decoded4a.payload == msg4a.payload &&
        decoded4b.payload == msg4b.payload) {
        qDebug() << "✅ 测试4通过：粘包处理正确！";
    } else {
        qDebug() << "❌ 测试4失败";
    }

    // ========== 测试5: 分批接收数据 ==========
    qDebug() << "\n【测试5】模拟分批接收数据";
    qDebug() << "--------------------";

    Message msg5(MessageType::FileUploadChunk, "FileData12345");
    QByteArray fullMessage = Protocol::encodeMessage(msg5);

    qDebug() << "完整消息:" << fullMessage.size() << "字节";

    QByteArray buffer5;

    // 第1批：只收到3字节
    buffer5.append(fullMessage.left(3));
    qDebug() << "\n第1批收到 3 字节";
    Message decoded5;
    if (!Protocol::decodeMessage(buffer5, decoded5)) {
        qDebug() << "  ✅ 正确返回false（不够）";
    }

    // 第2批：再收到5字节
    buffer5.append(fullMessage.mid(3, 5));
    qDebug() << "\n第2批收到 5 字节，累计" << buffer5.size() << "字节";
    if (!Protocol::decodeMessage(buffer5, decoded5)) {
        qDebug() << "  ✅ 正确返回false（还不够）";
    }

    // 第3批：收到剩余部分
    buffer5.append(fullMessage.mid(8));
    qDebug() << "\n第3批收到剩余数据，累计" << buffer5.size() << "字节";
    if (Protocol::decodeMessage(buffer5, decoded5)) {
        qDebug() << "  ✅ 成功解码！";
        qDebug() << "  Payload:" << QString::fromUtf8(decoded5.payload);
        if (decoded5.payload == msg5.payload) {
            qDebug() << "✅ 测试5通过：分批接收处理正确！";
        }
    } else {
        qDebug() << "  ❌ 应该能解码了";
    }

    // ========== 总结 ==========
    qDebug() << "\n========================================";
    qDebug() << "   测试完成！";
    qDebug() << "========================================\n";

    return 0;
}
