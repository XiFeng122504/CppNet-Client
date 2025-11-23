#include <QApplication>
#include <QMessageBox>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    QApplication::setApplicationName("VimUsing Client");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("VimUsing");

    qDebug() << "VimUsing Client starting...";
    qDebug() << "Qt version:" << QT_VERSION_STR;

    // 显示一个简单的消息框测试Qt是否工作
    QMessageBox::information(nullptr,
                            "VimUsing Client",
                            "项目配置成功！\n\nQt版本: " + QString(QT_VERSION_STR));

    return 0;
}


