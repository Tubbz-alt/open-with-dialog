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


#include "commandlinemanager.h"

CommandLineManager::CommandLineManager() :
    m_mimeTypeOption({"m", "mimetype"}, QCoreApplication::translate("main", "please give a mimetype name when you want to open a file with the specified mimetype."), "name")
{
    m_commandLineParser.setApplicationDescription("Open a file from a specific application. \n"
                                                  "Example: ./open-with-dialog file:///home/liuyang/Music/abcd.txt --mimetype=text/plain");
    m_commandLineParser.addHelpOption();
    m_commandLineParser.addVersionOption();
    m_commandLineParser.addOption(m_mimeTypeOption);
//    m_commandLineParser.addPositionalArgument("file", QCoreApplication::translate("main", "The file to open."));
    m_commandLineParser.addPositionalArgument("urls", QCoreApplication::translate("main", "URLs to open, can give a few of urls. [example: ./exec  URL1 URL2 ...] "), "[urls...]");
}

void CommandLineManager::process(const QCoreApplication &app)
{
    m_commandLineParser.process(app);
}

void CommandLineManager::process(const QStringList &list)
{
    m_commandLineParser.process(list);
}

QString CommandLineManager::mimeTypeData()
{
    return m_commandLineParser.value(m_mimeTypeOption);
}

QStringList CommandLineManager::urlsData()
{
    return m_commandLineParser.positionalArguments();
}

QString CommandLineManager::helpText()
{
    return m_commandLineParser.helpText();
}

void CommandLineManager::showHelp()
{
    m_commandLineParser.showHelp();
}
