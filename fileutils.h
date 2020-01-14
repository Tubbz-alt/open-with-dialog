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


#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>


class FileUtils
{
public:
    static bool openFilesByApp(const QString& desktopFile, const QStringList &filePaths);

    static bool openFile(const QString& filePath);
    static bool launchApp(const QString& desktopFile, const QStringList& filePaths = {}); // open filePaths by desktopFile
    static bool launchAppByDBus(const QString& desktopFile, const QStringList& filePaths = {});
    static bool launchAppByGio(const QString& desktopFile, const QStringList& filePaths = {});

    static QString defaultTerminalPath();
    static bool isOpenWithDlgSelf(const QString &desktopFile);
    static QString getFileMimetype(const QString& path);
};

#endif // FILEUTILS_H
