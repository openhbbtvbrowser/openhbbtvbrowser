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

protected Q_SLOTS:
    void broadcastPlay();
    void broadcastStop();

private:
    CommandClient *m_commandClient;
    WebView *m_webView;
};

#endif // BROWSERWINDOW_H
