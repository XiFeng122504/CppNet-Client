#include "EchoService.h"
#include "NetworkClient.h"
#include "Protocol.h"
#include <QString>

EchoService::EchoService(NetworkClient* newClient, QObject *parent)
: m_pClient(newClient),QObject(parent){
    connect(m_pClient,&NetworkClient::messageReceived,this,&EchoService::onMessageReceived);
}

void EchoService::sendEcho(const QString& text){
    if (!text.isEmpty())
    {
        Message message;
        message.type = MessageType::EchoRequest;
        message.payload = text.toUtf8();

        m_pClient->sendMessage(message);
    }
}

void EchoService::onMessageReceived(const Message& msg){
    if (msg.type==MessageType::EchoRequest)
    {
        QString result = QString::fromUtf8(msg.payload);
        emit echoReceived(result);
    }
    
} 