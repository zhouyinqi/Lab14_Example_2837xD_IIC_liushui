#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

class EthernetTcpTestClient final : public QObject
{
    Q_OBJECT

public:
    enum class Mode : quint8 {
        Link,
        Echo,
        Stability
    };

    explicit EthernetTcpTestClient(QObject *parent = nullptr);

    bool isActive() const;
    void start(const QString &host, quint16 port, Mode mode);
    void cancel();

signals:
    void testProgress(const QString &message);
    void testFinished(bool passed, const QString &message);

private:
    void connectToDsp();
    void scheduleReconnect();
    void sendEchoRequest();
    void processIncomingData();
    void finish(bool passed, const QString &message);

    QTcpSocket m_socket;
    QTimer m_connectRetryTimer;
    QTimer m_timeoutTimer;
    QTimer m_nextRequestTimer;
    QByteArray m_rxBuffer;
    QString m_host;
    quint16 m_port = 0;
    Mode m_mode = Mode::Link;
    int m_expectedReplyCount = 0;
    int m_replyCount = 0;
    bool m_active = false;
    bool m_connectedOnce = false;
    bool m_payloadComplete = false;
};
