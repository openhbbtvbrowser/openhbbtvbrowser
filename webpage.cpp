#include "webpage.h"
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
    setBackgroundColor(Qt::transparent);

    connect(this, &QWebEnginePage::windowCloseRequested, this, &WebPage::windowCloseRequested);
}

void WebPage::windowCloseRequested()
{
    qDebug() << "windowCloseRequested";
}

void WebPage::javaScriptConsoleMessage(WebPage::JavaScriptConsoleMessageLevel level,
                                       const QString &message,
                                       int lineId,
                                       const QString &sourceId)
{
    QWebEnginePage::javaScriptConsoleMessage(level, message, lineId, sourceId);
}
