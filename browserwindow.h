#ifndef BROWSERWINDOW_H
#define BROWSERWINDOW_H

#include <QMainWindow>

class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    BrowserWindow(QWidget *parent = Q_NULLPTR, Qt::WindowFlags flags = Qt::Widget);
    WebView *webView();

private:
    WebView *m_webView;
};

#endif // BROWSERWINDOW_H
