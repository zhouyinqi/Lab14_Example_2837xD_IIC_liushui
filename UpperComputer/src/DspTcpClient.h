#pragma once

#include "DspTestProtocol.h"

#include <QObject>
#include <QQueue>
#include <QTcpSocket>
#include <QTimer>

class DspTcpClient final : public QObject
{
    Q_OBJECT

public:
    explicit DspTcpClient(QObject *parent = nullptr);

    bool isConnected() const;
    void connectToDevice(const QString &host, quint16 port);
    void disconnectFromDevice();

    void ping();
    void startBoardAuto();
    void startExternalAuto();
    void startHpdAuto(bool realAdcInput);
    void startSingle(quint16 testId,
                     DspTestProtocol::Stage stage,
                     bool outputArmed);
    void stop();
    void selectBoardProfile(quint16 boardProfileId);
    void confirmBoardProfile();
    void clearBoardProfile();
    void requestStatus();
    void requestRecord(quint16 testId);
    void requestBoardInfo();

signals:
    void connectionChanged(bool connected, const QString &detail);
    void connectionProgress(const QString &detail);
    void responseReceived(const DspTestProtocol::Response &response);
    void protocolError(const QString &detail);

private:
    void send(DspTestProtocol::Command command,
              quint8 stage = 0,
              quint8 flags = 0,
              quint16 argument = 0);
    void sendNextRequest();
    void clearPendingRequests();
    void processIncomingData();

    QTcpSocket m_socket;
    QTimer m_reconnectTimer;
    QTimer m_connectTimer;
    QTimer m_responseTimer;
    QByteArray m_rxBuffer;
    QQueue<QByteArray> m_txQueue;
    QString m_host;
    quint16 m_port = 0;
    bool m_autoReconnect = false;
    bool m_requestInFlight = false;
    quint16 m_sequence = 1;
};
