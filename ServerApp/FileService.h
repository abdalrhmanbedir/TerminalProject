#pragma once
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMutex>

struct FileInfoResult {
    bool ok = false;
    long long size = 0;
    long long modified = 0;
    QString error;
};

class FileService {
public:
    explicit FileService(QString baseDir);

    static bool isSafeRelativeName(const QString& name);
    QString resolvePath(const QString& name) const;

    bool createEmpty(const QString& filename, QString& err);
    bool writeAll(const QString& filename, const QByteArray& data, QString& err);
    bool appendAll(const QString& filename, const QByteArray& data, QString& err);
    bool readAll(const QString& filename, QByteArray& out, QString& err);
    bool removeFile(const QString& filename, QString& err);
    bool renameFile(const QString& oldName, const QString& newName, QString& err);

    bool listFiles(QStringList& out, QString& err);
    FileInfoResult info(const QString& filename) const;

private:
    QString m_baseDir;
    mutable QMutex m_mtx;
};
