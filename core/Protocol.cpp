#include <QByteArray>
#include <Protocol.h>
#include <QDataStream>

/// @brief 编码消息，将[命令类型][命令长度][具体命令]编成一个Byte数组进行传输。
/// @param message Message结构体，包含[命令类型][命令长度][具体命令]这些内容。
/// @return QByteArray的encoded。
QByteArray Protocol::encodeMessage(const Message& message)
{
    QByteArray encoded;
    QDataStream stream(&encoded,QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);

    quint8 message_type = static_cast<quint8>(message.type);
    stream << message_type;

    quint32 message_payload_size = static_cast<quint32>(message.payload.size());
    stream << message_payload_size;

    encoded.append(message.payload);

    return encoded;
}

bool Protocol::decodeMessage(QByteArray& buffer, Message& outMessage){
    // 检查：至少需要HEADER_SIZE字节头部
    if(buffer.size() < HEADER_SIZE){
        return false;
    }

    //  使用Stream读数据：
    QDataStream stream(buffer);
    stream.setByteOrder(QDataStream::BigEndian);

    quint8 type;
    stream >> type;
    
    quint32 payLoadLength;
    stream >> payLoadLength;
    if((payLoadLength + 5) > buffer.size()){
        // 协议层通常不输出日志。。。
        // perror("[error]: 信息长度和记录长度不匹配，可能未发完整。");
        return false;
    }
    
    QByteArray payload = buffer.mid(5,payLoadLength);
    
    outMessage.type = static_cast<MessageType>(type);
    outMessage.payload = payload;


    buffer.remove(0,payLoadLength+5);

    return true;
}

Message Protocol::createLoginMessage(const QString& username, const QString& password){
    
    Message result;

    //  协议层要做好协议层的事情，尽量不要去做多余的事，例如合法性检查就是应该界面层做的事情。
    // //  合法性检查。
    // //  先将username 和 password转成String，好操作
    // std::string username_str = username.toStdString();
    // std::string password_str = password.toStdString();

    // qint16 i = 0;
    // qint16 j = 0;

    // while (i<username_str.size()) if(username_str[i]>127||username_str[i++]<0) return nullptr;
    // while (j<password_str.size()) if(password_str[i]>127||password_str[i++]<0) return nullptr;

    // //  合法性检查通过

    result.type = MessageType::LoginRequest;

    QString total = username + ":" + password;


    result.payload = total.toUtf8();

    return result;

}



QString Protocol::getPayloadAsString(const Message& message){
    return QString::fromUtf8(message.payload);
}