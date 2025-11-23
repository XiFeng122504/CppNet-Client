#ifndef _AUTH_SERVICE_H_
#define _AUTH_SERVICE_H_
#include <QObject>
#include <QString>

struct Message;
class NetworkClient;

class AuthService:public QObject
{
    Q_OBJECT
private:
    /* data */
public:
    AuthService(NetworkClient * client,QObject* parent = nullptr);

    void tryLogin(const QString &username, const QString &password);


signals:
    // 登录成功
    void loginSuccess();

    // 登录失败
    void loginFailed(const QString& errorMessage);


private slots:
    // 处理 NetworkClient 的 messageReceived信号
    void onMessageReceived(const Message& msg);

private:
    NetworkClient* m_pClient;

};
#endif//AUTH_SERVICE_H_