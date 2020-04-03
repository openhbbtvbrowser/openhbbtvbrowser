#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>

class CommandClient;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    WebView *webView();

public Q_SLOTS:
    void volumeMute();
    void volumeDown();
    void volumeUp();

protected Q_SLOTS:
    void broadcastPlay();
    void broadcastStop();

protected Q_SLOTS:
    void setUrl(const QString &url);
    void setSIData(const quint32 &pmt,
                   const quint32 &tsid,
                   const quint32 &onid,
                   const quint32 &ssid,
                   const quint32 &chantype,
                   const quint32 &chanid);
    void setAITData();

private:
    CommandClient *m_commandClient;
    WebView *m_webView;
};

#endif // BROWSERWINDOW_H
