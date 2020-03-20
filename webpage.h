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
};

#endif // WEBPAGE_H
