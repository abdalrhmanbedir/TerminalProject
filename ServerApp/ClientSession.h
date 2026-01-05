#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#pragma once
#include <QThread>
#include <QTcpSocket>
#include <QByteArray>

#include "Protocol.h"

class CommandDispatcher;

class ClientSession : public QThread {
    Q_OBJECT
public:
    ClientSession(QTcpSocket* socket, CommandDispatcher* dispatcher, QObject* parent=nullptr);

    bool isAuthenticated() const { return m_authenticated; }
    void setAuthenticated(bool v) { m_authenticated = v; }

protected:
    void run() override;

private slots:
    void onReadyRead();
    void onDisconnected();

private:
    QTcpSocket* m_socket = nullptr;
    CommandDispatcher* m_dispatcher = nullptr;

    QByteArray m_buffer;
    bool m_authenticated = false;
};


#endif // CLIENTSEASSION_H
