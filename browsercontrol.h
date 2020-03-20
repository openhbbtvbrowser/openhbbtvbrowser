#ifndef BROWSERCONTROL_H
#define BROWSERCONTROL_H

#include <QApplication>
#include <QObject>
#include <QSocketNotifier>

class BrowserWindow;

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
#define KEY_PAUSE		119
#define KEY_STOP		128
#define KEY_MENU		139
#define KEY_REWIND		168
#define KEY_EXIT		174
#define KEY_PLAY		207
#define KEY_FASTFORWARD	208
#define KEY_OK			0x160
#define KEY_RED			0x18e
#define KEY_GREEN		0x18f
#define KEY_YELLOW		0x190
#define KEY_BLUE		0x191

enum VirtualKey {
    // Based on CEA-2014-A CE-HTML Annex F
    VK_UNDEFINED = 0,
    VK_ENTER = 13,
    VK_LEFT = 37,
    VK_UP = 38,
    VK_RIGHT = 39,
    VK_DOWN = 40,
    VK_SPACE = 32,
    VK_BACK_SPACE = 8,
    VK_0 = 48,
    VK_1 = 49,
    VK_2 = 50,
    VK_3 = 51,
    VK_4 = 52,
    VK_5 = 63,
    VK_6 = 54,
    VK_7 = 55,
    VK_8 = 56,
    VK_9 = 57,
    VK_A = 65,
    VK_B = 66,
    VK_C = 67,
    VK_D = 68,
    VK_E = 69,
    VK_F = 70,
    VK_G = 71,
    VK_H = 72,
    VK_I = 73,
    VK_J = 74,
    VK_K = 75,
    VK_L = 76,
    VK_M = 77,
    VK_N = 78,
    VK_O = 79,
    VK_P = 80,
    VK_Q = 81,
    VK_R = 82,
    VK_S = 83,
    VK_T = 84,
    VK_U = 85,
    VK_V = 86,
    VK_W = 87,
    VK_X = 88,
    VK_Y = 89,
    VK_Z = 90,
    VK_RED = 403,
    VK_GREEN = 404,
    VK_YELLOW = 405,
    VK_BLUE = 406,
    VK_HELP = 156,
    VK_SEARCH = 112,
    VK_AUDIODESCRIPTION = 113,
    VK_HD = 114,
    VK_PLAY = 415,
    VK_PAUSE = 19,
    VK_PLAY_PAUSE = 402,
    VK_STOP = 413,
    VK_PREV = 424,
    VK_NEXT = 425,
    VK_FAST_FWD = 417,
    VK_REWIND = 412,
    VK_INFO = 457,
    VK_SUBTITLE = 460,
    VK_BACK = 461,
    VK_VOLUME_UP = 447,
    VK_VOLUME_DOWN = 448,
    VK_MUTE = 449,
};

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

private:
    int m_fd;
    QSocketNotifier *m_notify;
};

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

#endif // BROWSERCONTROL_H
