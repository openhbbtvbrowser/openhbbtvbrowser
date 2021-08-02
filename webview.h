#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebEngineView>
#include <QLabel>

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    WebView(QWidget *parent = Q_NULLPTR);
    void injectHbbTVScripts(const QString &src);
    void injectXmlHttpRequestScripts();
    void setCurrentChannel(const int &onid, const int &tsid, const int &sid);
    void setLanguage(const QString &language);
    void setScriptDebugging(const QString &scriptDebugging);

Q_SIGNALS:
    void broadcastPlay();
    void broadcastStop();

public Q_SLOTS:
    void sendKeyEvent(const int &keyCode);

protected Q_SLOTS:
    void titleChanged(const QString &title);
    void loadFinished(bool ok);

private:
    QLabel *m_quitMsg;
    int m_quitMsgStatus;
};

#endif // WEBVIEW_H
