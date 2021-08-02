#include "browsercontrol.h"
#include "browserwindow.h"
#include "webview.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QLockFile>
#include <QUrl>
#include <QWebEngineSettings>
#include <QWebEngineProfile>

#if defined(EMBEDDED_BUILD)
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
#if defined(EMBEDDED_BUILD)
    if (mkdir_mount_devshm())
        return 1;

    if (qgetenv("QT_QPA_FONTDIR").isNull())
        qputenv("QT_QPA_FONTDIR", "/usr/share/fonts");
    if (qgetenv("QT_QPA_PLATFORM").isNull())
        qputenv("QT_QPA_PLATFORM", "eglfs");
    if (qgetenv("QT_QPA_EGLFS_HIDECURSOR").isNull())
        qputenv("QT_QPA_EGLFS_HIDECURSOR", "1");
#endif

    QCoreApplication::setOrganizationName(QLatin1String("Qt"));
    QCoreApplication::setApplicationName("openhbbtvbrowser");
    QCoreApplication::setApplicationVersion(QLatin1String("0.1"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QByteArrayList args = QByteArrayList()
            << QByteArrayLiteral("--disable-web-security")
            << QByteArrayLiteral("--no-sandbox")
            << QByteArrayLiteral("--log-level=0");
    const int count = args.size() + argc;
    QVector<char *> qargv(count);

    qargv[0] = argv[0];
    for (int i = 0; i < args.size(); ++i)
        qargv[i + 1] = args[i].data();
    for (int i = args.size() + 1; i < count; ++i)
        qargv[i] = argv[i - args.size()];

    int qAppArgCount = qargv.size();

    QApplication app(qAppArgCount, qargv.data());

#if defined(EMBEDDED_BUILD)
    QLockFile lockFile(QDir::tempPath() + "/openhbbtvbrowser.lock");
    if(!lockFile.tryLock(100)) {
        qDebug() << "The application is already running.";
        return 1;
    }
#endif

    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::SpatialNavigationEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::ShowScrollBars, false);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
    QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->setHttpUserAgent(
        QWebEngineProfile::defaultProfile()->httpUserAgent() + QStringLiteral(" HbbTV/1.4.1 SmartTV2015"));

    QString src = QStringLiteral("qrc:/hbbtv_polyfill.js");
    int onid = -1;
    int tsid = -1;
    int sid = -1;

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("src", "Script Src", "src"));
    parser.addOption(QCommandLineOption("onid", "Original Network ID", "onid"));
    parser.addOption(QCommandLineOption("tsid", "Transport Stream ID", "tsid"));
    parser.addOption(QCommandLineOption("sid", "Service ID", "sid"));
    parser.addOption(QCommandLineOption("enable-script-debugging", "EnableScript Debugging", "enable-script-debugging"));
    parser.parse(QCoreApplication::arguments());
    if (parser.isSet("src"))
        src = parser.value("src");
    if (parser.isSet("onid"))
        onid = parser.value("onid").toInt();
    if (parser.isSet("tsid"))
        tsid = parser.value("tsid").toInt();
    if (parser.isSet("sid"))
        sid = parser.value("sid").toInt();
    bool scriptDebugging = parser.isSet("enable-script-debugging");

    QUrl url = commandLineUrlArgument();

    auto window = new BrowserWindow();
#if defined(EMBEDDED_BUILD)
    window->showFullScreen();
    window->setWindowFlags(Qt::FramelessWindowHint);
    window->setAttribute(Qt::WA_TranslucentBackground);
    window->setStyleSheet("background: transparent;");
#else
    window->resize(1280, 720);
#endif
    window->webView()->injectHbbTVScripts(src);
    window->webView()->injectXmlHttpRequestScripts();
    if (onid != -1 && tsid != -1 && sid != -1)
        window->webView()->setCurrentChannel(onid, tsid, sid);
    window->webView()->setLanguage(QStringLiteral("DEU")); // TODO:
    window->webView()->setScriptDebugging(scriptDebugging ? QStringLiteral("true") : QStringLiteral("false"));
    window->webView()->setUrl(url);
    window->show();

#if defined(EMBEDDED_BUILD)
    auto remote = new RemoteController();
    QObject::connect(remote, &RemoteController::activate, window->webView(), &WebView::sendKeyEvent);
#else
    auto filter = new WindowEventFilter();
    app.installEventFilter(filter);
    QObject::connect(filter, &WindowEventFilter::activate, window->webView(), &WebView::sendKeyEvent);
#endif

    return app.exec();
}
