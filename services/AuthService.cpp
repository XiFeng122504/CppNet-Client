#include "AuthService.h"
#include "Protocol.h"
#include "NetworkClient.h"


AuthService::AuthService(NetworkClient* client, QObject* parent):QObject(parent),m_pClient(client){
    //  链接信号和槽函数
    connect(m_pClient,&NetworkClient::messageReceived,this,&AuthService::onMessageReceived);
}



void AuthService::tryLogin(const QString &username, const QString& password){
    Message msg;

    msg = Protocol::createLoginMessage(username,password);

    m_pClient->sendMessage(msg);
}


void AuthService::onMessageReceived(const Message& msg){
    if(msg.type == MessageType::Ack){
        emit loginSuccess();
    }
    else if(msg.type == MessageType::Error){
        emit loginFailed(QString::fromUtf8(msg.payload));
    }
}
