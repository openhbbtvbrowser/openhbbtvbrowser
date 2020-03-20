#include "webview.h"
#include <QWebEngineScript>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::titleChanged, this, &WebView::titleChanged);
    connect(this, &QWebEngineView::loadFinished, this, &WebView::loadFinished);
}

void WebView::titleChanged(const QString &title)
{
     qDebug() << "titleChanged" << title;
}

void WebView::loadFinished(bool ok)
{
    if (ok) {
        if (size().width() == 1920 && size().height() == 1080) {
            QString s = QString::fromLatin1("document.body.style.setProperty('zoom', '150%');");
            page()->runJavaScript(s);
        }
    }
}

void WebView::sendKeyEvent(const int &keyCode)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  document.dispatchEvent(new KeyboardEvent('keydown',{keyCode:%1}));"
                                    "})()").arg(keyCode);
    page()->runJavaScript(s);
}
