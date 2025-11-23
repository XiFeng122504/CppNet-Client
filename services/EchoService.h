#ifndef _ECHO_SERVICE_
#define _ECHO_SERVICE_

#include <QObject>
#include <QString>

class NetworkClient;
struct Message;

class EchoService : public QObject
{
    Q_OBJECT

public:
    // 构造函数接受一个 NetworkClient 指针（parent 有默认值）
    EchoService(NetworkClient* client, QObject* parent = nullptr);

    // 发送 Echo 请求
    void sendEcho(const QString& text);

signals:
    // 收到 Echo 响应（const 引用，不是非 const 引用）
    void echoReceived(const QString& response);

private slots:
    // 处理 NetworkClient 的 messageReceived 信号
    void onMessageReceived(const Message& msg);

private:
    NetworkClient* m_pClient;
};



#endif//_ECHO_SERVICE_