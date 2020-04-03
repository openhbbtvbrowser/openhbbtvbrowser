#include "browsercontrol.h"
#include "browserwindow.h"
#include "webpage.h"
#include "webview.h"
#include <QWebEngineProfile>

BrowserWindow::BrowserWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
    , m_commandClient(new CommandClient)
    , m_webView(new WebView(this))
{
    WebPage *page = new WebPage(QWebEngineProfile::defaultProfile(), m_webView);
    m_webView->setPage(page);
    m_webView->show();

    setCentralWidget(m_webView);

    connect(m_webView, &WebView::broadcastPlay, this, &BrowserWindow::broadcastPlay);
    connect(m_webView, &WebView::broadcastStop, this, &BrowserWindow::broadcastStop);

    connect(m_commandClient, &CommandClient::setUrl, this, &BrowserWindow::setUrl);
    connect(m_commandClient, &CommandClient::setSIData, this, &BrowserWindow::setSIData);
    connect(m_commandClient, &CommandClient::setAITData, this, &BrowserWindow::setAITData);
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

void BrowserWindow::volumeMute()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeMute);
}

void BrowserWindow::volumeDown()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeDown);
}

void BrowserWindow::volumeUp()
{
    m_commandClient->writeCommand(CommandClient::CommandVolumeUp);
}

void BrowserWindow::setUrl(const QString &url)
{
    m_webView->setUrl(url);
}

void BrowserWindow::setSIData(const quint32 &pmt,
                              const quint32 &tsid,
                              const quint32 &onid,
                              const quint32 &ssid,
                              const quint32 &chantype,
                              const quint32 &chanid)
{
    Q_UNUSED(pmt);
    Q_UNUSED(tsid);
    Q_UNUSED(onid);
    Q_UNUSED(ssid);
    Q_UNUSED(chantype);
    Q_UNUSED(chanid);
}

void BrowserWindow::setAITData()
{
}
