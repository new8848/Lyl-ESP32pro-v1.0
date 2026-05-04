#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

static void filteredMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (type == QtWarningMsg && msg.contains("_q_startOperation was called more than once")) {
        return;
    }
    qt_message_output(type, context, msg);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(filteredMessageHandler);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
