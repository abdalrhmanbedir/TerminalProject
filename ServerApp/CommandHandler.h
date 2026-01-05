#ifndef COMMANDHANDLER_H
#define COMMANDHANDLER_H

#pragma once
#include <QString>
#include <QByteArray>

struct Command;
class ClientSession;
class FileService;

class CommandHandler {
public:
    explicit CommandHandler(FileService* fs);

    QByteArray handle(ClientSession& session, const Command& cmd);

private:
    FileService* m_fs;

    QByteArray handleAuth(ClientSession& session, const Command& cmd);
    QByteArray handleCreate(ClientSession& session, const Command& cmd);
    QByteArray handleWrite(ClientSession& session, const Command& cmd);
    QByteArray handleAppend(ClientSession& session, const Command& cmd);
    QByteArray handleRead(ClientSession& session, const Command& cmd);
    QByteArray handleDelete(ClientSession& session, const Command& cmd);
    QByteArray handleRename(ClientSession& session, const Command& cmd);
    QByteArray handleList(ClientSession& session, const Command& cmd);
    QByteArray handleInfo(ClientSession& session, const Command& cmd);

    static QByteArray okLine(const QByteArray& msg = {});
    static QByteArray errorLine(int code, const QByteArray& msg);
};


#endif // COMMANDHANDLER_H
