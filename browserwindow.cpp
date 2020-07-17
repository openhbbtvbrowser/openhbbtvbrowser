#include "browsercontrol.h"
#include "browserwindow.h"
#include "webview.h"
#include <QWebEngineProfile>

BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_commandClient(new CommandClient)
    , m_webView(new WebView(this))
{
    m_webView->show();

    setCentralWidget(m_webView);

    connect(m_webView, &WebView::broadcastPlay, this, &BrowserWindow::broadcastPlay);
    connect(m_webView, &WebView::broadcastStop, this, &BrowserWindow::broadcastStop);
}

WebView *BrowserWindow::webView()
{
    return m_webView;
}

void BrowserWindow::broadcastPlay()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastPlay);
}

void BrowserWindow::broadcastStop()
{
    m_commandClient->writeCommand(CommandClient::CommandBroadcastStop);
}
