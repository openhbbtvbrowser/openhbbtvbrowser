#include "webview.h"
#include "virtualkey.h"
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QUrl>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QWebEngineHistory>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent), m_quitMsg(new QLabel), m_quitMsgStatus(0)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    m_quitMsg->setText("Press Power Key to Quit");
    m_quitMsg->setGeometry(0, 0, 480, 120);
    m_quitMsg->setAlignment(Qt::AlignCenter);
    m_quitMsg->setStyleSheet("background: black; color: white; font: 24pt;");
    int x = (screenGeometry.width() - m_quitMsg->width()) / 2;
    int y = (screenGeometry.height() - m_quitMsg->height()) / 2;
    m_quitMsg->setGeometry(x, y, 480, 120);
    m_quitMsg->hide();

    connect(this, &QWebEngineView::titleChanged, this, &WebView::titleChanged);
    connect(this, &QWebEngineView::loadFinished, this, &WebView::loadFinished);
}

void WebView::injectHbbTVScripts(const QString &src)
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  var element = document.createElement('script');"
                                    "  element.setAttribute('type', 'text/javascript');"
                                    "  element.setAttribute('src', '%1');"
                                    "  document.head.appendChild(element);"
                                    "})();").arg(src);

    script.setName("hbbtv_polyfill");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::injectHbbTVQuirks()
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  var element = document.createElement('script');"
                                    "  element.setAttribute('type', 'text/javascript');"
                                    "  element.setAttribute('src', 'qrc:/hbbtv_quirks.js');"
                                    "  document.head.appendChild(element);"
                                    "})();");

    script.setName("hbbtv_quirks");
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
                                    "    onid : %1,"
                                    "    tsid : %2,"
                                    "    sid  : %3,"
                                    "  };"
                                    "})();").arg(onid).arg(tsid).arg(sid);

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
                                    "})();").arg(language);

    script.setName("preferred_language");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::setScriptDebugging(const QString &scriptDebugging)
{
    QWebEngineScript script;

    QString s = QString::fromLatin1("(function() {"
                                    "  window.HBBTV_POLYFILL_DEBUG = %1;"
                                    "})();").arg(scriptDebugging);

    script.setName("hbbtv_polyfill_debug");
    script.setSourceCode(s);
    script.setInjectionPoint(QWebEngineScript::DocumentReady);
    script.setRunsOnSubFrames(true);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void WebView::sendKeyEvent(const int &keyCode)
{
    if (keyCode == VirtualKey::VK_BACK) {
        if (!page()->history()->canGoBack()) {
            if (!m_quitMsgStatus) {
                m_quitMsg->show();
                m_quitMsgStatus = 1;
            } else {
                m_quitMsg->hide();
                m_quitMsgStatus = 0;
            }
        }
    } else {
        if (m_quitMsgStatus) {
            m_quitMsg->hide();
            m_quitMsgStatus = 0;
        }
    }

    QMetaEnum metaEnum = QMetaEnum::fromType<VirtualKey::VirtualKeyType>();

    QString s = QString::fromLatin1("(function() {"
                                    "  var keyEvent = new KeyboardEvent('keydown', {"
                                    "    bubbles : true,"
                                    "    cancelable : true,"
                                    "    keyCode : %1"
                                    "  });"
                                    "  if (window['%2'] !== 'undefined') {"
                                    "    delete keyEvent.keyCode;"
                                    "    Object.defineProperty(keyEvent, 'keyCode', { value: window['%2'] });"
                                    "  }"
                                    "  document.activeElement.dispatchEvent(keyEvent);"
                                    "})();").arg(keyCode).arg(metaEnum.valueToKey(keyCode));
    page()->runJavaScript(s);
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
