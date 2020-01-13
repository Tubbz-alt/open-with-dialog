#ifndef MAINWINDOW_H
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


#define MAINWINDOW_H

#include <dflowlayout.h>

#include <QUrl>
#include <QMimeType>
#include <DAbstractDialog>

QT_BEGIN_NAMESPACE
class QLabel;
class QScrollArea;
class QCheckBox;
class QPushButton;
class QCommandLinkButton;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DTitlebar;
class DIconButton;
class DHorizontalLine;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class OpenWithDialogListItem : public QWidget
{
    Q_OBJECT

public:
    explicit OpenWithDialogListItem(const QIcon &icon, const QString &text, QWidget *parent = nullptr);

    void setChecked(bool checked);
    QString text() const;

protected:
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void enterEvent(QEvent *e) Q_DECL_OVERRIDE;
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    QIcon m_icon;
    DIconButton *m_checkButton;
    QLabel *m_iconLabel;
    QLabel *m_label;
};


class OpenWithDialogListSparerItem : public QWidget
{
public:
    explicit OpenWithDialogListSparerItem(const QString &title, QWidget *parent = nullptr);

private:
    DHorizontalLine *m_separator;
    QLabel *m_title;
};


class MainWindow : public DAbstractDialog
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void openFileByApp();

protected:
    void showEvent(QShowEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void initUI();
    void initConnect();
    void initData();
    void checkItem(OpenWithDialogListItem *item);
    void useOtherApplication();
    OpenWithDialogListItem *createItem(const QIcon &icon, const QString &name, const QString &filePath);

    QCheckBox *m_setToDefaultCheckBox = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    DFlowLayout *m_recommandLayout = nullptr;
    DFlowLayout *m_otherLayout = nullptr;
    QCommandLinkButton *m_openFileChooseButton = nullptr;
    QPushButton *m_cancelButton = nullptr;
    QPushButton *m_chooseButton = nullptr;

    QUrl m_url;
    QMimeType m_mimeType;

    OpenWithDialogListItem *m_checkedItem = nullptr;
    DTitlebar *m_titlebar = nullptr;
};

#endif // MAINWINDOW_H
