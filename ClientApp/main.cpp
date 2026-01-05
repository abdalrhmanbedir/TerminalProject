#include <QCoreApplication>
#include <QTcpSocket>
#include <QSocketNotifier>
#include <QTextStream>
#include <QDebug>

static void printPrompt()
{
    QTextStream out(stdout);
    out << "> ";
    out.flush();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QTcpSocket sock;

    QObject::connect(&sock, &QTcpSocket::connected, [&](){
        qDebug() << "CLIENT: connected";
        printPrompt();
    });

    QObject::connect(&sock, &QTcpSocket::readyRead, [&](){
        const QByteArray data = sock.readAll();

        QTextStream out(stdout);
        out << "\n" << data;
        out.flush();
        printPrompt();
    });

    QObject::connect(&sock, &QTcpSocket::errorOccurred, [&](QAbstractSocket::SocketError){
        qDebug() << "CLIENT: socket error:" << sock.errorString();
    });

    QObject::connect(&sock, &QTcpSocket::disconnected, [&](){
        qDebug() << "CLIENT: disconnected";
        QCoreApplication::quit();
    });

    sock.connectToHost("127.0.0.1", 5555);

    // stdin -> socket  tus is event based
    QSocketNotifier stdinNotifier(fileno(stdin), QSocketNotifier::Read);
    QObject::connect(&stdinNotifier, &QSocketNotifier::activated, [&](){
        QTextStream in(stdin);
        QString line = in.readLine();
        if (line.isNull()) return;

        line = line.trimmed();
        if (line.isEmpty()) { printPrompt(); return; }

        sock.write(line.toUtf8());
        sock.write("\n");
    });

    return app.exec();
}
