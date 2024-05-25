#include <QtCore/QDebug>
#include <QtGui/QIcon>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#if QT_VERSION_MAJOR >= 6
#include <qscreen.h>
#else
#include <QtWidgets/QDesktopWidget>
#endif
#include <ui/splash/splash_widget.h>

#define MARGIN 10
#define SHADOW 1

/**
 * @brief		Constructor.
 *
 * @param[in]	parent		Parent widget.
 */
SplashWidget::SplashWidget(QWidget *parent) :
    QWidget(parent), m_thread(nullptr), m_text(""),
    m_eventLoop(new QEventLoop(this))
{
    qRegisterMetaType<::std::function<void()>>("::std::function<void()>");

    this->setWindowIcon(QIcon(":/Icons/MainIcon.png"));

    /// Set flags
    this->setWindowFlags(Qt::WindowType::FramelessWindowHint
                         | Qt::WindowType::NoDropShadowWindowHint
                         | Qt::WindowType::CustomizeWindowHint);

    /// Set size.
    QImage          background(":/Images/splash.jpg");
#if QT_VERSION_MAJOR >= 6
    QScreen* desktop = QGuiApplication::primaryScreen();
    int      width = desktop->size().width() / 2;
    QSize    sz(width, background.height() * width / background.width());
    this->setFixedSize(sz);
    this->setGeometry((desktop->size().width() - sz.width()) / 2,
        (desktop->size().height() - sz.height()) / 2, sz.width(),
        sz.height());
#else
    QDesktopWidget *desktop = QApplication::desktop();
    int             width = desktop->width() / 2;
    QSize           sz(width, background.height() * width / background.width());
    this->setFixedSize(sz);
    this->setGeometry((desktop->width() - sz.width()) / 2,
        (desktop->height() - sz.height()) / 2, sz.width(),
        sz.height());
#endif

    m_background = background.scaled(sz);

    this->connect(this, &SplashWidget::sigCallFunc, this,
                  &SplashWidget::onCallFunc,
                  Qt::ConnectionType::QueuedConnection);

    this->activateWindow();
}

/**
 * @brief		Start work.
 *
 * @param[in]	workFunc	Work to do.
 *
 * @return		Return value of workFunc.
 */
int SplashWidget::exec(::std::function<int()> workFunc)
{
    /// Create thread.
    m_closeable = false;
    m_thread    = new SplashThread(workFunc, this);
    this->connect(m_thread, &SplashThread::finished, this,
                  &SplashWidget::onFinished,
                  Qt::ConnectionType::QueuedConnection);

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_thread->start();
    this->show();
    m_eventLoop->exec();
    m_thread->wait();

    int ret = m_thread->exitCode();
    delete m_thread;
    m_thread = nullptr;

    m_closeable = true;
    this->close();
    return ret;
}

/**
 * @brief		Slot to run function in eventloop.
 *
 * @param[in]	func	Function to call.
 */
void SplashWidget::onCallFunc(::std::function<void()> func)
{
    func();
}

/**
 * @brief	Destructor.
 */
SplashWidget::~SplashWidget() {}

/**
 * @brief		Paint event.
 *
 * @param[in]	event	Event.
 */
void SplashWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    /// Draw baclkground
    painter.drawImage(0, 0, m_background);

    /// Draw text
    QPen        shadowPen(QColor(0, 0, 0));
    QPen        forgroundPen(QColor(255, 255, 255));
    QReadLocker locker(&m_textLock);
    if (! m_text.empty()) {
        /// Compute position
        int lineHeight  = this->fontMetrics().height();
        int totalHeight = lineHeight * m_text.size();
        int y           = this->height() - MARGIN - totalHeight
                + this->fontMetrics().height() / 2;
        int x = MARGIN;

        /// Draw text
        for (auto &s : m_text) {
            /// Shadow
            painter.setPen(shadowPen);
            /// Top
            painter.drawText(x, y - SHADOW, s);

            /// Top-right
            painter.drawText(x + SHADOW, y - SHADOW, s);

            /// Right
            painter.drawText(x + SHADOW, y, s);

            /// Bottom-right
            painter.drawText(x + SHADOW, y + SHADOW, s);

            /// Bottom
            painter.drawText(x, y + SHADOW, s);

            /// Bottom-left
            painter.drawText(x - SHADOW, y + SHADOW, s);

            /// Left
            painter.drawText(x - SHADOW, y, s);

            /// Top-left
            painter.drawText(x - SHADOW, y - SHADOW, s);

            /// Text
            painter.setPen(forgroundPen);
            painter.drawText(x, y, s);

            y += lineHeight;
        }
    }
    event->accept();
}

/**
 * @brief		Close event.
 *
 * @param[in]	event	Event.
 */
void SplashWidget::closeEvent(QCloseEvent *event)
{
    if (m_closeable) {
        event->accept();
    } else {
        event->ignore();
    }
}

/**
 * @brief		Set text.
 *
 * @param[in]	text	Text to show.
 */
void SplashWidget::setText(QString text)
{
    /// Split lines.
    int         lineWidth = this->width() - MARGIN - MARGIN;
    QStringList lines;
    if (! (text.isNull() || text.isEmpty())) {
        QString line("");
        for (auto &c : text) {
            if (c == '\n') {
                lines.append(line);
                line = "";
            } else {
                line.append(c);
                if (this->fontMetrics().horizontalAdvance(line) > lineWidth) {
                    line.remove(line.size() - 1, 1);
                    lines.append(line);
                    line = c;
                }
            }
        }
        if (line != "") {
            lines.append(line);
        }
    }

    /// Set text
    QWriteLocker locker(&m_textLock);
    m_text = ::std::move(lines);

    this->update();
}

/**
 * @brief		Tell the widget the work has benn finished
 *
 */
void SplashWidget::onFinished()
{
    m_closeable = true;
    m_eventLoop->quit();
}
