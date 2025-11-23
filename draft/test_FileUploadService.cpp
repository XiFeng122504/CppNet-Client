#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "NetworkClient.h"
#include "FileUploadService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    qDebug() << "=== FileUploadService 测试程序 ===\n";

    // 创建一个测试文件
    QString testFilePath = "test_upload.txt";
    QFile testFile(testFilePath);
    if (testFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&testFile);
        out << "这是一个测试文件，用于测试 FileUploadService\n";
        out << "包含一些中文和英文内容 - Test content\n";
        for(int i = 0; i < 100; i++) {
            out << "Line " << i << ": Some test data to make the file larger\n";
        }
        testFile.close();
        qDebug() << "[准备] ✅ 已创建测试文件:" << testFilePath;
    } else {
        qDebug() << "[准备] ❌ 无法创建测试文件";
        return 1;
    }

    NetworkClient* client = new NetworkClient(&app);
    FileUploadService* uploadService = new FileUploadService(client, &app);

    // 连接信号
    QObject::connect(client, &NetworkClient::connected, [uploadService, testFilePath]() {
        qDebug() << "\n[测试] ✅ 已连接到服务器";
        qDebug() << "[测试] 开始上传文件...\n";
        uploadService->uploadFile(testFilePath);
    });

    QObject::connect(uploadService, &FileUploadService::uploadStarted,
                     [](const QString& filename) {
        qDebug() << "\n[上传] 📤 开始上传:" << filename;
    });

    QObject::connect(uploadService, &FileUploadService::uploadProgress,
                     [](qint64 current, qint64 total) {
        double percentage = (current * 100.0) / total;
        qDebug() << QString("[上传] 📊 进度: %1/%2 字节 (%3%)")
                    .arg(current).arg(total).arg(percentage, 0, 'f', 2);
    });

    QObject::connect(uploadService, &FileUploadService::uploadFinished,
                     [client, testFilePath]() {
        qDebug() << "\n[上传] 🎉 上传完成！";
        qDebug() << "[测试] 准备断开连接...\n";

        // 删除测试文件
        QFile::remove(testFilePath);
        qDebug() << "[清理] 已删除测试文件\n";

        client->disconnectFromServer();
    });

    QObject::connect(uploadService, &FileUploadService::uploadFailed,
                     [client, testFilePath](const QString& error) {
        qDebug() << "\n[上传] ❌ 上传失败！";
        qDebug() << "[上传] 错误信息:" << error;

        // 删除测试文件
        QFile::remove(testFilePath);
        qDebug() << "[清理] 已删除测试文件\n";

        client->disconnectFromServer();
    });

    QObject::connect(client, &NetworkClient::disconnected, [&app]() {
        qDebug() << "\n[测试] 🔌 已断开连接";
        qDebug() << "[测试] 测试完成！退出程序...\n";
        app.quit();
    });

    QObject::connect(client, &NetworkClient::errorOccurred,
                     [&app, testFilePath](const QString& error) {
        qDebug() << "\n[测试] ⚠️  网络错误:" << error;

        // 删除测试文件
        QFile::remove(testFilePath);
        qDebug() << "[清理] 已删除测试文件\n";

        qDebug() << "[测试] 退出程序...\n";
        app.quit();
    });

    qDebug() << "[测试] 正在连接到 127.0.0.1:8080 ...";
    client->connectToServer("127.0.0.1", 8080);

    return app.exec();
}
