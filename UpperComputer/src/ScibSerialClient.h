#pragma once

#include <QObject>
#include <QSerialPort>
#include <QTimer>

class ScibSerialClient final : public QObject
{
    Q_OBJECT

public:
    explicit ScibSerialClient(QObject *parent = nullptr);

    bool isActive() const;
    void startTest(const QString &portName, qint32 baudRate = 9600);
    void cancel();

signals:
    void testProgress(const QString &message);
    void testFinished(bool passed, const QString &message);

private:
    friend class MainWindowTests;
    void processReceivedData(const QByteArray &data);
    void finish(bool passed, const QString &message);

    QSerialPort m_serial;
    QTimer m_timeout;
    QByteArray m_receivedData;
    bool m_active = false;
};
