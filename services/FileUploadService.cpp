#include <QFile>

#include "FileUploadService.h"
#include "NetworkClient.h"
#include "Protocol.h"
#include <QFileInfo>
#include <QByteArray>




FileUploadService::FileUploadService(NetworkClient* newClient, QObject* parent)
                                    : QObject(parent),m_pClient(newClient)
{
    m_eCurState = UploadState::Idle;
    connect(m_pClient,&NetworkClient::messageReceived,this,&FileUploadService::onMessageReceived);
}

void FileUploadService::sendBegin(const QString & filename){
    Message msg;
    msg.type = MessageType::FileUploadBegin;
    msg.payload = filename.toUtf8();

    m_pClient->sendMessage(msg);

    //  修改本服务的状态
    m_eCurState = UploadState::WaitingBeginAck;
}

void FileUploadService::sendNextChunk(){
    //  先获取目前发送的是多少个字节
    auto theRemainByte = m_nTotalBytes - m_uploadedBytes;

    QByteArray readedData;
    if(theRemainByte > MAX_DATA_LEN){
        readedData = m_pCurrentFile->read(MAX_DATA_LEN);
        m_uploadedBytes += MAX_DATA_LEN;
        //  实际发送部分
        Message msg;
        msg.payload = readedData;
        msg.type = MessageType::FileUploadChunk;
        m_pClient->sendMessage(msg);

        //  判断是否已经发送完成了。
        m_eCurState = UploadState::Uploading;
        emit uploadProgress(m_uploadedBytes, m_nTotalBytes);
    }
    else{
        sendEnd();
    }
    
    
    
    
    //  然后计算这次要发送多少，看看是否是最后一次，是的话，发送完之后，
    //  再发送一个FileUploadEnd状态 ，修改状态为WaitingEndAck
}

void FileUploadService::sendEnd(){
    auto theRemainByte = m_nTotalBytes - m_uploadedBytes;

    QByteArray readedData;
    readedData = m_pCurrentFile->read(theRemainByte);
    m_uploadedBytes += theRemainByte;
    //  实际发送部分
    Message msg;
    msg.payload = readedData;
    msg.type = MessageType::FileUploadEnd;
    m_pClient->sendMessage(msg);

    m_eCurState = UploadState::WaitingEndAck;
    //  判断是否已经发送完成了。
    emit uploadProgress(m_uploadedBytes, m_nTotalBytes);
    
}

void FileUploadService::cleanUp(){
    if(m_pCurrentFile != nullptr){
        if(m_pCurrentFile->isOpen()){
            m_pCurrentFile->close();
        }
        delete m_pCurrentFile;
        m_pCurrentFile = nullptr;
    }    
    m_eCurState = UploadState::Idle;
}

void FileUploadService::onMessageReceived(const Message& msg){
    //  有多种Ack需要处理
    if(msg.type == MessageType::Ack){

        if(m_eCurState == UploadState::WaitingBeginAck){
            sendNextChunk();
        }

        //  如果还有数据
        if(m_eCurState == UploadState::Uploading){
            sendNextChunk();
        }

        if(m_eCurState == UploadState::WaitingEndAck){
            cleanUp();
            emit uploadFinished();
        }
    }

    //  需要处理错误的部分
    if(msg.type == MessageType::Error){
        cleanUp();
        emit uploadFailed(Protocol::getPayloadAsString(msg));
    }
}

void FileUploadService::uploadFile(const QString& filePath){
    if(m_eCurState != UploadState::Idle){
        emit uploadFailed("已有文件正在上传");
        return;
    }
    //  使用路径打开文件。
    m_pCurrentFile = new QFile(filePath);
    bool openResult = m_pCurrentFile->open(QIODevice::ReadOnly);

    if(!openResult){
        
        emit uploadFailed("无法打开文件！");
        cleanUp();
        return ;
    }
    //  获取文件size
    m_nTotalBytes = m_pCurrentFile->size();

    //  初始化成员变量
    m_uploadedBytes = 0;
    QFileInfo fileInfo(m_pCurrentFile->fileName());
    sendBegin(fileInfo.fileName());
    emit uploadStarted(filePath);   
}



