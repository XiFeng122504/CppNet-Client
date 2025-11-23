#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "NetworkClient.h"
#include "Protocol.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== NetworkClient 测试程序 ===\n";

    // 创建 NetworkClient
    NetworkClient* client = new NetworkClient(&app);

    // ========== 连接信号 ==========

    // 1. 连接成功时
    QObject::connect(client, &NetworkClient::connected, [client]() {
        qDebug() << "\n[测试] ✅ 连接成功！";
        qDebug() << "[测试] 准备发送测试消息...\n";

        // 创建一条测试消息（简单的 Echo 请求）
        Message msg;
        msg.type = MessageType::EchoRequest;
        msg.payload = QString("Hello from NetworkClient!").toUtf8();

        qDebug() << "[测试] 发送 Echo 消息，内容:" << msg.payload;
        client->sendMessage(msg);
    });

    // 2. 收到消息时
    QObject::connect(client, &NetworkClient::messageReceived, [client](const Message& msg) {
        qDebug() << "\n[测试] 📩 收到服务器响应！";
        qDebug() << "[测试] 消息类型:" << static_cast<int>(msg.type);
        qDebug() << "[测试] 负载大小:" << msg.payload.size();
        qDebug() << "[测试] 负载内容:" << QString::fromUtf8(msg.payload);

        // 收到响应后，延迟1秒再断开（让日志输出完整）
        QTimer::singleShot(1000, [client]() {
            qDebug() << "\n[测试] 准备断开连接...";
            client->disconnectFromServer();
        });
    });

    // 3. 断开连接时
    QObject::connect(client, &NetworkClient::disconnected, [&app]() {
        qDebug() << "\n[测试] ❌ 已断开连接";
        qDebug() << "[测试] 测试完成！退出程序...\n";
        app.quit();
    });

    // 4. 发生错误时
    QObject::connect(client, &NetworkClient::errorOccurred, [&app](const QString& error) {
        qDebug() << "\n[测试] ⚠️  发生错误:" << error;
        qDebug() << "[测试] 退出程序...\n";
        app.quit();
    });

    // ========== 发起连接 ==========
    qDebug() << "[测试] 正在连接到 127.0.0.1:8080 ...";
    client->connectToServer("127.0.0.1", 8080);

    // 进入事件循环
    return app.exec();
}
