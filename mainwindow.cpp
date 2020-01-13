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
#include "mimesappsmanager.h"

#include <DTitlebar>
#include <DIconButton>
#include <DHorizontalLine>

#include <QLabel>
#include <QScroller>
#include <QCheckBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QPushButton>
#include <QResizeEvent>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QCommandLinkButton>



OpenWithDialogListItem::OpenWithDialogListItem(const QIcon &icon, const QString &text, QWidget *parent)
    : QWidget(parent)
    , m_icon(icon)
{
    if (m_icon.isNull())
        m_icon = QIcon::fromTheme("application-x-desktop");

    m_checkButton = new DIconButton(this);
    m_checkButton->setFixedSize(10, 10);
    m_checkButton->setFlat(true);

    m_label = new QLabel(this);
    m_label->setText(text);

    m_iconLabel = new QLabel(this);
    m_iconLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->setContentsMargins(5, 0, 5, 0);
    layout->addWidget(m_checkButton);
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_label);

    setMouseTracking(true);
}

void OpenWithDialogListItem::setChecked(bool checked)
{
    if (checked) {
        m_checkButton->setIcon(DStyle::SP_MarkElement);
    } else {
        m_checkButton->setIcon(QIcon());
    }
}

QString OpenWithDialogListItem::text() const
{
    return m_label->text();
}

void OpenWithDialogListItem::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    m_iconLabel->setFixedSize(e->size().height() - 20, e->size().height() - 20);
    m_iconLabel->setPixmap(m_icon.pixmap(m_iconLabel->size()));
}

void OpenWithDialogListItem::enterEvent(QEvent *e)
{
    Q_UNUSED(e)

    update();

    return QWidget::enterEvent(e);
}

void OpenWithDialogListItem::leaveEvent(QEvent *e)
{
    Q_UNUSED(e)

    update();

    return QWidget::leaveEvent(e);
}

void OpenWithDialogListItem::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    if (!underMouse())
        return;

    QPainter pa(this);
    QPainterPath path;

    path.addRoundedRect(rect(), 6, 6);
    pa.setRenderHint(QPainter::Antialiasing);
    pa.fillPath(path, QColor(0, 0, 0, static_cast<int>(0.05 * 255)));
}


OpenWithDialogListSparerItem::OpenWithDialogListSparerItem(const QString &title, QWidget *parent)
    : QWidget(parent)
    , m_separator(new DHorizontalLine(this))
    , m_title(new QLabel(title, this))
{
    QFont font;
    font.setPixelSize(18);
    m_title->setFont(font);

    QVBoxLayout *layout = new QVBoxLayout(this);

    layout->addWidget(m_separator);
    layout->addWidget(m_title);
    layout->setContentsMargins(20, 0, 20, 0);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}


MainWindow::MainWindow(QWidget *parent) : DAbstractDialog(parent)
{
    m_titlebar = new DTitlebar(this);
    m_titlebar->setBackgroundTransparent(true);
    m_titlebar->setTitle(tr("Open with"));

    setWindowFlags(windowFlags()
                           &~ Qt::WindowMaximizeButtonHint
                           &~ Qt::WindowMinimizeButtonHint
                           &~ Qt::WindowSystemMenuHint);
    initUI();
    initConnect();
    initData();
}

MainWindow::~MainWindow()
{

}

void MainWindow::openFileByApp()
{
    if (!m_checkedItem)
        return;

    const QString &app = m_checkedItem->property("app").toString();

    if (m_setToDefaultCheckBox->isChecked())
        mimeAppsManager->setDefautlAppForTypeByGio(m_mimeType.name(), app);

//    if (DFileService::instance()->openFileByApp(this, app, m_url))
//        close();
}

void MainWindow::showEvent(QShowEvent *event)
{
    m_recommandLayout->parentWidget()->setFixedWidth(m_scrollArea->width());
    m_recommandLayout->parentWidget()->resize(m_recommandLayout->parentWidget()->layout()->sizeHint());

    DAbstractDialog::showEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    // blumia: for m_scrollArea, to avoid touch screen scrolling cause window move
    if (event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->source() == Qt::MouseEventSynthesizedByQt) {
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonPress) {

        if (static_cast<QMouseEvent*>(event)->button() == Qt::LeftButton) {
            if (OpenWithDialogListItem *item = qobject_cast<OpenWithDialogListItem*>(obj))
                checkItem(item);

            return true;
        }
    }

    return false;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_titlebar->setFixedWidth(event->size().width());

    DAbstractDialog::resizeEvent(event);
}

void MainWindow::initUI()
{
    setFixedSize(710, 450);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setWidgetResizable(true);
    QScroller::grabGesture(m_scrollArea);
    m_scrollArea->installEventFilter(this);

    QWidget *content_widget = new QWidget;

    content_widget->setObjectName("contentWidget");
    content_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_scrollArea->setWidget(content_widget);

    m_recommandLayout = new DFlowLayout;
    m_otherLayout = new DFlowLayout;

    m_openFileChooseButton = new QCommandLinkButton(tr("Add other programs"));
    m_setToDefaultCheckBox = new QCheckBox(tr("Set as default"));
    m_setToDefaultCheckBox->setChecked(true);
    m_cancelButton = new QPushButton(tr("Cancel"));
    m_chooseButton = new QPushButton(tr("Confirm"));

    QVBoxLayout* content_layout = new QVBoxLayout;
    content_layout->setContentsMargins(10, 0, 10, 0);
    content_layout->addWidget(new OpenWithDialogListSparerItem(tr("Recommended Applications"), this));
    content_layout->addLayout(m_recommandLayout);
    content_layout->addWidget(new OpenWithDialogListSparerItem(tr("Other Applications"), this));
    content_layout->addLayout(m_otherLayout);
    content_layout->addStretch();

    content_widget->setLayout(content_layout);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(m_openFileChooseButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_setToDefaultCheckBox);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_chooseButton);
    buttonLayout->setContentsMargins(10, 0, 10, 0);

    QVBoxLayout *main_layout= new QVBoxLayout(this);
    QVBoxLayout *bottom_layout = new QVBoxLayout;

    bottom_layout->addWidget(new DHorizontalLine(this));
    bottom_layout->addSpacing(5);
    bottom_layout->addLayout(buttonLayout);
    bottom_layout->setContentsMargins(10, 0, 10, 0);

    main_layout->addWidget(m_scrollArea);
    main_layout->addLayout(bottom_layout);
    main_layout->setContentsMargins(0, 35, 0, 10);
}

void MainWindow::initConnect()
{
    connect(m_cancelButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(m_chooseButton, &QPushButton::clicked, this, &MainWindow::openFileByApp);
    connect(m_openFileChooseButton, &QCommandLinkButton::clicked, this, &MainWindow::useOtherApplication);
}

void MainWindow::initData()
{

    QMimeDatabase db;
    m_mimeType = db.mimeTypeForUrl(m_url);

    //m_url file is desktopFile, what To DO ?

//    if (file_info->isDesktopFile())
//        m_setToDefaultCheckBox->hide();

    const QString &default_app = mimeAppsManager->getDefaultAppByMimeType(m_mimeType);
    const QStringList &recommendApps = mimeAppsManager->getRecommendedAppsByQio(m_mimeType);

    for (int i = 0; i < recommendApps.count(); ++i) {
        const DesktopFile desktop_info = mimeAppsManager->DesktopObjs.value(recommendApps.at(i));

        OpenWithDialogListItem *item = createItem(QIcon::fromTheme(desktop_info.getIcon()), desktop_info.getDisplayName(), recommendApps.at(i));
        m_recommandLayout->addWidget(item);

        if (!default_app.isEmpty() && recommendApps.at(i).endsWith(default_app))
            checkItem(item);
    }

    QList<DesktopFile> other_app_list;

    foreach (const QString& f, mimeAppsManager->DesktopObjs.keys()) {
        //filter recommend apps , no show apps and no mime support apps
        const DesktopFile& app = mimeAppsManager->DesktopObjs.value(f);
        if(recommendApps.contains(f))
            continue;

        if(mimeAppsManager->DesktopObjs.value(f).getNoShow())
            continue;

        if(mimeAppsManager->DesktopObjs.value(f).getMimeType().isEmpty())
            continue;

        bool isSameDesktop = false;
        foreach (const DesktopFile& otherApp, other_app_list) {
            if(otherApp.getExec() == app.getExec() && otherApp.getLocalName() == app.getLocalName())
                isSameDesktop = true;
        }

        DDesktopEntry desktop_info(f);

        const QString &custom_open_desktop = desktop_info.stringValue("X-DDE-File-Manager-Custom-Open");

        // Filter self own desktop files for opening other types of files
        if (!custom_open_desktop.isEmpty() && custom_open_desktop != m_mimeType.name())
            continue;

        if (isSameDesktop)
            continue;

        other_app_list << mimeAppsManager->DesktopObjs.value(f);
        QString iconName = other_app_list.last().getIcon();
        OpenWithDialogListItem *item = createItem(QIcon::fromTheme(iconName), other_app_list.last().getDisplayName(), f);
        m_otherLayout->addWidget(item);

        if (!default_app.isEmpty() && f.endsWith(default_app))
            checkItem(item);
    }
}

void MainWindow::checkItem(OpenWithDialogListItem *item)
{
    if (m_checkedItem)
        m_checkedItem->setChecked(false);

    item->setChecked(true);
    m_checkedItem = item;
}

void MainWindow::useOtherApplication()
{
    const QString &file_path = QFileDialog::getOpenFileName(this);

    if (file_path.isEmpty())
        return;

    QFileInfo info(file_path);
    QString target_desktop_file_name("%1/%2-custom-open-%3.desktop");

    target_desktop_file_name = target_desktop_file_name.arg(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)).arg(qApp->applicationName()).arg(m_mimeType.name().replace("/", "-"));

    if (file_path.endsWith(".desktop")) {
        for (const MainWindow *w : m_recommandLayout->parentWidget()->findChildren<MainWindow*>()) {
            if (w->property("app").toString() == file_path)
                return;
        }

        DDesktopEntry desktop(file_path);

        if (desktop.stringValue("MimeType").isEmpty())
            return;

        if (!QFile::link(file_path, target_desktop_file_name))
            return;
    } else if (info.isExecutable()) {
        DDesktopEntry desktop(target_desktop_file_name);

        desktop.setStringValue("Type", "Application");
        desktop.setStringValue("Name", info.fileName());
        desktop.setStringValue("Icon", "application-x-desktop");
        desktop.setStringValue("Exec", file_path);
        desktop.setStringValue("MimeType", "*/*");
        desktop.setStringValue("X-DDE-File-Manager-Custom-Open", m_mimeType.name());

        if (QFile::exists(target_desktop_file_name))
            QFile(target_desktop_file_name).remove();

        if (!desktop.save())
            return;
    }

    // remove old custom item
    for (int i = 0; i < m_otherLayout->count(); ++i) {
        QWidget *w = m_otherLayout->itemAt(i)->widget();

        if (!w)
            continue;

        if (w->property("app").toString() == target_desktop_file_name) {
            m_otherLayout->removeWidget(w);
            w->deleteLater();
        }
    }

    OpenWithDialogListItem *item = createItem(QIcon::fromTheme("application-x-desktop"), info.fileName(), target_desktop_file_name);

    int other_layout_sizeHint_height = m_otherLayout->sizeHint().height();
    m_otherLayout->addWidget(item);
    item->show();
    m_otherLayout->parentWidget()->setFixedHeight(m_otherLayout->parentWidget()->height() + m_otherLayout->sizeHint().height() - other_layout_sizeHint_height);
    checkItem(item);
}

OpenWithDialogListItem *MainWindow::createItem(const QIcon &icon, const QString &name, const QString &filePath)
{
    OpenWithDialogListItem *item = new OpenWithDialogListItem(icon, name, this);

    item->setProperty("app", filePath);
    item->setFixedSize(220, 50);
    item->installEventFilter(this);

    return item;
}
