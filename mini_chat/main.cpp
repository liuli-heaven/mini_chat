#include "mainwindow.h"

#include <QApplication>
#include <QFile>
#include "global.h"
#include <QSettings>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //解析配置
    QString app_path = QCoreApplication::applicationDirPath();
    app_path = app_path.left(app_path.lastIndexOf("mini_chat") + 9);
    //拼接文件名
    QString filename = "config.ini";
    QString config_path = QDir::toNativeSeparators(app_path + QDir::separator() + filename);

    qDebug() << config_path;
    QSettings settings(config_path, QSettings::IniFormat);
    QString gate_host = settings.value("GateServer/host").toString();
    QString gate_port = settings.value("GateServer/port").toString();

    gate_url_prefix = "http://" + gate_host + ":" + gate_port;

    QFile qss(":/qss/style/stylesheet.qss");

    if(qss.open(QFile::ReadOnly))
    {
        qDebug("open success");
        QString style = QLatin1String(qss.readAll());
        a.setStyleSheet(style);
        qss.close();
    }
    else
    {
        qDebug("open failed");
        qDebug() << qss.errorString();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
