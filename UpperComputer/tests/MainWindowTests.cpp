#include "MainWindow.h"
#include "EthernetTcpTestClient.h"
#include "ScibSerialClient.h"
#include "ZlgCanClient.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QPlainTextEdit>
#include <QSettings>
#include <QScreen>
#include <QSplitter>
#include <QSignalSpy>
#include <QTableWidget>
#include <QTabWidget>
#include <QTcpServer>
#include <QTemporaryDir>
#include <QTest>
#include <QtEndian>

#include <cstring>

namespace {
constexpr quint16 TestDo = 0x0313;
constexpr quint16 TestPwm = 0x031F;
constexpr quint16 TestProtection = 0x0321;
constexpr quint16 TestDriverReset = 0x0322;
constexpr quint16 TestLowVoltageFirst = 0x0400;
constexpr quint16 TestLowVoltageLast = 0x040A;

void put16(QByteArray &frame, int offset, quint16 value)
{
    qToBigEndian(value, frame.data() + offset);
}

void putFloat(QByteArray &frame, int offset, float value)
{
    quint32 bits;
    std::memcpy(&bits, &value, sizeof(bits));
    qToBigEndian(bits, frame.data() + offset);
}

// Loopback only: never connects to a board or drives a physical output.
class MockDsp final : public QTcpServer
{
public:
    MockDsp()
    {
        setProxy(QNetworkProxy::NoProxy);
        connect(this, &QTcpServer::newConnection, this, [this] {
            while (hasPendingConnections()) {
                auto *socket = nextPendingConnection();
                connect(socket, &QTcpSocket::disconnected,
                        socket, &QObject::deleteLater);
                connect(socket, &QTcpSocket::readyRead, this,
                        [this, socket, buffer = QByteArray()]() mutable {
                    buffer += socket->readAll();
                    while (buffer.size() >= DspTestProtocol::RequestSize) {
                        const auto request = buffer.first(DspTestProtocol::RequestSize);
                        buffer.remove(0, DspTestProtocol::RequestSize);
                        requests.append(request);
                        if (respond) {
                            const auto response = makeResponse(request);
                            if(injectInvalidResponses) {
                                auto invalid = response;
                                put16(invalid, 4, qFromBigEndian<quint16>(request.data() + 4) - 1U);
                                put16(invalid, 38, DspTestProtocol::crc16(invalid, 38));
                                socket->write(invalid);
                                invalid = response;
                                invalid[3] = static_cast<char>(static_cast<quint8>(request[3]) ^ 1U);
                                put16(invalid, 38, DspTestProtocol::crc16(invalid, 38));
                                socket->write(invalid);
                                invalid = response;
                                invalid[38] = static_cast<char>(invalid[38] ^ 1);
                                socket->write(invalid);
                            }
                            if(sendValidResponse) socket->write(response);
                        }
                    }
                });
            }
        });
    }

    bool respond = true;
    bool injectInvalidResponses = false;
    bool sendValidResponse = true;
    void replyToLastRequest()
    {
        const auto sockets = findChildren<QTcpSocket *>();
        if(!sockets.isEmpty() && !requests.isEmpty())
            sockets.last()->write(makeResponse(requests.last()));
    }
    int statusResponses = 0;
    QList<QByteArray> requests;
    DspTestProtocol::Result recordResult = DspTestProtocol::Result::Warn;
    quint16 recordError = 0;
    quint16 managerMode = 1;
    quint16 activeId = DspTestProtocol::InvalidTestId;
    quint32 recordRaw = 0x0001000F;
    float recordMeasured = 0.0F;
    float recordMask = 1.0F;

private:
    QByteArray makeResponse(const QByteArray &request)
    {
        QByteArray frame(DspTestProtocol::ResponseSize, '\0');
        frame[0] = 'B';
        frame[1] = 'R';
        frame[2] = DspTestProtocol::Version;
        frame[3] = request[3];
        frame[4] = request[4];
        frame[5] = request[5];
        put16(frame, 10, managerMode);
        put16(frame, 14, activeId);
        put16(frame, 16, DspTestProtocol::InvalidTestId);
        const auto command = static_cast<DspTestProtocol::Command>(request[3]);
        if (command == DspTestProtocol::Command::GetRecord) {
            put16(frame, 16, qFromBigEndian<quint16>(request.data() + 8));
            put16(frame, 18, static_cast<quint16>(recordResult));
            put16(frame, 20, recordError);
            qToBigEndian(recordRaw, frame.data() + 22);
            putFloat(frame, 26, recordMeasured);
            putFloat(frame, 30, recordMask);
            putFloat(frame, 34, recordMask);
        }
        if (command == DspTestProtocol::Command::GetStatus) {
            ++statusResponses;
        }
        put16(frame, 38, DspTestProtocol::crc16(frame, 38));
        return frame;
    }
};
}

class MainWindowTests final : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_settingsDirectory;

    static QString resultText(const MainWindow &window, quint16 testId)
    {
        const auto location = window.m_recordRows.value(testId);
        return location.table->item(location.row, 7)->text();
    }

    static void requestManualRecord(MainWindow &window, quint16 testId, bool automatic)
    {
        window.m_boardProfileId = 3;
        if (automatic) {
            window.prepareResultRefresh(MainWindow::RefreshScope::External);
            window.m_edoManualConfirmationExpected = true;
            window.m_activeRecordIds = {TestDo};
            window.m_nextRecordIndex = 0;
            window.requestNextRecord();
        } else {
            window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, testId);
            window.m_singleTestRecordRefreshInFlight = true;
            window.m_client.requestRecord(testId);
        }
    }

private slots:
    void ethernetSimulatedDevice_data()
    {
        ethernetDeviceRows();
    }

    void simulatedCanReceiveAndCleanup()
    {
        runSimulatedCanReceiveAndCleanup();
    }

    void staleControlResponsesCannotAdvanceQueue()
    {
        MockDsp server;
        server.injectInvalidResponses = true;
        server.sendValidResponse = false;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));
        DspTcpClient client;
        QSignalSpy responses(&client, &DspTcpClient::responseReceived);
        QSignalSpy errors(&client, &DspTcpClient::protocolError);
        client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_VERIFY(client.isConnected());
        QTRY_COMPARE(errors.count(), 3);
        QCOMPARE(responses.count(), 0);
        client.requestStatus();
        QTest::qWait(50);
        QCOMPARE(server.requests.count(), 1);
        server.injectInvalidResponses = false;
        server.sendValidResponse = true;
        server.replyToLastRequest();
        QTRY_COMPARE(responses.count(), 2);
        QCOMPARE(server.requests.count(), 2);
        client.disconnectFromDevice();
    }

private:
    void runSimulatedCanReceiveAndCleanup()
    {
        static int scenario, resets, closes, receives;
        ZlgCanClient client;
        client.m_receive = [](quint32, quint32, quint32,
                              ZlgCanClient::VciCanObject *frames, quint32, qint32) -> quint32 {
            ++receives;
            if(scenario == 1) return 0xFFFFFFFFU;
            if(scenario == 2) return 0U;
            frames[0] = {};
            frames[0].id = scenario == 3 ? 0x123U : 1U;
            frames[0].dataLength = scenario == 4 ? 8U : 4U;
            frames[0].externFlag = scenario == 5 ? 1U : 0U;
            frames[0].remoteFlag = scenario == 6 ? 1U : 0U;
            frames[0].data[0] = scenario == 7 ? 0U : 0xA5U;
            frames[0].data[1] = 0x5AU;
            frames[0].data[2] = 0xA5U;
            frames[0].data[3] = 0x5AU;
            return 1U;
        };
        client.m_resetCan = [](quint32, quint32, quint32) -> quint32 { ++resets; return 1U; };
        client.m_closeDevice = [](quint32, quint32) -> quint32 { ++closes; return 1U; };
        QSignalSpy finished(&client, &ZlgCanClient::testFinished);
        for(scenario = 0; scenario < 9; ++scenario) {
            resets = closes = receives = 0;
            finished.clear();
            client.m_active = client.m_deviceOpened = client.m_channelStarted = true;
            if(scenario == 8) {
                client.cancel();
            } else {
                client.pollReceive();
                if(scenario >= 2) {
                    QCOMPARE(finished.count(), 0);
                    client.m_timeoutTimer.start(1);
                    QTRY_COMPARE(finished.count(), 1);
                }
            }
            QCOMPARE(finished.count(), 1);
            QCOMPARE(finished.first().at(0).toBool(), scenario == 0);
            QCOMPARE(resets, 1);
            QCOMPARE(closes, 1);
            QVERIFY(!client.isActive());
            QVERIFY(!client.m_pollTimer.isActive());
            QVERIFY(!client.m_timeoutTimer.isActive());
            const auto reads = receives;
            client.pollReceive();
            client.cancel();
            QCOMPARE(receives, reads);
            QCOMPARE(finished.count(), 1);
        }
    }

private slots:
    void simulatedSerialReceiveTimeoutAndRepeat()
    {
        ScibSerialClient client;
        QSignalSpy finished(&client, &ScibSerialClient::testFinished);
        const QList<QByteArray> replies = {QByteArray::fromHex("5a"),
            QByteArray::fromHex("00"), QByteArray::fromHex("a55a"),
            QByteArray::fromHex("5a5a")};
        for(const auto &reply : replies) {
            finished.clear();
            client.m_active = true;
            client.m_timeout.start(2000);
            client.processReceivedData({});
            QCOMPARE(finished.count(), 0);
            client.processReceivedData(reply);
            QCOMPARE(finished.count(), 1);
            QCOMPARE(finished.first().at(0).toBool(), reply == QByteArray::fromHex("5a"));
            QVERIFY(!client.isActive());
            QVERIFY(!client.m_timeout.isActive());
            client.processReceivedData(QByteArray::fromHex("5a"));
            QCOMPARE(finished.count(), 1);
        }
        finished.clear();
        client.m_active = true;
        client.m_timeout.start(1);
        QTRY_COMPARE(finished.count(), 1);
        QVERIFY(!finished.first().at(0).toBool());
        client.m_active = true;
        client.cancel();
        QCOMPARE(finished.count(), 2);
        QVERIFY(!client.isActive());
        client.startTest(QString());
        QCOMPARE(finished.count(), 3);
        QVERIFY(!finished.last().at(0).toBool());
    }

private:
    void ethernetDeviceRows()
    {
        QTest::addColumn<int>("scenario");
        QTest::addColumn<bool>("passed");
        QTest::newRow("link") << 0 << true;
        QTest::newRow("fragmented-echo") << 1 << true;
        QTest::newRow("five-exchanges") << 2 << true;
        QTest::newRow("extra-replies") << 3 << false;
        QTest::newRow("wrong-payload") << 4 << false;
        QTest::newRow("disconnect") << 5 << false;
        QTest::newRow("silent-timeout") << 6 << false;
    }

private slots:
    void ethernetSimulatedDevice()
    {
        QFETCH(int, scenario);
        QFETCH(bool, passed);
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));
        int requestCount = 0;
        connect(&server, &QTcpServer::newConnection, &server, [&] {
            auto *peer = server.nextPendingConnection();
            if(scenario == 0) {
                peer->disconnectFromHost();
                return;
            }
            connect(peer, &QTcpSocket::readyRead, peer, [&, peer] {
                auto pending = peer->property("pending").toByteArray();
                pending += peer->readAll();
                if(pending.size() < 4) {
                    peer->setProperty("pending", pending);
                    return;
                }
                QCOMPARE(pending, QByteArray("BTST"));
                peer->setProperty("pending", QByteArray());
                ++requestCount;
                if(scenario == 1) {
                    peer->write("BT");
                    QTimer::singleShot(10, peer, [peer] { peer->write("OK"); });
                } else if(scenario == 2) {
                    peer->write("BTOK");
                } else if(scenario == 3) {
                    peer->write("BTOKBTOKBTOKBTOKBTOK");
                } else if(scenario == 4) {
                    peer->write("FAIL");
                } else if(scenario == 5) {
                    peer->disconnectFromHost();
                }
            });
        });
        EthernetTcpTestClient client;
        QSignalSpy finished(&client, &EthernetTcpTestClient::testFinished);
        const auto mode = scenario == 0 ? EthernetTcpTestClient::Mode::Link :
            (scenario == 2 || scenario == 3 ? EthernetTcpTestClient::Mode::Stability :
                                              EthernetTcpTestClient::Mode::Echo);
        client.start(QStringLiteral("127.0.0.1"), server.serverPort(), mode);
        QTRY_COMPARE_WITH_TIMEOUT(finished.count(), 1, 6500);
        QCOMPARE(finished.first().at(0).toBool(), passed);
        QVERIFY(!client.isActive());
        if(scenario == 2) QCOMPARE(requestCount, 5);
    }

    void ethernetCancelAndRepeat()
    {
        QTcpServer server;
        QVERIFY(server.listen(QHostAddress::LocalHost, 0));
        EthernetTcpTestClient client;
        QSignalSpy finished(&client, &EthernetTcpTestClient::testFinished);
        for(int run = 0; run < 2; ++run) {
            client.start(QStringLiteral("127.0.0.1"), server.serverPort(),
                         EthernetTcpTestClient::Mode::Echo);
            QVERIFY(client.isActive());
            client.cancel();
            QCOMPARE(finished.count(), run + 1);
            QVERIFY(!finished.last().at(0).toBool());
            QVERIFY(!client.isActive());
        }
        QTest::qWait(150);
        QCOMPARE(finished.count(), 2);
    }

    void lowVoltageInputLiveProtocolCompletion()
    {
        for(quint16 testId : {quint16(0x0400), quint16(0x0403)}) {
            MockDsp server;
            QVERIFY(server.listen(QHostAddress::LocalHost, 0));
            MainWindow window;
            window.m_client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
            QTRY_VERIFY(window.m_client.isConnected());
            window.m_boardProfileId = 2;
            const quint16 mask = (testId == 0x0400) ? 0x3f : 3;
            server.activeId = testId;
            server.managerMode = 3;
            server.recordResult = DspTestProtocol::Result::Running;
            server.recordRaw = mask;
            server.recordMask = mask;
            window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, testId);
            window.m_client.startSingle(testId, DspTestProtocol::Stage::ExternalConnected, false, 0);
            QTRY_VERIFY(window.m_protectionResetLiveTimer->isActive());
            QTRY_VERIFY(window.m_lowVoltageInputStatus->text().contains(QStringLiteral("等待翻转")));
            QVERIFY(window.m_singleTestRefreshPending);
            server.recordResult = DspTestProtocol::Result::Pass;
            server.recordRaw = (quint32(mask) << 24) | (quint32(mask) << 16) | mask;
            server.recordMeasured = mask;
            QTRY_VERIFY(!window.m_singleTestRefreshPending);
            QVERIFY(!window.m_protectionResetLiveTimer->isActive());
            QCOMPARE(resultText(window, testId), QStringLiteral("PASS"));
            QVERIFY(window.m_lowVoltageInputStatus->text().contains(QStringLiteral("PASS")));
            window.m_client.disconnectFromDevice();
        }
    }
    void lowVoltageInputSelectionAndPartialProgress()
    {
        MainWindow window;
        window.m_singleStageBox->setCurrentIndex(window.m_singleStageBox->findData(
            static_cast<quint16>(DspTestProtocol::Stage::ExternalConnected)));
        window.m_singleTestBox->clear();
        window.m_singleTestBox->addItem(QStringLiteral("LV DI"), 0x0400);
        window.updateSingleTestParameterControls();
        QCOMPARE(window.m_lowVoltageInputBox->count(), 7);
        QCOMPARE(window.m_lowVoltageInputBox->itemData(6).toInt(), 6);
        DspTestProtocol::Response response{};
        response.recordId = 0x0400;
        response.recordResult = DspTestProtocol::Result::Running;
        response.expectedMin = 0x3f;
        response.expectedMax = 0x3f;
        response.rawValue = 0x1f1f003f;
        response.measuredValue = 0x1f;
        window.updateRecord(response);
        QVERIFY(window.m_lowVoltageInputStatus->text().contains(QStringLiteral("DI1：PASS")));
        QVERIFY(!window.m_lowVoltageInputStatus->text().contains(QStringLiteral("DI6：PASS")));
        window.m_singleTestBox->clear();
        window.m_singleTestBox->addItem(QStringLiteral("LV STO"), 0x0403);
        window.updateSingleTestParameterControls();
        QCOMPARE(window.m_lowVoltageInputBox->count(), 3);
        QVERIFY(!window.m_lowVoltageInputStatus->text().contains(QStringLiteral("DI1：PASS")));
    }
    void lowVoltageSciaSerialRouting()
    {
        MainWindow window;
        window.m_boardProfileId = 2U;
        window.m_scibSingleTestRequested = true;
        window.m_singleTestRefreshId = 0x0311;
        QCOMPARE(window.activeSerialTestId(), quint16(0x0311));
        QVERIFY(window.isSerialExternalTest(0x0311));
        QCOMPARE(window.activeSerialName(), QStringLiteral("SCIA/RS485"));
        window.m_scibSingleTestRequested = false;
        QCOMPARE(window.activeSerialTestId(), quint16(0x0309));
        window.m_boardProfileId = 3U;
        QCOMPARE(window.activeSerialTestId(), quint16(0x0316));
    }
    void initTestCase()
    {
        QVERIFY(m_settingsDirectory.isValid());
        QCoreApplication::setOrganizationName(QStringLiteral("HpdRegressionTests"));
        QCoreApplication::setApplicationName(QStringLiteral("ManualConfirmation"));
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                           m_settingsDirectory.path());
    }

    void lowVoltageV04RowsAreRegistered()
    {
        MainWindow window;

        for(quint16 testId = TestLowVoltageFirst;
            testId <= TestLowVoltageLast;
            ++testId) {
            QVERIFY2(window.m_recordRows.contains(testId),
                     qPrintable(QStringLiteral("missing Test ID 0x%1")
                         .arg(testId, 4, 16, QLatin1Char('0'))));
            QVERIFY(window.m_externalRecordIds.contains(testId));
        }

        const auto adcLocation = window.m_recordRows.value(0x0404U);
        QCOMPARE(adcLocation.table->item(adcLocation.row, 1)->text(),
                 QStringLiteral("低压板 / 13路ADC"));
        const auto resolverLocation = window.m_recordRows.value(0x040AU);
        QCOMPARE(resolverLocation.table->item(resolverLocation.row, 1)->text(),
                 QStringLiteral("低压板 / 旋变接口"));
    }

    void manualConfirmationKeepsTcpAlive_data()
    {
        QTest::addColumn<quint16>("testId");
        QTest::addColumn<bool>("automatic");
        QTest::addColumn<int>("decision");
        QTest::newRow("pwm-yes") << TestPwm << false << int(QMessageBox::Yes);
        QTest::newRow("pwm-no") << TestPwm << false << int(QMessageBox::No);
        QTest::newRow("pwm-dismiss") << TestPwm << false << int(QDialog::Rejected);
        QTest::newRow("do-single") << TestDo << false << int(QMessageBox::Yes);
        QTest::newRow("do-auto") << TestDo << true << int(QMessageBox::Yes);
        QTest::newRow("driver-reset") << TestDriverReset << false << int(QMessageBox::Yes);
    }

    void manualConfirmationKeepsTcpAlive()
    {
        QFETCH(quint16, testId);
        QFETCH(bool, automatic);
        QFETCH(int, decision);
        MockDsp server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        MainWindow window;
        QSignalSpy responses(&window.m_client, &DspTcpClient::responseReceived);
        QSignalSpy errors(&window.m_client, &DspTcpClient::protocolError);
        window.m_client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_VERIFY(responses.count() >= 3);
        QSignalSpy connections(&window.m_client, &DspTcpClient::connectionChanged);
        const int initialStatusResponses = server.statusResponses;
        bool found = false;
        bool finished = false;
        QTimer observer;
        observer.setInterval(20);
        connect(&observer, &QTimer::timeout, &window, [&] {
            auto *dialog = window.findChild<QMessageBox *>();
            if (!dialog || !dialog->isVisible() || found) {
                return;
            }
            found = true;
            // Longer than the real 3-second response timeout. Also runs inside
            // the old nested dialog event loop so this test reproduces the bug.
            QTimer::singleShot(4500, dialog, [&, dialog] {
                if (decision == QDialog::Rejected) {
                    dialog->close();
                } else {
                    dialog->button(static_cast<QMessageBox::StandardButton>(decision))->click();
                }
                finished = true;
            });
        });
        observer.start();
        requestManualRecord(window, testId, automatic);
        QTRY_VERIFY_WITH_TIMEOUT(finished, 8000);
        QCOMPARE(errors.count(), 0);
        QCOMPARE(connections.count(), 0);
        QVERIFY(server.statusResponses >= initialStatusResponses + 3);
        QVERIFY(window.m_client.isConnected());
        QCOMPARE(resultText(window, testId), decision == QMessageBox::Yes ?
                     QStringLiteral("PASS") : QStringLiteral("FAIL"));
        QVERIFY(!window.m_singleTestRecordRefreshInFlight);
        QVERIFY(!window.m_edoManualConfirmationAvailable);
        const QString resultBeforeReconnect = resultText(window, testId);
        window.m_client.disconnectFromDevice();
        QTRY_VERIFY(!window.m_client.isConnected());
        const auto responseCount = responses.count();
        window.m_client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_VERIFY(responses.count() >= responseCount + 3);
        QCOMPARE(resultText(window, testId), resultBeforeReconnect);
        window.m_client.disconnectFromDevice();
    }

    void invalidDoResultDoesNotAsk_data()
    {
        QTest::addColumn<int>("result");
        QTest::addColumn<quint16>("error");
        QTest::newRow("unsupported") << int(DspTestProtocol::Result::NotSupported) << quint16(5);
        QTest::newRow("skip") << int(DspTestProtocol::Result::Skip) << quint16(0);
        QTest::newRow("fail") << int(DspTestProtocol::Result::Fail) << quint16(0);
        QTest::newRow("warn-error") << int(DspTestProtocol::Result::Warn) << quint16(1);
        QTest::newRow("not-run") << int(DspTestProtocol::Result::NotRun) << quint16(0);
    }

    void invalidDoResultDoesNotAsk()
    {
        QFETCH(int, result);
        QFETCH(quint16, error);
        MainWindow window;
        window.m_pollTimer->stop();
        DspTestProtocol::Response response;
        response.recordId = TestDo;
        response.recordResult = static_cast<DspTestProtocol::Result>(result);
        response.recordErrorCode = error;
        window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, TestDo);
        window.m_singleTestRecordRefreshInFlight = true;
        window.updateRecord(response);
        QVERIFY(window.m_manualOutputDialog.isNull());
        QVERIFY(!window.m_singleTestRecordRefreshInFlight);

        window.prepareResultRefresh(MainWindow::RefreshScope::External);
        window.m_edoManualConfirmationExpected = true;
        window.updateRecord(response);
        window.requestEdoManualConfirmation();
        QVERIFY(window.m_manualOutputDialog.isNull());
        QVERIFY(!window.m_edoManualConfirmationExpected);
    }

    void missingDoRecordDoesNotAsk()
    {
        MainWindow window;
        window.m_pollTimer->stop();
        window.prepareResultRefresh(MainWindow::RefreshScope::External);
        window.m_edoManualConfirmationExpected = true;
        window.requestEdoManualConfirmation();
        QVERIFY(window.m_manualOutputDialog.isNull());
    }

    void staleConfirmationCannotOverwriteResults()
    {
        MainWindow window;
        window.m_pollTimer->stop();
        DspTestProtocol::Response response;
        response.recordId = TestPwm;
        response.recordResult = DspTestProtocol::Result::Warn;
        auto beginConfirmation = [&] {
            window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, TestPwm);
            window.m_singleTestRecordRefreshInFlight = true;
            window.updateRecord(response);
        };

        beginConfirmation();
        auto oldDialog = window.m_manualOutputDialog;
        QVERIFY(oldDialog);
        window.m_singleTestRefreshId = TestPwm;
        window.m_singleTestRecordRefreshInFlight = true;
        window.updateRecord(response);
        QCOMPARE(window.m_manualOutputDialog.data(), oldDialog.data());

        beginConfirmation();
        QVERIFY(window.m_manualOutputDialog);
        QVERIFY(window.m_manualOutputDialog != oldDialog);
        oldDialog->done(QMessageBox::Yes);
        QCOMPARE(resultText(window, TestPwm), QStringLiteral("MANUAL_CONFIRM"));

        oldDialog = window.m_manualOutputDialog;
        window.resetDisplayedResultsForBoardChange();
        oldDialog->done(QMessageBox::Yes);
        QVERIFY(window.m_manualOutputDialog.isNull());
        QCOMPARE(resultText(window, TestPwm), QStringLiteral("NOT_RUN"));

        beginConfirmation();
        oldDialog = window.m_manualOutputDialog;
        window.prepareResultRefresh(MainWindow::RefreshScope::External);
        oldDialog->done(QMessageBox::No);
        QVERIFY(window.m_manualOutputDialog.isNull());
        QCOMPARE(resultText(window, TestPwm), QStringLiteral("MANUAL_CONFIRM"));

        beginConfirmation();
        response.recordResult = DspTestProtocol::Result::Fail;
        window.updateRecord(response);
        window.m_manualOutputDialog->done(QMessageBox::Yes);
        QCOMPARE(resultText(window, TestPwm), QStringLiteral("FAIL"));
    }

    void pwmRunningRecordDisplaysLiveGpioVoltage()
    {
        MainWindow window;
        window.m_pollTimer->stop();
        window.m_pwmDidoLiveTimer->stop();
        window.m_pwmDidoPendingChannelText =
            QStringLiteral("第一套 U相上管 / PWM_U1H / GPIO16");

        DspTestProtocol::Response response;
        response.recordId = TestPwm;
        response.recordResult = DspTestProtocol::Result::Running;
        response.rawValue = 0x00010003U;
        response.measuredValue = 3.3F;
        response.expectedMin = 1.0F;
        response.expectedMax = 600.0F;
        window.updateRecord(response);

        QCOMPARE(resultText(window, TestPwm), QStringLiteral("RUNNING"));
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("GPIO 高")));
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("逻辑 3.3V")));
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("0.6/2.0s")));
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("端子电压")));

        response.rawValue = 0x00000003U;
        response.measuredValue = 0.0F;
        response.expectedMax = 50.0F;
        window.updateRecord(response);
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("GPIO 低")));
        QVERIFY(window.m_pwmDidoLiveValueLabel->text().contains(
            QStringLiteral("输出已关闭")));
    }

    void liveRecordFinalCompletesSingleRefresh()
    {
        MainWindow window;
        window.m_pollTimer->stop();
        window.prepareSingleTestRefresh(
            MainWindow::RefreshScope::External, TestProtection);
        window.m_singleTestRefreshPending = true;
        window.m_waitingForTestReconnect = true;
        window.m_liveRecordRequestPending = true;
        window.m_protectionResetLiveTimer->start();

        DspTestProtocol::Response response;
        response.command = DspTestProtocol::Command::GetRecord;
        response.protocolStatus = DspTestProtocol::ProtocolStatus::Ok;
        response.recordId = TestProtection;
        response.recordResult = DspTestProtocol::Result::Fail;
        response.rawValue = quint32(0x0A000000U) |
                            (quint32(0x0007U) << 16U);
        window.m_client.responseReceived(response);

        QVERIFY(!window.m_singleTestRefreshPending);
        QVERIFY(!window.m_singleTestRecordRefreshInFlight);
        QVERIFY(!window.m_liveRecordRequestPending);
        QVERIFY(!window.m_protectionResetLiveTimer->isActive());
        QCOMPARE(window.m_singleTestRefreshId,
                 DspTestProtocol::InvalidTestId);
        QCOMPARE(resultText(window, TestProtection), QStringLiteral("FAIL"));
    }

    void genuineNoResponseStillTimesOut()
    {
        MockDsp server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        DspTcpClient client;
        QSignalSpy responses(&client, &DspTcpClient::responseReceived);
        QSignalSpy errors(&client, &DspTcpClient::protocolError);
        client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_COMPARE(responses.count(), 1);
        server.respond = false;
        client.requestStatus();
        QTRY_COMPARE_WITH_TIMEOUT(errors.count(), 1, 4500);
        QVERIFY(!client.isConnected());
        client.disconnectFromDevice();
    }

    void driverFaultChannelsAreIndependent()
    {
        constexpr quint16 testId = 0x0320;
        MainWindow window;
        window.m_pollTimer->stop();
        DspTestProtocol::Response response;
        response.recordId = testId;
        response.recordResult = DspTestProtocol::Result::Pass;
        const QStringList names{QStringLiteral("D_FLT_1U"), QStringLiteral("D_FLT_1D"),
                                QStringLiteral("D_FLT_2U"), QStringLiteral("D_FLT_2D")};
        const QList<quint16> groups{0x0003U, 0x000CU};
        for (quint16 group : groups) {
            window.m_driverFaultGroupBox->setCurrentIndex(group == 0x0003U ? 0 : 1);
            window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, testId);
            QVERIFY(!window.m_driverFaultStatusLabel->text().contains(QStringLiteral("PASS")));
            response.expectedMin = group;
            response.expectedMax = group;
            response.rawValue = (quint32(group) << 16U) | group;
            response.measuredValue = group;
            window.m_singleTestRecordRefreshInFlight = true;
            window.updateRecord(response);
            QCOMPARE(resultText(window, testId), QStringLiteral("PASS"));
            for (int index = 0; index < names.size(); ++index) {
                const quint16 bit = static_cast<quint16>(1U << index);
                QVERIFY(window.m_driverFaultStatusLabel->text().contains(
                    names.at(index) + ((group & bit) != 0U ?
                        QStringLiteral(": PASS") : QStringLiteral(": 未选择"))));
            }
        }

        // Per-channel diagnostics survive a failed group result.
        window.m_driverFaultGroupBox->setCurrentIndex(0);
        window.prepareSingleTestRefresh(MainWindow::RefreshScope::External, testId);
        response.recordResult = DspTestProtocol::Result::Timeout;
        response.recordErrorCode = 0x0186;
        response.expectedMin = 3.0F;
        response.expectedMax = 3.0F;
        response.rawValue = 0x00030003;
        response.measuredValue = 1.0F;
        window.updateRecord(response);
        QVERIFY(window.m_driverFaultStatusLabel->text().contains(
            QStringLiteral("D_FLT_1U: PASS")));
        QVERIFY(window.m_driverFaultStatusLabel->text().contains(
            QStringLiteral("D_FLT_1D: 未通过")));

        response.recordResult = DspTestProtocol::Result::Pass;
        response.recordErrorCode = 0;
        response.rawValue = 0x00030000; // Static high cannot validate a channel.
        response.measuredValue = 0;
        window.updateRecord(response);
        QVERIFY(!window.m_driverFaultStatusLabel->text().contains(QStringLiteral("PASS")));
        window.resetDisplayedResultsForBoardChange();
        QVERIFY(!window.m_driverFaultStatusLabel->text().contains(QStringLiteral("PASS")));
    }

    void singleSelectionIsEncodedInStartRequest()
    {
        MockDsp server;
        QVERIFY(server.listen(QHostAddress::LocalHost));
        DspTcpClient client;
        QSignalSpy responses(&client, &DspTcpClient::responseReceived);
        client.connectToDevice(QStringLiteral("127.0.0.1"), server.serverPort());
        QTRY_VERIFY(responses.count() >= 1);

        for (quint8 selection : {quint8(0), quint8(3), quint8(12)}) {
            server.requests.clear();
            client.startSingle(0x0320,
                               DspTestProtocol::Stage::ExternalConnected,
                               false,
                               selection);
            QTRY_COMPARE(server.requests.size(), 1);
            const QByteArray request = server.requests.constFirst();
            QCOMPARE(static_cast<quint8>(request.at(3)), quint8(0x13));
            QCOMPARE(static_cast<quint8>(request.at(7)) &
                         DspTestProtocol::FlagSingleSelectionMask,
                     static_cast<quint8>(selection <<
                         DspTestProtocol::FlagSingleSelectionShift));
        }
        client.disconnectFromDevice();
    }

    void protectionAndResetControlsMatchPinMatrix()
    {
        MainWindow window;
        window.m_pollTimer->stop();

        QCOMPARE(window.m_protectionSelectionBox->count(), 14);
        for (int index = 0; index < 14; ++index) {
            QCOMPARE(window.m_protectionSelectionBox->itemData(index).toInt(), index);
        }
        QVERIFY(window.m_protectionSelectionBox->itemText(0).contains(
            QStringLiteral("J_IU1 / X3-20")));
        QVERIFY(window.m_protectionSelectionBox->itemText(5).contains(
            QStringLiteral("J_IW2 / X4-22")));
        QVERIFY(window.m_protectionSelectionBox->itemText(6).contains(
            QStringLiteral("0.1V")));
        QVERIFY(window.m_protectionSelectionBox->itemText(12).contains(
            QStringLiteral("J_UDC1 / X3-44")));
        QVERIFY(window.m_protectionSelectionBox->itemText(13).contains(
            QStringLiteral("J_UDC2 / X4-44")));

        QCOMPARE(window.m_driverResetChannelBox->count(), 4);
        const QStringList resetSignals{
            QStringLiteral("D_URST1 / GPIO80"),
            QStringLiteral("D_DRST1 / GPIO81"),
            QStringLiteral("D_URST2 / GPIO78"),
            QStringLiteral("D_DRST2 / GPIO79")};
        for (int index = 0; index < resetSignals.size(); ++index) {
            QCOMPARE(window.m_driverResetChannelBox->itemData(index).toInt(), index);
            QVERIFY(window.m_driverResetChannelBox->itemText(index).contains(
                resetSignals.at(index)));
        }

        window.m_testAvailabilityReady = true;
        window.m_boardProfileId = 3;
        window.m_testAvailability.insert(
            TestProtection,
            DspTestProtocol::TestAvailability::PendingValidation);
        window.m_testAvailability.insert(
            TestDriverReset,
            DspTestProtocol::TestAvailability::PendingValidation);
        window.m_singleStageBox->setCurrentIndex(1);
        window.updateSingleTestChoices(MainWindow::RefreshScope::External);

        const int protectionIndex = window.m_singleTestBox->findData(TestProtection);
        const int resetIndex = window.m_singleTestBox->findData(TestDriverReset);
        QVERIFY(protectionIndex >= 0);
        QVERIFY(resetIndex >= 0);
        window.m_singleTestBox->setCurrentIndex(protectionIndex);
        QVERIFY(!window.m_protectionPanel->isHidden());
        QVERIFY(window.m_driverResetPanel->isHidden());

        DspTestProtocol::Response runningProtection;
        runningProtection.recordId = TestProtection;
        runningProtection.recordResult = DspTestProtocol::Result::Running;
        runningProtection.rawValue = (quint32(2U) << 28U) |
                                     quint32(0x0A000000U) |
                                     (quint32(0x000FU) << 16U);
        runningProtection.measuredValue = 4.9F;
        runningProtection.expectedMin = 1.0F;
        runningProtection.expectedMax = 30.0F;
        window.updateRecord(runningProtection);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("等待保护GPIO响应")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("4.900V")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("阈值已到")));

        DspTestProtocol::Response liveGpioProtection = runningProtection;
        liveGpioProtection.rawValue |= quint32(0x0005U) |
                                              quint32(1U << 6U) |
                                              quint32(1U << 7U);
        window.updateRecord(liveGpioProtection);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("OC1 低")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("OC2 高")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("UOV1 低")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("CLR 高")));

        DspTestProtocol::Response preexistingBreak = runningProtection;
        preexistingBreak.rawValue |= quint32(1U << 5U) |
                                     quint32(1U << 4U);
        window.updateRecord(preexistingBreak);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("BRK预置高")));

        DspTestProtocol::Response blockedBaseline = runningProtection;
        blockedBaseline.rawValue = (quint32(1U) << 28U) |
                                   quint32(0x0A000000U) |
                                   (quint32(0x0001U) << 16U) |
                                   (quint32(0x0001U) << 12U) |
                                   quint32(0x0001U);
        blockedBaseline.measuredValue = 2.5F;
        window.updateRecord(blockedBaseline);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("阻塞")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("D_OC1/GPIO77")));

        DspTestProtocol::Response failedProtection = runningProtection;
        failedProtection.recordResult = DspTestProtocol::Result::Fail;
        failedProtection.rawValue = (quint32(0U) << 28U) |
                                    quint32(0x0A000000U) |
                                    (quint32(0x0007U) << 16U);
        window.updateRecord(failedProtection);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("10秒响应超时")));
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("未通过")));
        const int diagnosticCount = window.m_log->toPlainText().count(
            QStringLiteral("保护诊断："));
        window.updateRecord(failedProtection);
        QCOMPARE(window.m_log->toPlainText().count(
                     QStringLiteral("保护诊断：")),
                 diagnosticCount);

        DspTestProtocol::Response staleProtection = runningProtection;
        staleProtection.rawValue = (quint32(1U) << 28U) |
                                   (quint32(0x0001U) << 16U);
        window.updateRecord(staleProtection);
        QVERIFY(window.m_protectionStatusLabel->text().contains(
            QStringLiteral("DSP固件版本不匹配")));

        window.m_singleTestBox->setCurrentIndex(resetIndex);
        QVERIFY(window.m_protectionPanel->isHidden());
        QVERIFY(!window.m_driverResetPanel->isHidden());

        DspTestProtocol::Response runningReset;
        runningReset.recordId = TestDriverReset;
        runningReset.recordResult = DspTestProtocol::Result::Running;
        runningReset.rawValue = (quint32(0x0001U) << 16U) |
                                quint32(0x0007U);
        runningReset.measuredValue = 3.3F;
        runningReset.expectedMin = 1.0F;
        runningReset.expectedMax = 2500.0F;
        window.updateRecord(runningReset);
        QVERIFY(window.m_driverResetLiveValueLabel->text().contains(
            QStringLiteral("复位GPIO实时值：高")));
        QVERIFY(window.m_driverResetLiveValueLabel->text().contains(
            QStringLiteral("2.5/10.0秒")));
    }

    void longTestNamesExpandWithoutWideningWindow()
    {
        MainWindow window;
        window.m_pollTimer->stop();
        window.m_testAvailabilityReady = true;
        window.m_boardProfileId = 3;
        for (quint16 id : window.m_externalRecordIds) {
            window.m_testAvailability.insert(id, DspTestProtocol::TestAvailability::PendingValidation);
        }
        window.m_singleStageBox->setCurrentIndex(1);
        window.updateSingleTestChoices(MainWindow::RefreshScope::External);
        QVERIFY(window.m_singleTestBox->count() > 5);
        window.m_singleTestBox->setEnabled(true);
        window.m_singleStageBox->setEnabled(true);
        window.resize(1280, 800);
        window.show();
        auto *splitter = qobject_cast<QSplitter *>(window.m_resultTabs->parentWidget()->parentWidget());
        QVERIFY(splitter);
        splitter->setSizes({600, 600});
        QCoreApplication::processEvents();
        const int narrowWidth = window.m_singleTestBox->width();
        splitter->setSizes({900, 300});
        QCoreApplication::processEvents();
        QVERIFY(window.m_singleTestBox->width() > narrowWidth);
        QVERIFY(window.m_singleTestBox->maximumWidth() > window.m_singleTestBox->minimumWidth());

        int longestIndex = 0;
        for (int index = 1; index < window.m_singleTestBox->count(); ++index) {
            if (window.m_singleTestBox->itemText(index).size() >
                window.m_singleTestBox->itemText(longestIndex).size()) {
                longestIndex = index;
            }
        }
        window.m_singleTestBox->setCurrentIndex(longestIndex);
        QCOMPARE(window.m_singleTestBox->toolTip(), window.m_singleTestBox->currentText());
        const QSize windowSize = window.size();
        window.m_singleTestBox->showPopup();
        QCoreApplication::processEvents();
        const int textWidth = window.m_singleTestBox->fontMetrics().horizontalAdvance(
            window.m_singleTestBox->currentText());
        QVERIFY(textWidth > 240);
        const int screenWidth = window.m_singleTestBox->screen()->availableGeometry().width();
        QVERIFY(window.m_singleTestBox->view()->width() >= qMin(textWidth + 32, screenWidth - 32));
        QCOMPARE(window.m_singleTestBox->view()->textElideMode(), Qt::ElideNone);
        QCOMPARE(window.size(), windowSize);
        QCOMPARE(window.m_singleTestBox->itemData(longestIndex, Qt::ToolTipRole).toString(),
                 window.m_singleTestBox->currentText());
        window.m_singleTestBox->view()->window()->grab().save(
            QCoreApplication::applicationDirPath() + QStringLiteral("/test-id-popup.png"));
        window.m_singleTestBox->hidePopup();
        window.m_singleTestBox->parentWidget()->grab().save(
            QCoreApplication::applicationDirPath() + QStringLiteral("/single-test-layout.png"));
    }
};

QTEST_MAIN(MainWindowTests)
#include "MainWindowTests.moc"
