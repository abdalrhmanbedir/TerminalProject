#include "CommandHandler.h"
#include "Protocol.h"
#include "ClientSession.h"
#include "FileService.h"

#include <QtConcurrent/QtConcurrent>

CommandHandler::CommandHandler(FileService* fs) : m_fs(fs) {}

QByteArray CommandHandler::okLine(const QByteArray& msg)
{
    if (msg.isEmpty()) return "OK\n";
    return "OK " + msg + "\n";
}

QByteArray CommandHandler::errorLine(int code, const QByteArray& msg)
{
    return "ERROR " + QByteArray::number(code) + " " + msg + "\n";
}

QByteArray CommandHandler::handle(ClientSession& session, const Command& cmd)
{
    if (cmd.verb == "AUTH") return handleAuth(session, cmd);

    if (!session.isAuthenticated())
        return errorLine(401, "UNAUTHORIZED");

    if (cmd.verb == "CREATE") return handleCreate(session, cmd);
    if (cmd.verb == "WRITE")  return handleWrite(session, cmd);
    if (cmd.verb == "APPEND") return handleAppend(session, cmd);
    if (cmd.verb == "READ")   return handleRead(session, cmd);
    if (cmd.verb == "DELETE") return handleDelete(session, cmd);
    if (cmd.verb == "RENAME") return handleRename(session, cmd);
    if (cmd.verb == "LIST")   return handleList(session, cmd);
    if (cmd.verb == "INFO")   return handleInfo(session, cmd);

    return errorLine(404, "UNKNOWN COMMAND");
}

QByteArray CommandHandler::handleAuth(ClientSession& session, const Command& cmd)
{
    if (session.isAuthenticated())
        return okLine("AlreadyAuthenticated");

    const QString rest = cmd.rest;
    int comma = rest.indexOf(',');
    if (comma <= 0 || comma == rest.size() - 1)
        return errorLine(400, "BAD_REQUEST");

    const QString user = rest.left(comma).trimmed();
    const QString pass = rest.mid(comma + 1).trimmed();

    if (user == "abdalrhmanmoghazy" && pass == "575859") {
        session.setAuthenticated(true);
        return okLine("Authenticated");
    }
    return errorLine(403, "INVALID_CREDENTIALS");
}


QByteArray CommandHandler::handleCreate(ClientSession&, const Command& cmd)
{
    const QString filename = cmd.rest.trimmed();
    if (filename.isEmpty()) return errorLine(400, "BAD_REQUEST");

    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->createEmpty(filename, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_ALREADY_EXISTS") return errorLine(409, "FILE_ALREADY_EXISTS");
        return errorLine(500, "CANNOT_CREATE");
    }
    return okLine("FILE CREATED SUCCESSFULLY");
}

QByteArray CommandHandler::handleWrite(ClientSession&, const Command& cmd)
{
    const QString rest = cmd.rest;
    int semi = rest.indexOf(';');
    if (semi <= 0) return errorLine(400, "BAD_REQUEST");

    const QString filename = rest.left(semi).trimmed();
    const QByteArray data = rest.mid(semi + 1).toUtf8();

    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->writeAll(filename, data, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        return errorLine(500, "CANNOT_WRITE");
    }
    return okLine("FILE WRITTEN SUCCESSFULLY");
}

QByteArray CommandHandler::handleAppend(ClientSession&, const Command& cmd)
{
    const QString rest = cmd.rest;
    int semi = rest.indexOf(';');
    if (semi <= 0) return errorLine(400, "BAD_REQUEST");

    const QString filename = rest.left(semi).trimmed();
    const QByteArray data = rest.mid(semi + 1).toUtf8();

    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->appendAll(filename, data, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        return errorLine(500, "CANNOT_WRITE");
    }
    return okLine("FILE APPENDED SUCCESSFULLY");
}

QByteArray CommandHandler::handleRead(ClientSession&, const Command& cmd)
{
    const QString filename = cmd.rest.trimmed();
    if (filename.isEmpty()) return errorLine(400, "BAD_REQUEST");

    QByteArray outData;
    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->readAll(filename, outData, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        return errorLine(500, "CANNOT_READ");
    }

    QByteArray out = "OK " + QByteArray::number(outData.size()) + "\n";
    out += outData;
    if (!out.endsWith('\n')) out += "\n";
    return out;
}

QByteArray CommandHandler::handleDelete(ClientSession&, const Command& cmd)
{
    const QString filename = cmd.rest.trimmed();
    if (filename.isEmpty()) return errorLine(400, "BAD_REQUEST");

    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->removeFile(filename, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        return errorLine(500, "CANNOT_DELETE");
    }
    return okLine("FILE DELETED SUCCESSFULLY");
}

QByteArray CommandHandler::handleRename(ClientSession&, const Command& cmd)
{
    const QString rest = cmd.rest;
    int semi = rest.indexOf(';');
    if (semi <= 0 || semi == rest.size() - 1) return errorLine(400, "BAD_REQUEST");

    const QString oldName = rest.left(semi).trimmed();
    const QString newName = rest.mid(semi + 1).trimmed();

    QString err;
    auto f = QtConcurrent::run([&]{ return m_fs->renameFile(oldName, newName, err); });
    const bool ok = f.result();

    if (!ok) {
        if (err == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (err == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        if (err == "FILE_ALREADY_EXISTS") return errorLine(409, "FILE_ALREADY_EXISTS");
        return errorLine(500, "RENAME_FAILED");
    }
    return okLine("FILE RENAMED SUCCESSFULLY");
}

QByteArray CommandHandler::handleList(ClientSession&, const Command&)
{
    QStringList files;
    QString err;

    auto f = QtConcurrent::run([&]{ return m_fs->listFiles(files, err); });
    const bool ok = f.result();

    if (!ok) return errorLine(500, "INTERNAL_ERROR");

    QByteArray out = "OK " + QByteArray::number(files.size()) + "\n";
    for (const QString& fn : files) {
        out += fn.toUtf8();
        out += "\n";
    }
    return out;
}

QByteArray CommandHandler::handleInfo(ClientSession&, const Command& cmd)
{
    const QString filename = cmd.rest.trimmed();
    if (filename.isEmpty()) return errorLine(400, "BAD_REQUEST");
    const FileInfoResult r = m_fs->info(filename);
    if (!r.ok) {
        if (r.error == "FORBIDDEN_PATH") return errorLine(403, "FORBIDDEN_PATH");
        if (r.error == "FILE_NOT_FOUND") return errorLine(404, "FILE_NOT_FOUND");
        return errorLine(500, "INTERNAL_ERROR");
    }

    QByteArray out = "OK size=" + QByteArray::number(r.size) +
                     " modified=" + QByteArray::number(r.modified) + "\n";
    return out;
}
