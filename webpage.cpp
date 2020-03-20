#include "webpage.h"
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

WebPage::WebPage(QWebEngineProfile *profile, QObject *parent)
    : QWebEnginePage(profile, parent)
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  var element = document.createElement('script');"
                                    "  element.setAttribute('type','text/javascript');"
                                    "  element.setAttribute('src','qrc:///js/hbbtv_polyfill.js');"
                                    "  document.head.appendChild(element);"
                                    "})()");

    script.setName("hbbtv_polyfill");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    scripts().insert(script);

    setBackgroundColor(Qt::transparent);

    connect(this, &QWebEnginePage::windowCloseRequested, this, &WebPage::windowCloseRequested);
}

void WebPage::windowCloseRequested()
{
    qDebug() << "windowCloseRequested";
}
