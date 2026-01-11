#include <gtest/gtest.h>

#include <QProcess>
#include <QTcpSocket>
#include <QTemporaryDir>
#include <QDir>
#include <QStringList>

static QByteArray readLineBlocking(QTcpSocket& s)
{
    while (!s.canReadLine()) {
        ASSERT_TRUE(s.waitForReadyRead(2000));
    }
    return s.readLine(); // includes '\n'
}

static void sendLine(QTcpSocket& s, const QByteArray& line)
{
    s.write(line);
    s.write("\n");
    ASSERT_TRUE(s.waitForBytesWritten(2000));
}

TEST(ProtocolIntegration, CreateWriteReadList)
{
    // 1) Temp working dir
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid()); // temp dir exists [web:680]
    const QString wd = tempDir.path();

    // 2) Start server process (عدل المسار حسب build عندك)
    QProcess server;
    // شغّل ServerApp من build folder بتاعه:
    // مثال: "../build-ServerApp-Desktop-Debug/ServerApp"
    // لازم تعدل المسار ده مرة واحدة عندك.
    const QString serverExe = "../build-ServerApp-Desktop-Debug/ServerApp";
    QStringList args;
    // لو السيرفر بتاعك مش بيدعم args تجاهلها.
    // الأفضل تضيف option في السيرفر لاحقًا: --port --workdir

    server.setProgram(serverExe);
    server.setArguments(args);
    server.setWorkingDirectory(wd);
    server.start();
    ASSERT_TRUE(server.waitForStarted(3000));

    // 3) Connect as client
    QTcpSocket client;
    client.connectToHost("127.0.0.1", 5555);
    ASSERT_TRUE(client.waitForConnected(3000));

    // لو عندك AUTH لازم تبعته الأول:
    sendLine(client, "AUTH abdalrhmanmoghazy,575859");
    ASSERT_EQ(readLineBlocking(client), QByteArray("OK Authenticated\n"));

    // Client: CREATE hi.txt
    sendLine(client, "CREATE hi.txt");
    // انت عندك response: "OK CREATED\n" (لو كودك كده)
    // لكن المثال بتاعك: "OK <path>"
    // اختار واحد وخليه ثابت، وهنا هنتوقع "OK CREATED"
    ASSERT_EQ(readLineBlocking(client), QByteArray("OK CREATED\n"));

    // Client: WRITE hi.txt;Hello World
    sendLine(client, "WRITE hi.txt;Hello World");
    ASSERT_EQ(readLineBlocking(client), QByteArray("OK WRITTEN\n"));

    // Client: READ hi.txt
    sendLine(client, "READ hi.txt");
    ASSERT_EQ(readLineBlocking(client), QByteArray("OK 11\n")); // "Hello World" = 11
    QByteArray data = readLineBlocking(client);
    ASSERT_EQ(data, QByteArray("Hello World\n"));

    // Client: LIST
    sendLine(client, "LIST");
    ASSERT_EQ(readLineBlocking(client), QByteArray("OK 1\n"));
    ASSERT_EQ(readLineBlocking(client), QByteArray("hi.txt\n"));

    client.disconnectFromHost();
    client.waitForDisconnected(1000);

    server.terminate();
    server.waitForFinished(2000);
}
