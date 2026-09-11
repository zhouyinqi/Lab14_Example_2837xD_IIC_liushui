#include "ScibSerialClient.h"

namespace {

constexpr char ScibTestRequest = static_cast<char>(0xA5);
constexpr char ScibTestResponse = static_cast<char>(0x5A);
constexpr int ScibResponseTimeoutMs = 2000;

} // namespace

ScibSerialClient::ScibSerialClient(QObject *parent)
    : QObject(parent)
{
    m_timeout.setSingleShot(true);
    connect(&m_timeout, &QTimer::timeout, this, [this] {
        finish(false,
               QStringLiteral("串口等待 0x5A 超时。请检查转换器、COM 口、9600 8N1 和 DSP 待命状态。"));
    });
    connect(&m_serial, &QSerialPort::readyRead, this, [this] {
        processReceivedData(m_serial.readAll());
    });
    connect(&m_serial, &QSerialPort::errorOccurred, this,
            [this](QSerialPort::SerialPortError error) {
                if (m_active && error != QSerialPort::NoError) {
                    finish(false,
                           QStringLiteral("串口错误：%1")
                               .arg(m_serial.errorString()));
                }
            });
}

bool ScibSerialClient::isActive() const
{
    return m_active;
}

void ScibSerialClient::processReceivedData(const QByteArray &data)
{
    if(!m_active || data.isEmpty()) return;
    m_receivedData = data;
    if(data == QByteArray(1, ScibTestResponse))
        finish(true, QStringLiteral("串口自动测试通过：0xA5 -> 0x5A。"));
    else
        finish(false, QStringLiteral("串口应答不匹配：应为单字节 0x5A。"));
}

void ScibSerialClient::startTest(const QString &portName, qint32 baudRate)
{
    if (m_active) {
        emit testProgress(QStringLiteral("串口测试正在进行。"));
        return;
    }

    if (portName.isEmpty()) {
        emit testFinished(false, QStringLiteral("未选择可用的 COM 口。"));
        return;
    }

    m_receivedData.clear();
    m_serial.setPortName(portName);
    m_serial.setBaudRate(baudRate);
    m_serial.setDataBits(QSerialPort::Data8);
    m_serial.setParity(QSerialPort::NoParity);
    m_serial.setStopBits(QSerialPort::OneStop);
    m_serial.setFlowControl(QSerialPort::NoFlowControl);
    m_active = true;

    if (!m_serial.open(QIODevice::ReadWrite)) {
        finish(false,
               QStringLiteral("无法打开 %1：%2")
                   .arg(portName, m_serial.errorString()));
        return;
    }

    if (m_serial.write(&ScibTestRequest, 1) != 1) {
        finish(false,
               QStringLiteral("%1 发送 0xA5 失败：%2")
                   .arg(portName, m_serial.errorString()));
        return;
    }

    m_serial.flush();
    emit testProgress(
        QStringLiteral("串口自动测试：%1，9600 8N1，已发送 0xA5，等待 0x5A。")
            .arg(portName));
    m_timeout.start(ScibResponseTimeoutMs);
}

void ScibSerialClient::cancel()
{
    if (m_active) {
        finish(false, QStringLiteral("串口测试已取消。"));
    }
}

void ScibSerialClient::finish(bool passed, const QString &message)
{
    if (!m_active) {
        return;
    }

    m_active = false;
    m_timeout.stop();
    if (m_serial.isOpen()) {
        m_serial.close();
    }
    emit testFinished(passed, message);
}
