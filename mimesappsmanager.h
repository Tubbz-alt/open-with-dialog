#ifndef MIMESAPPSMANAGER_H
#define MIMESAPPSMANAGER_H

#include "singleton.h"
#include "dtkwidget_global.h"

#include <QMap>
#include <QUrl>
#include <QTimer>
#include <QObject>
#include <QString>
#include <QMimeType>
#include <QFileInfo>
#include <QStringList>
#include <QFileSystemWatcher>

#include <DDesktopEntry>

#define mimeAppsManager Singleton<MimesAppsManager>::instance()

DCORE_USE_NAMESPACE
//DWIDGET_USE_NAMESPACE

class MimeAppsWorker: public QObject
{
   Q_OBJECT

public:
    MimeAppsWorker(QObject *parent = nullptr);
    ~MimeAppsWorker();

    void initConnect();

public slots:
    void startWatch();
    void handleDirectoryChanged(const QString& filePath);
    void handleFileChanged(const QString& filePath);
    void updateCache();
    void writeData(const QString& path, const QByteArray& content);
    QByteArray readData(const QString& path);


private:
    QFileSystemWatcher* m_fileSystemWatcher = nullptr;
    QTimer* m_updateCacheTimer;

};


class MimesAppsManager : public QObject
{
    Q_OBJECT
public:
    explicit MimesAppsManager(QObject *parent = nullptr);
    ~MimesAppsManager();

    static QStringList DesktopFiles;
    static QMap<QString, QStringList> MimeApps;
    static QMap<QString, QStringList> DDE_MimeTypes;
//    specially cache for video, image, text and audio
//    static QMap<QString, DDesktopEntry> VideoMimeApps;
//    static QMap<QString, DDesktopEntry> ImageMimeApps;
//    static QMap<QString, DDesktopEntry> TextMimeApps;
//    static QMap<QString, DDesktopEntry> AudioMimeApps;
//    static QMap<QString, DDesktopEntry> DesktopObjs;

    static QMimeType getMimeType(const QString& fileName);
    static QString getMimeTypeByFileName(const QString& fileName);
    static QString getDefaultAppByFileName(const QString& fileName);
    static QString getDefaultAppByMimeType(const QMimeType& mimeType);
    static QString getDefaultAppByMimeType(const QString& mimeType);
    static QString getDefaultAppDisplayNameByMimeType(const QMimeType& mimeType);
    static QString getDefaultAppDisplayNameByGio(const QString& mimeType);
    static QString getDefaultAppDesktopFileByMimeType(const QString& mimeType);

    static bool setDefautlAppForTypeByGio(const QString& mimeType,
                                     const QString& targetAppName);

    static QStringList getRecommendedApps(const QUrl& url);
    static QStringList getRecommendedAppsByQio(const QMimeType& mimeType);
    static QStringList getRecommendedAppsByGio(const QString& mimeType);
//    static QStringList getrecommendedAppsFromMimeWhiteList(const QUrl& url);


    static QStringList getApplicationsFolders();
//    static QString getMimeAppsCacheFile();
    static QString getMimeInfoCacheFilePath();
    static QString getMimeInfoCacheFileRootPath();
//    static QString getDesktopFilesCacheFile();
//    static QString getDesktopIconsCacheFile();
    static QStringList getDesktopFiles();
    static QString getDDEMimeTypeFile();
//    static QMap<QString, DDesktopEntry> getDesktopObjs();
    static void initMimeTypeApps();
    static void loadDDEMimeTypes();
    static bool lessByDateTime(const QFileInfo& f1,  const QFileInfo& f2);
    static bool removeOneDupFromList(QStringList& list, const QString desktopFilePath);

signals:
    void requestUpdateCache();

public slots:

private:
    MimeAppsWorker* m_mimeAppsWorker=nullptr;
};

#endif // MIMESAPPSMANAGER_H
