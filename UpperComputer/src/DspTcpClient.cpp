#include "DspTcpClient.h"

#include <QNetworkProxy>

DspTcpClient::DspTcpClient(QObject *parent)
    : QObject(parent)
{
    m_socket.setProxy(QNetworkProxy::NoProxy);
    m_reconnectTimer.setInterval(2000);
    m_reconnectTimer.setSingleShot(true);
    m_connectTimer.setInterval(5000);
    m_connectTimer.setSingleShot(true);
    m_responseTimer.setInterval(3000);
    m_responseTimer.setSingleShot(true);
    connect(&m_connectTimer, &QTimer::timeout, this, [this] {
        if(m_socket.state() != QAbstractSocket::ConnectingState)
        {
            return;
        }

        emit connectionProgress(
            QStringLiteral("连接 DSP 超时（%1:%2），将在 2 秒后重试。")
                .arg(m_host)
                .arg(m_port));
        m_socket.abort();
        scheduleAutomaticReconnect();
    });
    connect(&m_responseTimer, &QTimer::timeout, this, [this] {
        if (!m_requestInFlight || !isConnected()) {
            return;
        }
        emit protocolError(QStringLiteral("DSP 响应超时，正在自动重连。"));
        m_socket.abort();
        scheduleAutomaticReconnect();
    });
    connect(&m_reconnectTimer, &QTimer::timeout, this, [this] {
        if (!m_autoReconnect || isConnected() || m_host.isEmpty()) {
            return;
        }
        if (m_automaticReconnectAttempts >= MaxAutomaticReconnectAttempts) {
            scheduleAutomaticReconnect();
            return;
        }
        ++m_automaticReconnectAttempts;
        m_socket.abort();
        emit connectionProgress(
            QStringLiteral("正在自动重连 DSP：%1:%2（%3/%4）。")
                .arg(m_host)
                .arg(m_port)
                .arg(m_automaticReconnectAttempts)
                .arg(MaxAutomaticReconnectAttempts));
        m_socket.connectToHost(m_host, m_port);
        m_connectTimer.start();
    });
    connect(&m_socket, &QTcpSocket::connected, this, [this] {
        m_connectTimer.stop();
        m_reconnectTimer.stop();
        emit connectionChanged(true, QStringLiteral("已连接DSP"));
        ping();
    });
    connect(&m_socket, &QTcpSocket::disconnected, this, [this] {
        m_connectTimer.stop();
        clearPendingRequests();
        emit connectionChanged(false, QStringLiteral("连接已断开"));
        scheduleAutomaticReconnect();
    });
    connect(&m_socket, &QTcpSocket::readyRead, this, [this] {
        m_rxBuffer.append(m_socket.readAll());
        processIncomingData();
    });
    connect(&m_socket, &QTcpSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) {
                m_connectTimer.stop();
                emit connectionChanged(false, m_socket.errorString());
                scheduleAutomaticReconnect();
            });
}

bool DspTcpClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void DspTcpClient::connectToDevice(const QString &host, quint16 port)
{
    startConnection(host, port, true);
}

void DspTcpClient::reconnectToDevice(const QString &host, quint16 port)
{
    startConnection(host, port, false);
}

void DspTcpClient::startConnection(const QString &host, quint16 port,
                                   bool resetReconnectLimit)
{
    if (!resetReconnectLimit && m_reconnectLimitReached) {
        emit connectionProgress(
            QStringLiteral("自动重连已停止，请手动点击“连接”后重试。"));
        return;
    }

    m_autoReconnect = false;
    m_reconnectTimer.stop();
    m_connectTimer.stop();
    m_socket.abort();
    m_rxBuffer.clear();
    clearPendingRequests();
    m_host = host;
    m_port = port;
    if (resetReconnectLimit) {
        m_automaticReconnectAttempts = 0;
        m_reconnectLimitReached = false;
    }
    m_autoReconnect = true;
    emit connectionProgress(
        QStringLiteral("正在连接 DSP：%1:%2。")
            .arg(m_host)
            .arg(m_port));
    m_socket.connectToHost(m_host, m_port);
    m_connectTimer.start();
}

void DspTcpClient::scheduleAutomaticReconnect()
{
    if (!m_autoReconnect || isConnected() || m_host.isEmpty() ||
        (m_socket.state() == QAbstractSocket::ConnectingState) ||
        m_reconnectTimer.isActive()) {
        return;
    }

    if (m_automaticReconnectAttempts >= MaxAutomaticReconnectAttempts) {
        m_autoReconnect = false;
        m_reconnectLimitReached = true;
        m_reconnectTimer.stop();
        emit connectionProgress(
            QStringLiteral("自动重连已连续尝试%1次，现已停止。请检查网络后手动点击“连接”。")
                .arg(MaxAutomaticReconnectAttempts));
        return;
    }

    m_reconnectTimer.start();
}

void DspTcpClient::disconnectFromDevice()
{
    m_autoReconnect = false;
    m_reconnectTimer.stop();
    m_connectTimer.stop();
    clearPendingRequests();
    m_socket.disconnectFromHost();
}

void DspTcpClient::ping()
{
    send(DspTestProtocol::Command::Ping);
}

void DspTcpClient::startBoardAuto()
{
    send(DspTestProtocol::Command::StartBoard);
}

void DspTcpClient::startExternalAuto()
{
    send(DspTestProtocol::Command::StartExternal);
}

void DspTcpClient::startHpdAuto(bool realAdcInput)
{
    send(DspTestProtocol::Command::StartHpd, realAdcInput ? 1U : 0U);
}

void DspTcpClient::startSingle(quint16 testId,
                               DspTestProtocol::Stage stage,
                               bool outputArmed,
                               quint8 singleSelection)
{
    quint8 flags = outputArmed ? DspTestProtocol::FlagOutputArmed : 0U;
    flags |= static_cast<quint8>(
        (singleSelection << DspTestProtocol::FlagSingleSelectionShift) &
        DspTestProtocol::FlagSingleSelectionMask);
    send(DspTestProtocol::Command::StartSingle,
         static_cast<quint8>(stage),
         flags,
         testId);
}

void DspTcpClient::stop()
{
    send(DspTestProtocol::Command::Stop);
}

void DspTcpClient::selectBoardProfile(quint16 boardProfileId)
{
    send(DspTestProtocol::Command::SelectBoardProfile,
         0U,
         0U,
         boardProfileId);
}

void DspTcpClient::confirmBoardProfile()
{
    send(DspTestProtocol::Command::ConfirmBoardProfile);
}

void DspTcpClient::clearBoardProfile()
{
    send(DspTestProtocol::Command::ClearBoardProfile);
}

void DspTcpClient::requestStatus()
{
    send(DspTestProtocol::Command::GetStatus);
}

void DspTcpClient::requestRecord(quint16 testId)
{
    send(DspTestProtocol::Command::GetRecord, 0U, 0U, testId);
}

void DspTcpClient::requestBoardInfo()
{
    send(DspTestProtocol::Command::GetBoardInfo);
}

void DspTcpClient::requestTestAvailability(quint16 testId)
{
    send(DspTestProtocol::Command::GetTestAvailability, 0U, 0U, testId);
}

void DspTcpClient::configureAdcInjection(quint8 signalId,
                                         quint16 expectedMillivolts,
                                         quint16 toleranceMillivolts)
{
    send(DspTestProtocol::Command::ConfigureAdcInjection,
         signalId,
         static_cast<quint8>(
             toleranceMillivolts /
             DspTestProtocol::AdcToleranceStepMillivolts),
         expectedMillivolts);
}

void DspTcpClient::send(DspTestProtocol::Command command,
                        quint8 stage,
                        quint8 flags,
                        quint16 argument)
{
    if (!isConnected()) {
        emit protocolError(QStringLiteral("DSP未连接"));
        return;
    }

    m_txQueue.enqueue(DspTestProtocol::makeRequest(command,
                                                    m_sequence++,
                                                    stage,
                                                    flags,
                                                    argument));
    sendNextRequest();
}

void DspTcpClient::sendNextRequest()
{
    if (!isConnected() || m_requestInFlight || m_txQueue.isEmpty()) {
        return;
    }

    m_requestInFlight = true;
    m_socket.write(m_txQueue.dequeue());
    m_responseTimer.start();
}

void DspTcpClient::clearPendingRequests()
{
    m_txQueue.clear();
    m_requestInFlight = false;
    m_responseTimer.stop();
}

void DspTcpClient::processIncomingData()
{
    while (m_rxBuffer.size() >= DspTestProtocol::ResponseSize) {
        const int headerIndex = m_rxBuffer.indexOf(QByteArrayLiteral("BR"));
        if (headerIndex < 0) {
            m_rxBuffer.clear();
            m_requestInFlight = false;
            m_responseTimer.stop();
            emit protocolError(QStringLiteral("收到无效响应帧头"));
            sendNextRequest();
            return;
        }

        if (headerIndex > 0) {
            m_rxBuffer.remove(0, headerIndex);
            continue;
        }

        const QByteArray frame = m_rxBuffer.left(DspTestProtocol::ResponseSize);
        DspTestProtocol::Response response;
        QString error;
        m_rxBuffer.remove(0, DspTestProtocol::ResponseSize);
        if (!DspTestProtocol::parseResponse(frame, &response, &error)) {
            m_requestInFlight = false;
            m_responseTimer.stop();
            emit protocolError(error);
            sendNextRequest();
            continue;
        }

        m_requestInFlight = false;
        m_responseTimer.stop();
        emit responseReceived(response);
        sendNextRequest();
    }
}
