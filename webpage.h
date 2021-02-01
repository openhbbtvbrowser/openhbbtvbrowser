#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    WebPage(QWebEngineProfile *profile, QObject *parent = Q_NULLPTR);

protected Q_SLOTS:
    void windowCloseRequested();

protected:
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level,
                                  const QString &message,
                                  int lineId,
                                  const QString &sourceId) override;
};

#endif // WEBPAGE_H
