#include <QCoreApplication>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QDebug>

#include "ClientSession.h"
#include "CommandDispatcher.h"
#include "CommandHandler.h"
#include "FileService.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QDir wd(QDir::current().filePath("data"));
    if (!wd.exists())
        QDir().mkpath(wd.absolutePath());

    FileService fs(wd.absolutePath());
    CommandHandler handler(&fs);
    CommandDispatcher dispatcher(&handler);

    QTcpServer server;
    QObject::connect(&server, &QTcpServer::newConnection, [&](){
        while (server.hasPendingConnections()) {
            QTcpSocket* s = server.nextPendingConnection();
            qDebug() << "Client connected:";

            auto* th = new ClientSession(s, &dispatcher, &app);
            QObject::connect(th, &QThread::finished, th, &QObject::deleteLater);
            th->start();
        }
    });

    if (!server.listen(QHostAddress::Any, 5555)) {
        qCritical() << "Listen failed:" << server.errorString();
        return 1;
    }

    qDebug() << "Server listening on port 5555 local host network";
    return app.exec();
}
