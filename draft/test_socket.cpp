#include <QCoreApplication>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== QTcpSocket 连接测试 ===\n";

    // 1. 创建 socket 对象
    QTcpSocket* socket = new QTcpSocket(&app);

    // 2. 连接信号和槽 - 理解这部分很重要！

    // 当连接成功时，会触发 connected() 信号
    QObject::connect(socket, &QTcpSocket::connected, [&]() {
        qDebug() << "✅ 成功连接到服务器！";
        qDebug() << "本地地址:" << socket->localAddress().toString();
        qDebug() << "本地端口:" << socket->localPort();
        qDebug() << "远程地址:" << socket->peerAddress().toString();
        qDebug() << "远程端口:" << socket->peerPort();

        // 连接成功后，发送一条测试消息
        qDebug() << "\n📤 发送测试消息...";
        QByteArray testData = "Hello from Qt Client!";
        socket->write(testData);
        socket->flush();  // 确保立即发送
    });

    // 当有数据可读时，会触发 readyRead() 信号
    QObject::connect(socket, &QTcpSocket::readyRead, [&]() {
        QByteArray data = socket->readAll();
        qDebug() << "\n📩 收到服务器回显 (" << data.size() << "字节):";
        qDebug() << "   十六进制:" << data.toHex(' ');
        qDebug() << "   文本:" << QString::fromUtf8(data);

        // 收到回显后，断开连接
        qDebug() << "\n👋 断开连接...";
        socket->disconnectFromHost();
    });

    // 当断开连接时，会触发 disconnected() 信号
    QObject::connect(socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "\n❌ 与服务器断开连接";
        app.quit();  // 退出程序
    });

    // 当发生错误时，会触发 error() 信号（Qt 5.12 使用 error，Qt 5.15+ 使用 errorOccurred）
    QObject::connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                    [&](QAbstractSocket::SocketError socketError) {
        qDebug() << "\n⚠️  发生错误:" << socket->errorString();
        qDebug() << "错误代码:" << socketError;
        app.quit();
    });

    // 3. 发起连接
    qDebug() << "正在连接到 127.0.0.1:8080 ...";
    socket->connectToHost("127.0.0.1", 8080);

    // 4. 进入事件循环（类似你的 epoll_wait 循环）
    // Qt 会自动处理所有网络事件，并调用对应的槽函数
    return app.exec();
}
