/*
 * Copyright (C) 2016 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuyang<liuyang_cm@deepin.com>
 *
 * Maintainer: liuyang<liuyang_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "fileutils.h"
#include <mimesappsmanager.h>

#include <QUrl>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDesktopServices>

#undef signals
extern "C" {
    #include <gio/gio.h>
    #include <gio/gdesktopappinfo.h>
}
#define signals public


bool FileUtils::openFilesByApp(const QString& desktopFile, const QStringList& filePaths)
{
    bool ok = false;

    if (desktopFile.isEmpty()) {
        qDebug() << "Failed to open desktop file with gio: app file path is empty";
        return ok;
    }

    if (filePaths.isEmpty()) {
        qDebug() << "Failed to open desktop file with gio: file path is empty";
        return ok;
    }

    qDebug() << desktopFile << filePaths;

    GDesktopAppInfo* appInfo = g_desktop_app_info_new_from_filename(desktopFile.toLocal8Bit().constData());
    if (!appInfo) {
        qDebug() << "Failed to open desktop file with gio: g_desktop_app_info_new_from_filename returns NULL. Check PATH maybe?";
        return false;
    }

    QString terminalFlag = QString(g_desktop_app_info_get_string(appInfo, "Terminal"));
    if (terminalFlag == "true"){
        QString exec = QString(g_desktop_app_info_get_string(appInfo, "Exec"));
        QStringList args;
        args << "-e" << exec.split(" ").at(0) << filePaths;
        QString termPath = defaultTerminalPath();
        qDebug() << termPath << args;
        ok = QProcess::startDetached(termPath, args);
    }else{
        ok = launchApp(desktopFile, filePaths);
    }
    g_object_unref(appInfo);

    return ok;
}

bool FileUtils::openFile(const QString& filePath)
{
    bool result = false;
    if (QFileInfo(filePath).suffix() == "desktop"){
        result = FileUtils::launchApp(filePath);
        return result;
    }

    QString mimetype = getFileMimetype(filePath);
    QString defaultDesktopFile = MimesAppsManager::getDefaultAppDesktopFileByMimeType(mimetype);
    if (defaultDesktopFile.isEmpty()) {
        qDebug() << "no default application for" << filePath;
        return false;
    }

    if (isOpenWithDlgSelf(defaultDesktopFile) && mimetype != "inode/directory"){
        QStringList recommendApps = mimeAppsManager->getRecommendedApps(QUrl::fromLocalFile(filePath));
        recommendApps.removeOne(defaultDesktopFile);
        if (recommendApps.count() > 0){
            defaultDesktopFile = recommendApps.first();
        }else{
            qDebug() << "no default application for" << filePath;
            return false;
        }
    }

    result = launchApp(defaultDesktopFile, QStringList() << QUrl::fromLocalFile(filePath).toString());
    if (result){
        // workaround since DTK apps doesn't support the recent file spec.
        // spec: https://www.freedesktop.org/wiki/Specifications/desktop-bookmark-spec/
        // the correct approach: let the app add it to the recent list.
        // addToRecentFile(DUrl::fromLocalFile(filePath), mimetype);
        return result;
    }

    if (mimeAppsManager->getDefaultAppByFileName(filePath) == "org.gnome.font-viewer.desktop"){
        QProcess::startDetached("gio", QStringList() << "open" << filePath);
        QTimer::singleShot(200, [=]{
            QProcess::startDetached("gio", QStringList() << "open" << filePath);
        });
        return true;
    }

    result = QProcess::startDetached("gio", QStringList() << "open" << filePath);

    if (!result)
        return QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    return result;
}

bool FileUtils::launchApp(const QString& desktopFile, const QStringList& filePaths)
{
    if (isOpenWithDlgSelf(desktopFile) && filePaths.count() > 1){
        foreach(const QString& filePath, filePaths){
            openFile(QUrl(filePath).toLocalFile());
        }
        return true;
    }

    bool ok = launchAppByDBus(desktopFile, filePaths);
    if (!ok){
        ok = launchAppByGio(desktopFile, filePaths);
    }
    return ok;
}

bool FileUtils::launchAppByDBus(const QString& desktopFile, const QStringList& filePaths)
{
//    if (appController->hasLaunchAppInterface()){
//        qDebug() << "launchApp by dbus:" << desktopFile << filePaths;
//        appController->startManagerInterface()->LaunchApp(desktopFile, QX11Info::getTimestamp(), filePaths);
//        return true;
//    }
    return false;
}

bool FileUtils::launchAppByGio(const QString& desktopFile, const QStringList& filePaths)
{
    qDebug() << "launchApp by gio:" << desktopFile << filePaths;

    std::string stdDesktopFilePath = desktopFile.toStdString();
    const char* cDesktopPath = stdDesktopFilePath.data();

    GDesktopAppInfo* appInfo = g_desktop_app_info_new_from_filename(cDesktopPath);
    if (!appInfo) {
        qDebug() << "Failed to open desktop file with gio: g_desktop_app_info_new_from_filename returns NULL. Check PATH maybe?";
        return false;
    }

    GList* g_files = nullptr;
    foreach (const QString& filePath, filePaths) {
        std::string stdFilePath = filePath.toStdString();
        const char* cFilePath = stdFilePath.data();
        GFile* f = g_file_new_for_uri(cFilePath);
        g_files = g_list_append(g_files, f);
    }

    GError* gError = nullptr;
    gboolean ok = g_app_info_launch(reinterpret_cast<GAppInfo*>(appInfo), g_files, nullptr, &gError);

    if (gError) {
        qWarning() << "Error when trying to open desktop file with gio:" << gError->message;
        g_error_free(gError);
    }

    if (!ok) {
        qWarning() << "Failed to open desktop file with gio: g_app_info_launch returns false";
    }
    g_object_unref(appInfo);
    g_list_free(g_files);

    return ok;
}

QString FileUtils::defaultTerminalPath()
{
    const static QString dde_daemon_default_term = QStringLiteral("/usr/lib/deepin-daemon/default-terminal");
    const static QString debian_x_term_emu = QStringLiteral("/usr/bin/x-terminal-emulator");

    if (QFileInfo::exists(dde_daemon_default_term)) {
        return dde_daemon_default_term;
    } else if (QFileInfo::exists(debian_x_term_emu)) {
        return debian_x_term_emu;
    }

    return QStandardPaths::findExecutable("xterm");
}

bool FileUtils::isOpenWithDlgSelf(const QString &desktopFile)
{
    Q_UNUSED(desktopFile)

    return false;
}

QString FileUtils::getFileMimetype(const QString& path)
{
    GFile *file;
    GFileInfo *info;
    QString result;

    file = g_file_new_for_path(path.toUtf8());
    info = g_file_query_info(file, "standard::content-type", G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
    result = g_file_info_get_content_type(info);

    g_object_unref(file);

    return result;
}
