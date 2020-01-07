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


#include "mainwindow.h"

#include <DApplication>
#include <DApplicationSettings>
#include <DLog>

#include <QDebug>
#include <QCommandLineParser>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

DCORE_USE_NAMESPACE

int main(int argc, char *argv[])
{

    DApplication::loadDXcbPlugin();
    DApplication app(argc,argv);
    app.setQuitOnLastWindowClosed(true);

    app.setOrganizationName("deepin");
    app.setApplicationName(QObject::tr("Deepin Open With Dialog"));
    app.setApplicationVersion("v1.0");

#ifdef QT_DEBUG
    QTranslator translator;
    translator.load(QString("dterm_%1").arg(QLocale::system().name()));
    app.installTranslator(&translator);
#endif // QT_DEBUG

    //Logger handle
    DLogManager::registerConsoleAppender();

    MainWindow mw;
    mw.show();


    qDebug() << "Hello World!";

    return app.exec();
}
