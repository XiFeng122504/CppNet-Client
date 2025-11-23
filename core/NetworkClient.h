#ifndef _NET_WORK_CLIENT_H_
#define _NET_WORK_CLIENT_H_

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "Protocol.h"  // 需要完整的 Message 定义

  class NetworkClient : public QObject {
      Q_OBJECT

  public:
      // 构造函数
      NetworkClient(QObject* parent = nullptr);

      // 公共方法
      void connectToServer(const QString& host, quint16 port);
      void disconnectFromServer();
      void sendMessage(const Message& message);

  signals:
      // 信号：通知外部发生的事件
      void connected();                        // 连接成功
      void disconnected();                     // 断开连接
      void messageReceived(const Message& msg); // 收到消息
      void errorOccurred(const QString& error); // 发生错误

  private slots:
      // 内部槽：响应QTcpSocket的信号
      void onSocketConnected();     // socket连接成功时
      void onSocketDisconnected();  // socket断开时
      void onSocketReadyRead();     // socket有数据可读时
      void onSocketError(QAbstractSocket::SocketError socketError);  // socket发生错误时（Qt 5.12使用error信号）

  private:
      QTcpSocket* socket;
      QByteArray receiveBuffer;  // 累积接收的数据（处理分包）
  };

#endif//_NET_WORK_CLIENT_H_


