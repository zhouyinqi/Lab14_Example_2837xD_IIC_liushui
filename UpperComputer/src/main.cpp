#include "MainWindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QSettings>

namespace {

struct LegacyZlgCanSettings
{
    QString libraryPath;
    QVariant deviceType;
    QVariant deviceIndex;
};

LegacyZlgCanSettings readLegacyZlgCanSettings()
{
    QSettings legacySettings;
    LegacyZlgCanSettings settings;

    settings.libraryPath =
        legacySettings.value(QStringLiteral("zlgCan/libraryPath")).toString();
    settings.deviceType = legacySettings.value(QStringLiteral("zlgCan/deviceType"));
    settings.deviceIndex = legacySettings.value(QStringLiteral("zlgCan/deviceIndex"));
    return settings;
}

void migrateZlgCanSettings(const LegacyZlgCanSettings &legacy)
{
    QSettings settings;
    if(!settings.contains(QStringLiteral("zlgCan/libraryPath")) &&
       !legacy.libraryPath.isEmpty()) {
        settings.setValue(QStringLiteral("zlgCan/libraryPath"), legacy.libraryPath);
    }
    if(!settings.contains(QStringLiteral("zlgCan/deviceType")) &&
       legacy.deviceType.isValid()) {
        settings.setValue(QStringLiteral("zlgCan/deviceType"), legacy.deviceType);
    }
    if(!settings.contains(QStringLiteral("zlgCan/deviceIndex")) &&
       legacy.deviceIndex.isValid()) {
        settings.setValue(QStringLiteral("zlgCan/deviceIndex"), legacy.deviceIndex);
    }
    settings.sync();
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    const LegacyZlgCanSettings legacyZlgCanSettings = readLegacyZlgCanSettings();
    QCoreApplication::setOrganizationName(QStringLiteral("CRRC"));
    QCoreApplication::setApplicationName(QStringLiteral("HpdBoardTestHost"));
    migrateZlgCanSettings(legacyZlgCanSettings);
    MainWindow window;
    window.show();
    return application.exec();
}
