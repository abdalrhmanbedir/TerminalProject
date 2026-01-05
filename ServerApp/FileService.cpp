#include "FileService.h"
#include <QDir>
#include <QFile>
#include <QMutexLocker>

#include <sys/stat.h>

FileService::FileService(QString baseDir) : m_baseDir(std::move(baseDir)) {}

bool FileService::isSafeRelativeName(const QString& name)
{
    if (name.isEmpty()) return false;
    if (QDir::isAbsolutePath(name)) return false;
    if (name.contains("..")) return false;
    if (name.contains('\\')) return false;
    return true;
}

QString FileService::resolvePath(const QString& name) const
{
    return QDir(m_baseDir).filePath(name);
}

static bool existsStat(const QString& fullPath)
{
    struct stat st;
    const QByteArray p = fullPath.toUtf8();
    return stat(p.constData(), &st) == 0;
}

bool FileService::createEmpty(const QString& filename, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(filename)) { err = "FORBIDDEN_PATH"; return false; }

    const QString p = resolvePath(filename);
    if (existsStat(p)) { err = "FILE_ALREADY_EXISTS"; return false; }

    QFile f(p);
    if (!f.open(QIODevice::WriteOnly)) { err = "CANNOT_CREATE"; return false; }
    f.close();
    return true;
}

bool FileService::writeAll(const QString& filename, const QByteArray& data, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(filename)) { err = "FORBIDDEN_PATH"; return false; }

    const QString p = resolvePath(filename);
    if (!existsStat(p)) { err = "FILE_NOT_FOUND"; return false; }

    QFile f(p);
    if (!f.open(QIODevice::WriteOnly)) { err = "CANNOT_WRITE"; return false; }
    if (f.write(data) < 0) { err = "CANNOT_WRITE"; f.close(); return false; }
    f.close();
    return true;
}

bool FileService::appendAll(const QString& filename, const QByteArray& data, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(filename)) { err = "FORBIDDEN_PATH"; return false; }

    const QString p = resolvePath(filename);
    if (!existsStat(p)) { err = "FILE_NOT_FOUND"; return false; }

    QFile f(p);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Append)) { err = "CANNOT_WRITE"; return false; }
    if (f.write(data) < 0) { err = "CANNOT_WRITE"; f.close(); return false; }
    f.close();
    return true;
}

bool FileService::readAll(const QString& filename, QByteArray& out, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(filename)) { err = "FORBIDDEN_PATH"; return false; }

    const QString p = resolvePath(filename);
    if (!existsStat(p)) { err = "FILE_NOT_FOUND"; return false; }

    QFile f(p);
    if (!f.open(QIODevice::ReadOnly)) { err = "CANNOT READ"; return false; }
    out = f.readAll();
    f.close();
    return true;
}

bool FileService::removeFile(const QString& filename, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(filename)) { err = "FORBIDDEN PATH"; return false; }

    const QString p = resolvePath(filename);
    if (!existsStat(p)) { err = "FILE_NOT_FOUND"; return false; }

    QFile f(p);
    if (!f.remove()) { err = "CANNOT DELETE"; return false; }
    return true;
}

bool FileService::renameFile(const QString& oldName, const QString& newName, QString& err)
{
    QMutexLocker lock(&m_mtx);
    if (!isSafeRelativeName(oldName) || !isSafeRelativeName(newName)) { err = "FORBIDDEN_PATH"; return false; }

    const QString oldP = resolvePath(oldName);
    const QString newP = resolvePath(newName);
    if (!existsStat(oldP)) { err = "FILE_NOT_FOUND"; return false; }
    if (existsStat(newP)) { err = "FILE_ALREADY_EXISTS"; return false; }

    if (!QFile::rename(oldP, newP)) { err = "RENAME_FAILED"; return false; }
    return true;
}

bool FileService::listFiles(QStringList& out, QString& err)
{
    QMutexLocker lock(&m_mtx);
    QDir d(m_baseDir);
    if (!d.exists()) { err = "INTERNAL_ERROR"; return false; }
    out = d.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
    return true;
}

FileInfoResult FileService::info(const QString& filename) const
{
    QMutexLocker lock(&m_mtx);
    FileInfoResult r;

    if (!isSafeRelativeName(filename)) { r.error = "FORBIDDEN_PATH"; return r; }

    const QString fullPath = resolvePath(filename);
    struct stat st;
    const QByteArray p = fullPath.toUtf8();
    if (stat(p.constData(), &st) != 0) { r.error = "FILE_NOT_FOUND"; return r; }

    r.ok = true;
    r.size = (long long)st.st_size;
    r.modified = (long long)st.st_mtime;
    return r;
}
