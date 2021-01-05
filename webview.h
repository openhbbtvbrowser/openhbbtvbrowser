#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = Q_NULLPTR);
    void injectHbbTVScripts(const QString &src);
    void injectXmlHttpRequestScripts();
    void setCurrentChannel(const int &onid, const int &tsid, const int &sid);
    void setLanguage(const QString &language);

Q_SIGNALS:
    void broadcastPlay();
    void broadcastStop();

public Q_SLOTS:
    void sendKeyEvent(const int &keyCode);

protected Q_SLOTS:
    void windowCloseRequested();
    void titleChanged(const QString &title);
    void loadFinished(bool ok);
};

#endif // WEBVIEW_H
