#ifndef COMMANDDISPATCHER_H
#define COMMANDDISPATCHER_H

#pragma once
#include <QByteArray>

struct Command;
class ClientSession;
class CommandHandler;

class CommandDispatcher {
public:
    explicit CommandDispatcher(CommandHandler* handler);
    QByteArray dispatch(ClientSession& session, const Command& cmd);

private:
    CommandHandler* m_handler;
};


#endif // COMMANDDISPATCHER_H
