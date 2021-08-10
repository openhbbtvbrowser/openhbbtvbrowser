#include "browsercontrol.h"
#include "browserwindow.h"
#include "virtualkey.h"
#include <QKeyEvent>

#if defined(EMBEDDED_BUILD)

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/dvb/version.h>
#include <linux/dvb/audio.h>

RemoteController::RemoteController(const QString &device)
    : m_muteToggle(false)
{
    m_fd = ::open(device.toLocal8Bit().constData(), O_RDONLY, 0);
    if (m_fd >= 0) {
        m_notify = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
        connect(m_notify, &QSocketNotifier::activated, this, &RemoteController::readKeycode);
    } else {
        qErrnoWarning("Cannot open keyboard input device '%ls'", qUtf16Printable(device));
    }
}

RemoteController::~RemoteController()
{
    if (m_fd >= 0)
        ::close(m_fd);
}

void RemoteController::readKeycode()
{
    struct ::input_event buffer[32];
    int n = 0;

    forever {
        int result = read(m_fd, reinterpret_cast<char *>(buffer) + n, sizeof(buffer) - n);

        if (result == 0) {
            qWarning("evdevkeyboard: Got EOF from the input device");
            return;
        } else if (result < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                qErrnoWarning("evdevkeyboard: Could not read from input device");
                if (errno == ENODEV) {
                    delete m_notify;
                    m_notify = nullptr;
                    ::close(m_fd);
                    m_fd = -1;
                }
                return;
            }
        } else {
            n += result;
            if (n % sizeof(buffer[0]) == 0)
                break;
        }
    }

    n /= sizeof(buffer[0]);

    for (int i = 0; i < n; ++i) {
        if (buffer[i].type == 1 && buffer[i].value != 0) {
            int vk = VirtualKey::VK_UNDEFINED;
            switch (buffer[i].code) {
            case KEY_RED:           vk = VirtualKey::VK_RED; break;
            case KEY_GREEN:         vk = VirtualKey::VK_GREEN; break;
            case KEY_YELLOW:        vk = VirtualKey::VK_YELLOW; break;
            case KEY_BLUE:          vk = VirtualKey::VK_BLUE; break;
            case KEY_LEFT:          vk = VirtualKey::VK_LEFT; break;
            case KEY_UP:            vk = VirtualKey::VK_UP; break;
            case KEY_RIGHT:         vk = VirtualKey::VK_RIGHT; break;
            case KEY_DOWN:          vk = VirtualKey::VK_DOWN; break;
            case KEY_OK:            vk = VirtualKey::VK_ENTER; break;
            case KEY_EXIT:          vk = VirtualKey::VK_BACK; break;
            case KEY_0:             vk = VirtualKey::VK_0; break;
            case KEY_1:             vk = VirtualKey::VK_1; break;
            case KEY_2:             vk = VirtualKey::VK_2; break;
            case KEY_3:             vk = VirtualKey::VK_3; break;
            case KEY_4:             vk = VirtualKey::VK_4; break;
            case KEY_5:             vk = VirtualKey::VK_5; break;
            case KEY_6:             vk = VirtualKey::VK_6; break;
            case KEY_7:             vk = VirtualKey::VK_7; break;
            case KEY_8:             vk = VirtualKey::VK_8; break;
            case KEY_9:             vk = VirtualKey::VK_9; break;
            case KEY_PAUSE:         vk = VirtualKey::VK_PAUSE; break;
            case KEY_PLAY:          vk = VirtualKey::VK_PLAY; break;
            case KEY_STOP:          vk = VirtualKey::VK_STOP; break;
            case KEY_FASTFORWARD:   vk = VirtualKey::VK_FAST_FWD; break;
            case KEY_REWIND:        vk = VirtualKey::VK_REWIND; break;
            case KEY_MUTE:          vk = VirtualKey::VK_MUTE; break;
            case KEY_VOLUMEDOWN:    vk = VirtualKey::VK_VOLUME_DOWN; break;
            case KEY_VOLUMEUP:      vk = VirtualKey::VK_VOLUME_UP; break;
            case KEY_INFO:          vk = VirtualKey::VK_INFO; break;
            case KEY_TEXT:          vk = VirtualKey::VK_TELETEXT; break;
            case KEY_SUBTITLE:      vk = VirtualKey::VK_SUBTITLE; break;
            case KEY_MENU:          [[fallthrough]];
            case KEY_POWER:         QApplication::quit(); break;
            }
            if (vk != VirtualKey::VK_UNDEFINED) emit activate(vk);
        }
    }
}

void RemoteController::volumeMute()
{
    int fd = open("/dev/dvb/adapter0/audio0", O_RDWR);
    if (fd >= 0) {
        m_muteToggle = !m_muteToggle;
        ioctl(fd, AUDIO_SET_MUTE, m_muteToggle);
        ::close(fd);
    }
}

void RemoteController::volumeDown()
{
    int fd = open("/dev/dvb/adapter0/audio0", O_RDWR);
    if (fd >= 0) {
        struct audio_status status;
        ioctl(fd, AUDIO_GET_STATUS, &status);
        int volume = status.mixer_state.volume_left;
        if (volume++ > 63)
            volume = 63;
        status.mixer_state.volume_left =
	    status.mixer_state.volume_right = volume;
        ioctl(fd, AUDIO_SET_MIXER, &status.mixer_state);
        m_muteToggle = false;
        ioctl(fd, AUDIO_SET_MUTE, m_muteToggle);
        ::close(fd);
    }
}

void RemoteController::volumeUp()
{
    int fd = open("/dev/dvb/adapter0/audio0", O_RDWR);
    if (fd >= 0) {
        struct audio_status status;
        ioctl(fd, AUDIO_GET_STATUS, &status);
        int volume = status.mixer_state.volume_left;
        if (--volume < 0)
            volume = 0;
        status.mixer_state.volume_left =
	    status.mixer_state.volume_right = volume;
        ioctl(fd, AUDIO_SET_MIXER, &status.mixer_state);
        m_muteToggle = false;
        ioctl(fd, AUDIO_SET_MUTE, m_muteToggle);
        ::close(fd);
    }
}

#endif

WindowEventFilter::WindowEventFilter(QObject *parent)
    : QObject(parent)
{
}

bool WindowEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        int vk = VirtualKey::VK_UNDEFINED;
        switch (keyEvent->key()) {
        case Qt::Key_R:             vk = VirtualKey::VK_RED; break;
        case Qt::Key_G:             vk = VirtualKey::VK_GREEN; break;
        case Qt::Key_Y:             vk = VirtualKey::VK_YELLOW; break;
        case Qt::Key_B:             vk = VirtualKey::VK_BLUE; break;
        case Qt::Key_Left:          vk = VirtualKey::VK_LEFT; break;
        case Qt::Key_Up:            vk = VirtualKey::VK_UP; break;
        case Qt::Key_Right:         vk = VirtualKey::VK_RIGHT; break;
        case Qt::Key_Down:          vk = VirtualKey::VK_DOWN; break;
        case Qt::Key_Return:        vk = VirtualKey::VK_ENTER; break;
        case Qt::Key_Backspace:     vk = VirtualKey::VK_BACK; break;
        case Qt::Key_0:             vk = VirtualKey::VK_0; break;
        case Qt::Key_1:             vk = VirtualKey::VK_1; break;
        case Qt::Key_2:             vk = VirtualKey::VK_2; break;
        case Qt::Key_3:             vk = VirtualKey::VK_3; break;
        case Qt::Key_4:             vk = VirtualKey::VK_4; break;
        case Qt::Key_5:             vk = VirtualKey::VK_5; break;
        case Qt::Key_6:             vk = VirtualKey::VK_6; break;
        case Qt::Key_7:             vk = VirtualKey::VK_7; break;
        case Qt::Key_8:             vk = VirtualKey::VK_8; break;
        case Qt::Key_9:             vk = VirtualKey::VK_9; break;
        case Qt::Key_K:             vk = VirtualKey::VK_PAUSE; break;
        case Qt::Key_J:             vk = VirtualKey::VK_PLAY; break;
        case Qt::Key_L:             vk = VirtualKey::VK_STOP; break;
        case Qt::Key_P:             vk = VirtualKey::VK_FAST_FWD; break;
        case Qt::Key_N:             vk = VirtualKey::VK_REWIND; break;
        case Qt::Key_Q:             QApplication::quit(); break;
        }
        if (vk != VirtualKey::VK_UNDEFINED) emit activate(vk);
        return true;
    }

    return QObject::eventFilter(obj, event);
}

CommandClient::CommandClient(const QString &sockFile)
    : m_socket(new QLocalSocket(this))
{
    connect(m_socket, &QLocalSocket::readyRead, this, &CommandClient::readCommand);

    m_socket->abort();
    m_socket->connectToServer(sockFile);
}

void CommandClient::readCommand()
{
    while (!m_socket->atEnd()) {
        if (m_socket->bytesAvailable() < 12)
            return;

        quint32 magic, command, dataSize;

        QDataStream inStream(m_socket->read(12));
        inStream >> magic
                 >> command
                 >> dataSize;

        if (magic != 987654321)
            continue;

        char buf[dataSize + 1];
        if (dataSize)
            m_socket->read(buf, dataSize);
        buf[dataSize] = 0;

        qDebug() << "command" << command;
    }
}

bool CommandClient::writeCommand(int command)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)0;
    return m_socket->waitForConnected(1000);
}

bool CommandClient::writeCommand(int command, const QString &data)
{
    if (!m_socket->isValid())
        return false;

    QDataStream outStream(m_socket);
    outStream << (qint32)987654321
              << (qint32)command
              << (qint32)data.size();
    m_socket->write(data.toStdString().c_str(), data.size());
    return m_socket->waitForConnected(1000);
}
