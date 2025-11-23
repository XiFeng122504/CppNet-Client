#ifndef _FILE_UPLOAD_SERVICE_H_
#define _FILE_UPLOAD_SERVICE_H_

#include <QObject>
#include <QString>



class NetworkClient;
class QFile;
struct Message;

enum class UploadState{
    Idle,                       //  空闲
    WaitingBeginAck,            //  等待
    Uploading,                  //  正在上传
    WaitingEndAck               //  等待End的信号
};



class FileUploadService : public QObject
{
    Q_OBJECT

public:
    FileUploadService(NetworkClient* newClient,QObject* parent = nullptr);


    void uploadFile(const QString& filePath);

signals:
    void uploadStarted(const QString& filename);
    void uploadProgress(qint64 currentByte,qint64 totalByte);

    void uploadFinished();
    void uploadFailed(const QString& error);

private slots:
    void onMessageReceived(const Message& msg);



private:
    void sendBegin(const QString& filename);
    void sendNextChunk();
    void sendEnd();
    void cleanUp();


private:
    NetworkClient* m_pClient;
    qint64 m_uploadedBytes;
    qint64 m_nTotalBytes;
    QFile* m_pCurrentFile;

    UploadState m_eCurState;
    const qint64 MAX_DATA_LEN = 64 * 1024;


};


    /* data */


#endif  // _FILE_UPLOAD_SERVICE_H_