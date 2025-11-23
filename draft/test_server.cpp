#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>

class SimpleServer : public QObject {
    Q_OBJECT

public:
    SimpleServer(quint16 port, QObject* parent = nullptr) : QObject(parent) {
        server = new QTcpServer(this);

        // 当有新连接时，触发 newConnection 信号
        connect(server, &QTcpServer::newConnection, this, &SimpleServer::onNewConnection);

        // 开始监听
        if (server->listen(QHostAddress::Any, port)) {
            qDebug() << "🚀 服务器启动成功！";
            qDebug() << "监听地址:" << server->serverAddress().toString();
            qDebug() << "监听端口:" << server->serverPort();
            qDebug() << "等待客户端连接...\n";
        } else {
            qDebug() << "❌ 服务器启动失败:" << server->errorString();
        }
    }

private slots:
    // 当有新客户端连接时
    void onNewConnection() {
        // 获取新连接的 socket
        QTcpSocket* clientSocket = server->nextPendingConnection();

        qDebug() << "\n✅ 新客户端连接！";
        qDebug() << "客户端地址:" << clientSocket->peerAddress().toString();
        qDebug() << "客户端端口:" << clientSocket->peerPort();

        // 当客户端发送数据时
        connect(clientSocket, &QTcpSocket::readyRead, [clientSocket]() {
            QByteArray data = clientSocket->readAll();
            qDebug() << "\n📩 收到数据 (" << data.size() << "字节):";
            qDebug() << "   十六进制:" << data.toHex(' ');
            qDebug() << "   文本:" << QString::fromUtf8(data);

            // 回声：把收到的数据原样发回
            clientSocket->write(data);
            qDebug() << "📤 已回显" << data.size() << "字节";
        });

        // 当客户端断开连接时
        connect(clientSocket, &QTcpSocket::disconnected, [clientSocket]() {
            qDebug() << "\n👋 客户端断开连接";
            clientSocket->deleteLater();  // 稍后删除 socket 对象
        });

        // 当发生错误时
        connect(clientSocket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
               [clientSocket](QAbstractSocket::SocketError error) {
            qDebug() << "⚠️  客户端错误:" << clientSocket->errorString();
        });
    }

private:
    QTcpServer* server;
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "=== 简单的回声服务器 ===\n";

    // 创建服务器，监听 8080 端口
    SimpleServer server(8080);

    // 进入事件循环
    return app.exec();
}

// 必须包含 moc 生成的代码（因为有 Q_OBJECT 宏）
#include "test_server.moc"
