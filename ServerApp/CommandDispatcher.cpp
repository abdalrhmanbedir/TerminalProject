#include "CommandDispatcher.h"
#include "CommandHandler.h"
#include "Protocol.h"
#include "ClientSession.h"

CommandDispatcher::CommandDispatcher(CommandHandler* handler)
    : m_handler(handler) {}

QByteArray CommandDispatcher::dispatch(ClientSession& session, const Command& cmd)
{
    return m_handler->handle(session, cmd);
}
