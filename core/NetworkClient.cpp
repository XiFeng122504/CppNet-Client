#include "NetworkClient.h"
#include "Protocol.h"  // 向前声明
#include <QDebug>
#include <QHostAddress>

NetworkClient::NetworkClient(QObject* parent):QObject(parent){
    socket = new QTcpSocket(this);

    //  链接时触发🔗槽函数
    connect(socket,&QTcpSocket::connected,this,&NetworkClient::onSocketConnected);

    //  断开链接的断开槽函数
    connect(socket, &QTcpSocket::disconnected,this,&NetworkClient::onSocketDisconnected);

    //  有数据可读
    connect(socket, &QTcpSocket::readyRead,this,&NetworkClient::onSocketReadyRead);

    connect(socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),this,&NetworkClient::onSocketError);
}


void NetworkClient::connectToServer(const QString& host, quint16 port){
    
    // QHostAddress address;
    // address.setAddress(host);
    //  这种写法也可以，没有问题。
    // socket->connectToHost(address,port);
    socket->connectToHost(host,port);

    // connectToHost 支持什么格式？
    // // 都可以！
    // socket->connectToHost("127.0.0.1", 8080);
    // // IP 地址
    // socket->connectToHost("localhost", 8080);
    // // 主机名
    // socket->connectToHost("example.com", 80);
    // // 域名
}


void NetworkClient::disconnectFromServer(){
    //  看上去不需要指定什么host，或者port。
    socket->disconnectFromHost();
}

void NetworkClient::sendMessage(const Message& message){
    if(socket->state()!=QAbstractSocket::ConnectedState){
        qDebug() << "服务器尚未链接，无法发送消息。";
        return ;
    }
    QByteArray need_send_message = Protocol::encodeMessage(message);

    //  
      qDebug()  << "[NetworkClient] 发送消息，类型:" << static_cast<int>(message.type)
                << "负载大小:" << message.payload.size();
    socket->write(need_send_message);
    socket->flush();
}
//槽函数部分
void NetworkClient::onSocketConnected(){
    qDebug() << "[NetWorkClient] 已链接到服务器";
    emit connected();
}

void NetworkClient::onSocketDisconnected(){
    qDebug() << "[NetWorkClient] 已断开链接";   
    //  需要在这里清空吗？还是在析构的地方清空？两者什么区别？claude看到之后，回答一下
    receiveBuffer.clear();
    emit disconnected();
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError socketError){
    //  如何处理这个错误的socket？
    qDebug() << "[NetWorkClient] 发生错误，" << "代码：" << socketError;
    emit errorOccurred(socket->errorString());
}

void NetworkClient::onSocketReadyRead(){
    //  进入这个回调就说明已经接收到信号了。因此直接开始读数据  
    receiveBuffer.append(socket->readAll());
    Message message;
    while(Protocol::decodeMessage(receiveBuffer, message)){
        
        qDebug() << "解码出消息，类型:" << static_cast<int>(message.type);
        emit messageReceived(message);
    }

    qDebug() << "缓冲区剩余" << receiveBuffer.size() << "字节";
}
