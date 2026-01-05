#include "ClientSession.h"
#include "CommandDispatcher.h"

ClientSession::ClientSession(QTcpSocket* socket, CommandDispatcher* dispatcher, QObject* parent)
    : QThread(parent), m_socket(socket), m_dispatcher(dispatcher)
{

}

void ClientSession::run()
{

    m_socket->setParent(nullptr);
    m_socket->moveToThread(this);

    connect(m_socket, &QTcpSocket::readyRead, this, &ClientSession::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientSession::onDisconnected);

    exec();
}

void ClientSession::onReadyRead()
{
    m_buffer += m_socket->readAll();

    while (true) {
        int nl = m_buffer.indexOf('\n');
        if (nl < 0) break;

        QByteArray lineBytes = m_buffer.left(nl);
        m_buffer.remove(0, nl + 1);

        const QString line = QString::fromUtf8(lineBytes).trimmed();
        if (line.isEmpty()) continue;

        const Command cmd = Protocol::parseLine(line);
        const QByteArray reply = m_dispatcher->dispatch(*this, cmd);
        m_socket->write(reply);
    }
}

void ClientSession::onDisconnected()
{
    m_socket->deleteLater();
    quit();
}
