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
               QStringLiteral("SCIB 等待 0x5A 超时。请检查 USB-RS485、COM 口、9600 8N1 和 DSP 待命状态。"));
    });
    connect(&m_serial, &QSerialPort::readyRead, this, [this] {
        if (!m_active) {
            return;
        }

        m_receivedData.append(m_serial.readAll());
        if (m_receivedData.contains(ScibTestResponse)) {
            finish(true, QStringLiteral("SCIB 自动测试通过：0xA5 -> 0x5A。"));
        }
    });
    connect(&m_serial, &QSerialPort::errorOccurred, this,
            [this](QSerialPort::SerialPortError error) {
                if (m_active && error != QSerialPort::NoError) {
                    finish(false,
                           QStringLiteral("SCIB 串口错误：%1")
                               .arg(m_serial.errorString()));
                }
            });
}

bool ScibSerialClient::isActive() const
{
    return m_active;
}

void ScibSerialClient::startTest(const QString &portName, qint32 baudRate)
{
    if (m_active) {
        emit testProgress(QStringLiteral("SCIB 测试正在进行。"));
        return;
    }

    if (portName.isEmpty()) {
        emit testFinished(false, QStringLiteral("未选择可用的 SCIB COM 口。"));
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
        QStringLiteral("SCIB 自动测试：%1，9600 8N1，已发送 0xA5，等待 0x5A。")
            .arg(portName));
    m_timeout.start(ScibResponseTimeoutMs);
}

void ScibSerialClient::cancel()
{
    if (m_active) {
        finish(false, QStringLiteral("SCIB 测试已取消。"));
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
