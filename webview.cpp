#include "webview.h"
#include <QUrl>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    page()->setBackgroundColor(Qt::transparent);

    connect(page(), &QWebEnginePage::windowCloseRequested, this, &WebView::windowCloseRequested);
    connect(this, &QWebEngineView::titleChanged, this, &WebView::titleChanged);
    connect(this, &QWebEngineView::loadFinished, this, &WebView::loadFinished);
}

void WebView::injectHbbTVScripts()
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  var element = document.createElement('script');"
                                    "  element.setAttribute('type','text/javascript');"
                                    "  element.setAttribute('src','qrc:/hbbtv_polyfill.js');"
                                    "  document.head.appendChild(element);"
                                    "})()");

    script.setName("hbbtv_polyfill");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::injectXmlHttpRequestScripts()
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  var element = document.createElement('script');"
                                    "  element.setAttribute('type','text/javascript');"
                                    "  element.setAttribute('src','qrc:/xmlhttprequest_quirks.js');"
                                    "  document.head.appendChild(element);"
                                    "})()");

    script.setName("xmlhttprequest_quirks");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::setCurrentChannel(const int &onid, const int &tsid, const int &sid)
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  window.HBBTV_POLYFILL_NS = window.HBBTV_POLYFILL_NS || {};"
                                    "  window.HBBTV_POLYFILL_NS.currentChannel = {"
                                    "    'onid':%1,"
                                    "    'tsid':%2,"
                                    "    'sid':%3,"
                                    "  };"
                                    "})()").arg(onid).arg(tsid).arg(sid);

    script.setName("current_channel");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::setLanguage(const QString &language)
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  window.HBBTV_POLYFILL_NS = window.HBBTV_POLYFILL_NS || {};"
                                    "  window.HBBTV_POLYFILL_NS.preferredLanguage = '%1';"
                                    "})()").arg(language);

    script.setName("current_channel");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::sendKeyEvent(const int &keyCode)
{
    QString s = QString::fromLatin1("(function() {"
                                    "  document.dispatchEvent(new KeyboardEvent('keydown',{keyCode:%1}));"
                                    "})()").arg(keyCode);
    page()->runJavaScript(s);
}

void WebView::windowCloseRequested()
{
    qDebug() << "windowCloseRequested";
}

void WebView::titleChanged(const QString &title)
{
    if (title.startsWith("OipfVideoBroadcastEmbeddedObject")) {
        emit broadcastPlay();
    } else if (title.startsWith("OipfAVControlObject")) {
        emit broadcastStop();

        QString url = title.mid(20);
        qDebug() << url;
    } else if (title.startsWith("createApplication")) {
        QString url = title.mid(18);
        qDebug() << url;
        setUrl(QUrl(url));
    }
}

void WebView::loadFinished(bool ok)
{
    if (ok) {
        if (size().width() == 1920 && size().height() == 1080)
            page()->runJavaScript(QString::fromLatin1("document.body.style.setProperty('zoom', '150%');"));

        page()->runJavaScript(QString::fromLatin1("document.body.style.setProperty('overflow', 'hidden');"));

        page()->runJavaScript(QString::fromLatin1("if (document.getElementById('appmgr')) document.getElementById('appmgr').style.setProperty('visibility', 'hidden');"));
        page()->runJavaScript(QString::fromLatin1("if (document.getElementById('oipfcfg')) document.getElementById('oipfcfg').style.setProperty('visibility', 'hidden');"));
        page()->runJavaScript(QString::fromLatin1("if (document.getElementById('oipfCap')) document.getElementById('oipfCap').style.setProperty('visibility', 'hidden');"));
        page()->runJavaScript(QString::fromLatin1("if (document.getElementById('oipfDrm')) document.getElementById('oipfDrm').style.setProperty('visibility', 'hidden');"));
    }
}
