#include <ui/splash/splash_thread.h>

/**
 * @brief		Constructor.
 *
 * @param[in]	workFunc		Work.
 * @param[in]	parent			Parent.
 */
SplashThread::SplashThread(::std::function<int()> workFunc,
                           SplashWidget *         parent) :
    QThread(reinterpret_cast<QObject*>(parent)),
    m_workFunc(std::move(workFunc)), m_exitCode(-1)
{}

/**
 * @brief		Get exit code.
 *
 * @return		Exit code.
 */
int SplashThread::exitCode() const
{
    return m_exitCode;
}

/**
 * @brief		Destructor.
 */
SplashThread::~SplashThread() = default;

/**
 * @brief		Thread function..
 */
void SplashThread::run()
{
    m_exitCode = m_workFunc();
    emit this->finished();
}
