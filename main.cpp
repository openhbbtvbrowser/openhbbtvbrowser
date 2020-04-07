#include "browsercontrol.h"
#include "browserwindow.h"
#include "webview.h"
#include <QApplication>
#include <QDir>
#include <QLockFile>
#include <QUrl>
#include <QWebEngineSettings>
#include <QWebEngineProfile>

#if defined(Q_OS_LINUX)
#include <sys/stat.h>
#include <sys/mount.h>
#include <errno.h>
#include <stddef.h>

int mkdir_mount_devshm(void)
{
    const char mountpoint[] = "/dev/shm";
    struct stat s;

    if (stat(mountpoint, &s) == -1) {
        if (errno == ENOENT) {
            if (mkdir(mountpoint, 0755))
                return -1;

            if (mount("tmpfs", mountpoint, "tmpfs", 0, NULL))
                return -1;
        } else {
            return -1;
        }
    }

    return 0;
}
#endif

const QString userAgent =
    QStringLiteral("HbbTV/1.4.1 (+DRM;Samsung;SmartTV2015;T-HKM6DEUC-1490.3;;) HybridTvViewer");

QUrl commandLineUrlArgument()
{
    const QStringList args = QCoreApplication::arguments();
    for (const QString &arg : args.mid(1)) {
        if (!arg.startsWith(QLatin1Char('-')))
            return QUrl::fromUserInput(arg);
    }
    return QUrl();
}

int main(int argc, char *argv[])
{
#if defined(Q_OS_LINUX)
    if (mkdir_mount_devshm())
        return 1;

    qputenv("QT_QPA_FONTDIR", "/usr/share/fonts");
    qputenv("QT_QPA_PLATFORM", "eglfs");
    qputenv("QT_QPA_EGLFS_HIDECURSOR", "1");
#endif

    QCoreApplication::setOrganizationName(QLatin1String("Qt"));
    QCoreApplication::setApplicationName("openhbbtvbrowser");
    QCoreApplication::setApplicationVersion(QLatin1String("0.1"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QByteArrayList args = QByteArrayList()
#if defined(Q_OS_OSX)
            << QByteArrayLiteral("--remote-debugging-port=9000")
#endif
            << QByteArrayLiteral("--no-sandbox");
    const int count = args.size() + argc;
    QVector<char *> qargv(count);

    qargv[0] = argv[0];
    for (int i = 0; i < args.size(); ++i)
        qargv[i + 1] = args[i].data();
    for (int i = args.size() + 1; i < count; ++i)
        qargv[i] = argv[i - args.size()];

    int qAppArgCount = qargv.size();

    QApplication app(qAppArgCount, qargv.data());

    QLockFile lockFile(QDir::tempPath() + "/openhbbtvbrowser.lock");
    if(!lockFile.tryLock(100)) {
        qDebug() << "The application is already running.";
        return 1;
    }

    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(userAgent);

    QUrl url = commandLineUrlArgument();

    auto window = new BrowserWindow();
#if defined(Q_OS_LINUX)
    window->showFullScreen();
    window->setWindowFlags(Qt::FramelessWindowHint);
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setStyleSheet("background: transparent;");
#else
    window->resize(1280, 720);
#endif
    window->webView()->setUrl(url);
    window->show();

#if defined(Q_OS_LINUX)
    auto remote = new RemoteController();
    QObject::connect(remote, &RemoteController::activate, window->webView(), &WebView::sendKeyEvent);
    QObject::connect(remote, &RemoteController::volumeMute, window, &BrowserWindow::volumeMute);
    QObject::connect(remote, &RemoteController::volumeDown, window, &BrowserWindow::volumeDown);
    QObject::connect(remote, &RemoteController::volumeUp, window, &BrowserWindow::volumeUp);
#endif

    auto filter = new WindowEventFilter();
    app.installEventFilter(filter);
    QObject::connect(filter, &WindowEventFilter::activate, window->webView(), &WebView::sendKeyEvent);

    return app.exec();
}
