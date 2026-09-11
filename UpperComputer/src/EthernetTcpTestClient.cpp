#include "EthernetTcpTestClient.h"

#include <QNetworkProxy>

namespace {

const QByteArray EchoRequest = QByteArrayLiteral("BTST");
const QByteArray EchoResponse = QByteArrayLiteral("BTOK");
constexpr int ConnectRetryIntervalMs = 100;
constexpr int LinkTimeoutMs = 5000;
constexpr int EchoTimeoutMs = 5000;
constexpr int StabilityTimeoutMs = 8000;
constexpr int StabilityReplyCount = 5;
constexpr int StabilityRequestIntervalMs = 80;

} // namespace

EthernetTcpTestClient::EthernetTcpTestClient(QObject *parent)
    : QObject(parent)
{
    m_socket.setProxy(QNetworkProxy::NoProxy);
    m_connectRetryTimer.setSingleShot(true);
    m_connectRetryTimer.setInterval(ConnectRetryIntervalMs);
    m_timeoutTimer.setSingleShot(true);
    m_nextRequestTimer.setSingleShot(true);

    connect(&m_connectRetryTimer, &QTimer::timeout,
            this, &EthernetTcpTestClient::connectToDsp);
    connect(&m_nextRequestTimer, &QTimer::timeout,
            this, &EthernetTcpTestClient::sendEchoRequest);
    connect(&m_timeoutTimer, &QTimer::timeout, this, [this] {
        finish(false, QStringLiteral("TCP 测试超时。DSP 未在规定时间内完成连接或应答。"));
    });
    connect(&m_socket, &QTcpSocket::connected, this, [this] {
        m_connectedOnce = true;
        if(m_mode == Mode::Link)
        {
            emit testProgress(QStringLiteral("TCP_LINK：Qt 已建立到 DSP 的 TCP 连接，等待 DSP 关闭连接。"));
            return;
        }

        emit testProgress(
            m_mode == Mode::Echo ?
                QStringLiteral("TCP_ECHO：已连接 DSP，发送 BTST。") :
                QStringLiteral("TCP_STABILITY：已连接 DSP，开始 5 次 BTST -> BTOK 收发。"));
        sendEchoRequest();
    });
    connect(&m_socket, &QTcpSocket::readyRead, this, [this] {
        if(!m_active) return;
        m_rxBuffer.append(m_socket.readAll());
        processIncomingData();
    });
    connect(&m_socket, &QTcpSocket::disconnected, this, [this] {
        if(!m_active)
        {
            return;
        }

        if((m_mode == Mode::Link) && m_connectedOnce)
        {
            finish(true, QStringLiteral("TCP_LINK：DSP 已建立并正常关闭测试连接。"));
            return;
        }
        if(m_payloadComplete)
        {
            finish(true,
                   m_mode == Mode::Echo ?
                       QStringLiteral("TCP_ECHO：BTST -> BTOK 收发完成。") :
                       QStringLiteral("TCP_STABILITY：5 次 BTST -> BTOK 收发完成。"));
            return;
        }

        finish(false, QStringLiteral("TCP 应答完成前连接断开。"));
    });
    connect(&m_socket, &QTcpSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) {
        if(m_active &&
           (m_socket.state() == QAbstractSocket::UnconnectedState))
        {
            scheduleReconnect();
        }
    });
}

bool EthernetTcpTestClient::isActive() const
{
    return m_active;
}

void EthernetTcpTestClient::start(const QString &host, quint16 port, Mode mode)
{
    if(m_active)
    {
        emit testFinished(false, QStringLiteral("TCP 测试正在进行。"));
        return;
    }

    m_host = host;
    m_port = port;
    m_mode = mode;
    m_expectedReplyCount = (mode == Mode::Stability) ? StabilityReplyCount : 1;
    m_replyCount = 0;
    m_active = true;
    m_connectedOnce = false;
    m_payloadComplete = false;
    m_waitingForReply = false;
    m_rxBuffer.clear();
    m_socket.abort();
    m_timeoutTimer.start(mode == Mode::Stability ? StabilityTimeoutMs :
                                                  (mode == Mode::Echo ? EchoTimeoutMs : LinkTimeoutMs));
    connectToDsp();
}

void EthernetTcpTestClient::cancel()
{
    if(m_active)
    {
        finish(false, QStringLiteral("TCP 测试已取消。"));
    }
}

void EthernetTcpTestClient::connectToDsp()
{
    if(!m_active || m_host.isEmpty() || (m_port == 0U))
    {
        return;
    }

    if(m_socket.state() != QAbstractSocket::UnconnectedState)
    {
        return;
    }

    m_socket.connectToHost(m_host, m_port);
}

void EthernetTcpTestClient::scheduleReconnect()
{
    if(m_active && !m_payloadComplete && !m_connectRetryTimer.isActive())
    {
        m_connectRetryTimer.start();
    }
}

void EthernetTcpTestClient::sendEchoRequest()
{
    if(!m_active || (m_socket.state() != QAbstractSocket::ConnectedState))
    {
        return;
    }

    m_waitingForReply = true;
    m_socket.write(EchoRequest);
}

void EthernetTcpTestClient::processIncomingData()
{
    if(!m_rxBuffer.isEmpty() && !m_waitingForReply)
    {
        finish(false, QStringLiteral("TCP 收到非请求应答。"));
        return;
    }
    if(m_rxBuffer.size() >= EchoResponse.size())
    {
        if(m_rxBuffer != EchoResponse)
        {
            finish(false, QStringLiteral("TCP 应答内容或数量不匹配。"));
            return;
        }
        m_rxBuffer.clear();
        m_waitingForReply = false;
        ++m_replyCount;
        emit testProgress(QStringLiteral("TCP 应答 %1/%2：BTOK。")
                              .arg(m_replyCount)
                              .arg(m_expectedReplyCount));

        if(m_replyCount >= m_expectedReplyCount)
        {
            m_payloadComplete = true;
            m_socket.disconnectFromHost();
            return;
        }

        m_nextRequestTimer.start(StabilityRequestIntervalMs);
        return;
    }
}

void EthernetTcpTestClient::finish(bool passed, const QString &message)
{
    if(!m_active)
    {
        return;
    }

    m_active = false;
    m_connectRetryTimer.stop();
    m_timeoutTimer.stop();
    m_nextRequestTimer.stop();
    m_socket.abort();
    emit testFinished(passed, message);
}
