#ifndef BROWSERCONTROL_H
#define BROWSERCONTROL_H

#include <QApplication>
#include <QLocalSocket>
#include <QObject>
#include <QSocketNotifier>

class BrowserWindow;

#if defined(EMBEDDED_BUILD)

struct input_event {
    long pad1;
    long pad2;
    quint16 type;
    quint16 code;
    qint32 value;
};
#define KEY_1			2
#define KEY_2			3
#define KEY_3			4
#define KEY_4			5
#define KEY_5			6
#define KEY_6			7
#define KEY_7			8
#define KEY_8			9
#define KEY_9			10
#define KEY_0			11
#define KEY_UP			103
#define KEY_LEFT		105
#define KEY_RIGHT		106
#define KEY_DOWN		108
#define KEY_MUTE		113
#define KEY_VOLUMEDOWN	114
#define KEY_VOLUMEUP	115
#define KEY_POWER		116
#define KEY_PAUSE		119
#define KEY_STOP		128
#define KEY_MENU		139
#define KEY_REWIND		168
#define KEY_EXIT		174
#define KEY_PLAY		207
#define KEY_FASTFORWARD	208
#define KEY_OK			0x160
#define KEY_INFO		0x166
#define KEY_SUBTITLE	0x172
#define KEY_TEXT		0x184
#define KEY_RED			0x18e
#define KEY_GREEN		0x18f
#define KEY_YELLOW		0x190
#define KEY_BLUE		0x191

class RemoteController : public QObject
{
    Q_OBJECT

public:
    RemoteController(const QString &device = QString("/dev/input/event0"));
    ~RemoteController();

Q_SIGNALS:
    void activate(const int &keyCode);

protected Q_SLOTS:
    void readKeycode();

protected:
    void volumeMute();
    void volumeDown();
    void volumeUp();

private:
    int m_fd;
    QSocketNotifier *m_notify;
    bool m_muteToggle;
};

#endif

class WindowEventFilter : public QObject
{
    Q_OBJECT

public:
    WindowEventFilter(QObject *parent = Q_NULLPTR);

Q_SIGNALS:
    void activate(const int &keyCode);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
};

class CommandClient : public QObject
{
    Q_OBJECT

public:
    enum BrowserCommand {
        CommandBroadcastPlay = 1,
        CommandBroadcastStop = 2,
    };

    CommandClient(const QString &sockFile = QString("/tmp/openhbbtvbrowser.socket"));
    bool writeCommand(int command);
    bool writeCommand(int command, const QString &data);

protected Q_SLOTS:
    void readCommand();

private:
    QLocalSocket *m_socket;
};

#endif // BROWSERCONTROL_H
