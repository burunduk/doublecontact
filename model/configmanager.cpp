/* Double Contact
 *
 * Module: Configuration manager
 *
 * Copyright 2016 Mikhail Y. Zvyozdochkin aka DarkHobbit <pub@zvyozdochkin.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version. See COPYING file for more details.
 *
 */

#include <QLocale>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#include "configmanager.h"
#include "globals.h"
#include "contactlist.h"

ConfigManager::ConfigManager()
    :settings("DarkHobbit", "doublecontact")
{
}

void ConfigManager::readConfig()
{
    // Locale
    gd.dateFormat = settings.value("Locale/DateFormat", QLocale::system().dateFormat()).toString();
    gd.timeFormat = settings.value("Locale/TimeFormat", QLocale::system().timeFormat()).toString();
    gd.useSystemDateTimeFormat = settings.value("Locale/UseSystemDateTimeFormat", true).toBool();
    updateFormats();
    // Misc
    gd.openLastFilesAtStartup = settings.value("General/OpenLastFilesAtStartup", true).toBool();
    // For column view
    validColumnNames.clear();
    for (int i=0; i<ccLast; i++)
         validColumnNames << contactColumnHeaders[i];
    // Column view
    gd.columnNames.clear();
    int visibleColumnCount = settings.value("VisibleColumns/Count", 0).toInt();
    for (int i=0; i<visibleColumnCount; i++) { // Fill visible columns list
        QString columnCandidate = settings.value(QString("VisibleColumns/Column%1").arg(i+1)).toString();
        if (validColumnNames.contains(columnCandidate))
            gd.columnNames.push_back((ContactColumn)validColumnNames.indexOf(columnCandidate));
    }
    if (gd.columnNames.count()==0) { // if list is empty, set default
        gd.columnNames.push_back(ccLastName);
        gd.columnNames.push_back(ccFirstName);
        gd.columnNames.push_back(ccPhone);
    }
    // Saving
    QString sPrefVer = settings.value("Saving/PreferredVCardVersion", "2.1").toString();
    if (sPrefVer=="2.1")
        gd.preferredVCFVersion = GlobalConfig::VCF21;
    else
        gd.preferredVCFVersion = GlobalConfig::VCF30;
    // TODO 4.0
    gd.useOriginalFileVersion = settings.value("Saving/UseOriginalFileVCardVersion", true).toBool();
    gd.defaultCountryRule = settings.value("Saving/DefaultCountryRule", 0).toInt();
    gd.skipTimeFromDate = settings.value("Saving/SkipTimeFromDate", false).toBool();
    gd.addXToNonStandardTypes = settings.value("Saving/AddXToNonStandardTypes", false).toBool();
    gd.replaceNLNSNames = settings.value("Saving/ReplaceNLNSNames", false).toBool();
    // Loading
    gd.defaultEmptyPhoneType = settings.value("Loading/DefaultEmptyPhoneType",
        Phone::standardTypes.translate("voice")).toString(); // many phones treat type 'voice' as 'other'
    gd.warnOnNonStandardTypes = settings.value("Loading/WarnOnNonStandardTypes", true).toBool();
}

void ConfigManager::writeConfig()
{
    // Locale
    updateFormats();
    settings.setValue("Locale/DateFormat", gd.dateFormat);
    settings.setValue("Locale/TimeFormat", gd.timeFormat);
    settings.setValue("Locale/UseSystemDateTimeFormat", gd.useSystemDateTimeFormat);
    // Misc
    settings.setValue("General/OpenLastFilesAtStartup", gd.openLastFilesAtStartup);
    // Column view
    settings.setValue("VisibleColumns/Count", gd.columnNames.count());
    for (int i=0; i<gd.columnNames.count(); i++)
        settings.setValue(QString("VisibleColumns/Column%1").arg(i+1), contactColumnHeaders[gd.columnNames[i]]);
    // Saving
    QString sPrefVer;
    switch (gd.preferredVCFVersion) {
    case GlobalConfig::VCF21:
        sPrefVer = "2.1";
        break;
    case GlobalConfig::VCF30:
        sPrefVer = "3.0";
        break;
        // TODO 4.0
    default:
        sPrefVer = "2.1";
        break;
    }
    settings.setValue("Saving/PreferredVCardVersion", sPrefVer);
    settings.setValue("Saving/UseOriginalFileVCardVersion", gd.useOriginalFileVersion);
    settings.setValue("Saving/DefaultCountryRule", gd.defaultCountryRule);
    settings.setValue("Saving/SkipTimeFromDate", gd.skipTimeFromDate);
    settings.setValue("Saving/AddXToNonStandardTypes", gd.addXToNonStandardTypes);
    settings.setValue("Saving/ReplaceNLNSNames", gd.replaceNLNSNames);
    // Loading
    settings.setValue("Loading/DefaultEmptyPhoneType", gd.defaultEmptyPhoneType);
    settings.setValue("Loading/WarnOnNonStandardTypes", gd.warnOnNonStandardTypes);
}

QString ConfigManager::readLanguage()
{
    return settings.value("General/Language", "").toString();
}

void ConfigManager::writeLanguage(const QString &language)
{
    settings.setValue("General/Language", language);
}

QString ConfigManager::lastContactFile()
{
    return settings.value("General/LastContactFile", defaultDocDir()).toString();
}

void ConfigManager::setLastContactFile(const QString &path)
{
    settings.setValue("General/LastContactFile", path);
}

QString ConfigManager::lastImageFile()
{
    return settings.value("General/LastImageFile", defaultImageDir()).toString();
}

void ConfigManager::setLastImageFile(const QString &path)
{
    settings.setValue("General/LastImageFile", path);
}

bool ConfigManager::showTwoPanels()
{
    return settings.value("General/ShowTwoPanels", true).toBool();
}

void ConfigManager::setShowTwoPanels(bool value)
{
    settings.setValue("General/ShowTwoPanels", value);
}

bool ConfigManager::sortingEnabled()
{
    return settings.value("General/SortingEnabled", false).toBool();
}

void ConfigManager::setSortingEnabled(bool value)
{
    settings.setValue("General/SortingEnabled", value);

}

QString ConfigManager::defaultDocDir()
{
    return
        #if QT_VERSION >= 0x050000
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        #else
                QDesktopServices::storageLocation( QDesktopServices::DocumentsLocation);
        #endif
}

QString ConfigManager::defaultImageDir()
{
    return
        #if QT_VERSION >= 0x050000
                QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
        #else
                QDesktopServices::storageLocation( QDesktopServices::PicturesLocation);
        #endif
}

void ConfigManager::updateFormats()
{
    if (gd.useSystemDateTimeFormat) {
        gd.dateFormat = QLocale::system().dateFormat();
        gd.timeFormat = QLocale::system().timeFormat();
    }
}

ConfigManager configManager;
