#include <QCoreApplication>
#include <QDebug>
#include "NetworkClient.h"
#include "EchoService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "=== EchoService 测试程序 ===\n";

    // 创建 NetworkClient 和 EchoService
    NetworkClient* client = new NetworkClient(&app);
    EchoService* echoService = new EchoService(client, &app);

    // 1. 连接成功时 - 通过 EchoService 发送消息
    QObject::connect(client, &NetworkClient::connected, [echoService]() {
        qDebug() << "\n[测试] ✅ 连接成功！";
        qDebug() << "[测试] 通过 EchoService 发送消息...\n";

        // ✅ 使用 EchoService，不需要手动创建 Message！
        echoService->sendEcho("Hello from EchoService!");
    });

    // 2. 收到 Echo 响应时 - 验证并断开连接
    QObject::connect(echoService, &EchoService::echoReceived,
                     [client](const QString& response) {
        qDebug() << "\n[测试] 📩 收到 Echo 响应！";
        qDebug() << "[测试] 内容:" << response;

        // 验证内容
        if (response == "Hello from EchoService!") {
            qDebug() << "[测试] ✅ 内容正确！";
        } else {
            qDebug() << "[测试] ⚠️  内容不匹配！";
        }

        // 收到响应后，断开连接
        qDebug() << "[测试] 准备断开连接...\n";
        client->disconnectFromServer();
    });

    // 3. 断开连接时 - 退出程序
    QObject::connect(client, &NetworkClient::disconnected, [&app]() {
        qDebug() << "\n[测试] ❌ 已断开连接";
        qDebug() << "[测试] 测试完成！退出程序...\n";
        app.quit();
    });

    // 4. 发生错误时 - 打印错误并退出
    QObject::connect(client, &NetworkClient::errorOccurred,
                     [&app](const QString& error) {
        qDebug() << "\n[测试] ⚠️  发生错误:" << error;
        qDebug() << "[测试] 退出程序...\n";
        app.quit();
    });

    // 发起连接
    qDebug() << "[测试] 正在连接到 127.0.0.1:8080 ...";
    client->connectToServer("127.0.0.1", 8080);

    // 进入事件循环
    return app.exec();
}
