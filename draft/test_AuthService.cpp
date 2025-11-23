#include <QCoreApplication>
#include <QDebug>
#include "NetworkClient.h"
#include "AuthService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "=== AuthService 测试程序 ===\n";

    // 创建 NetworkClient 和 AuthService
    NetworkClient* client = new NetworkClient(&app);
    AuthService* authService = new AuthService(client, &app);

    // 1. 连接成功时 - 尝试登录（使用错误的密码测试失败情况）
    QObject::connect(client, &NetworkClient::connected, [authService]() {
        qDebug() << "\n[测试] ✅ 连接成功！";
        qDebug() << "[测试] 尝试登录（错误密码）...\n";

        // 测试登录失败的情况
        authService->tryLogin("testuser", "wrongpassword");
    });

    // 2. 登录成功时
    QObject::connect(authService, &AuthService::loginSuccess, [client]() {
        qDebug() << "\n[测试] 🎉 登录成功！";
        qDebug() << "[测试] 准备断开连接...\n";
        client->disconnectFromServer();
    });

    // 3. 登录失败时
    QObject::connect(authService, &AuthService::loginFailed,
                     [client](const QString& errorMessage) {
        qDebug() << "\n[测试] ❌ 登录失败！";
        qDebug() << "[测试] 错误信息:" << errorMessage;
        qDebug() << "[测试] 准备断开连接...\n";
        client->disconnectFromServer();
    });

    // 4. 断开连接时 - 退出程序
    QObject::connect(client, &NetworkClient::disconnected, [&app]() {
        qDebug() << "\n[测试] 🔌 已断开连接";
        qDebug() << "[测试] 测试完成！退出程序...\n";
        app.quit();
    });

    // 5. 发生网络错误时 - 打印错误并退出
    QObject::connect(client, &NetworkClient::errorOccurred,
                     [&app](const QString& error) {
        qDebug() << "\n[测试] ⚠️  网络错误:" << error;
        qDebug() << "[测试] 退出程序...\n";
        app.quit();
    });

    // 发起连接
    qDebug() << "[测试] 正在连接到 127.0.0.1:8080 ...";
    client->connectToServer("127.0.0.1", 8080);

    // 进入事件循环
    return app.exec();
}
