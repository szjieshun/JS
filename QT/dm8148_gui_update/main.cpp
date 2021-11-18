#include <QtGui/QApplication>
#include <QTextStream>
#include <QWSServer>
#include <QMutex>
#include <QTime>
#include <QDate>
#include <QFile>
#include <signal.h>
#include <QFileInfo>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "mainwindow.h"

#define DEBUG_INFO_FILE_MAX 1024*1024*5
#define DEBUG_INFO_FILE "/opt/data/cpr/qt_log.txt"

#if 0
#define PIDFILE "/var/run/dm8148_gui.pid"

#define FILE_MODE (S_IRWXU|S_IRWXG|S_IRWXO)

void handle_pipe(int sig)
{
    /* do nothing */
    static int sig_count = 0;
    sig_count += 1;
    if(sig_count >= 20000)
    {
        qDebug() << "@@@@@@@@@@@@@ handle sig = " << sig;
        sig_count = 0;
    }
}

void set_sigpipe_cb()
{
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = handle_pipe;
    sigemptyset(&action.sa_mask);
    sigaction(SIGPIPE, &action, NULL);
    sigaction(SIGSEGV, &action, NULL);
    //sigaction(SIGTERM, &action, NULL);
    sigaction(SIGBUS, &action, NULL);
    sigaction(SIGABRT, &action, NULL);
    sigaction(SIGSYS, &action, NULL);
    sigaction(SIGIO, &action, NULL);
}

int test_and_set_wrlock(int fd, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    int ret = fcntl(fd, F_SETLK, &lock);
    return ret;
}

bool isSingleton()
{
    int fd,val;
    char buf[10];

    if ((fd = open(PIDFILE, O_WRONLY|O_CREAT, FILE_MODE)) < 0) {
        qDebug() << "daemon need run by root. open" << PIDFILE << "error.";
        return false;
    }

    if(test_and_set_wrlock(fd, 0, SEEK_SET, 0) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
            qDebug() << "Qt has been in running!";
        else
            qDebug() << "Qt running error: " << strerror(errno);
        close(fd);
        return false;
    }

    if (ftruncate(fd, 0) < 0) {
        qDebug() << "ftruncate error.";
        close(fd);
        return false;
    }

    sprintf(buf,"%d\n",getpid());
    if (write(fd, buf, strlen(buf)) != strlen(buf)) {
        qDebug() << "pid write error.";
        close(fd);
        return false;
    }

    // close file descriptor
    if ((val = fcntl(fd, F_GETFD, 0)) < 0) {
        qDebug() << "fcntl F_GETFD error.";
        close(fd);
        return false;
    }
    val |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, val) < 0) {
        qDebug() << "fcntl F_SETFD error.";
        close(fd);
        return false;
    }
    close(fd);
    return true;
}

void initDaemon(void)
{
    pid_t pid, sid;
    if((pid = fork()) < 0)
        return;
    else if(pid != 0)
        exit(0); /* parent exit */
    /* child continues */
    sid = setsid(); /* become session leader */
    if (sid < 0) {
        exit(-1);
    }
    /* change working directory */
    if ((chdir("/")) < 0) {
        exit(-1);
    }
    umask(0); /* clear file mode creation mask */
#if 0
    close(0); /* close stdin */
    close(1); /* close stdout */
    close(2); /* close stderr */
#endif
}
#endif
void customMessageHandler(QtMsgType type, const char *msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    QString curDate = QDate::currentDate().toString("[yyyy-MM-dd ");
    QString curTime = QTime::currentTime().toString("hh:mm:ss] ");

    switch(type)
    {
        case QtDebugMsg:
        {
            text = QString("Debug: ");
            break;
        }

        case QtWarningMsg:
        {
            text = QString("Warning: ");
            break;
        }

        case QtCriticalMsg:
        {
            text = QString("Critical: ");
            break;
        }

        case QtFatalMsg:
        {
            text = QString("Fatal: ");
            break;
        }

        default:
            break;
    }

    QFileInfo fi(DEBUG_INFO_FILE);
    if(fi.size() >= DEBUG_INFO_FILE_MAX)
    {
        ::rename(DEBUG_INFO_FILE, DEBUG_INFO_FILE".bak");
    }

    QFile file(DEBUG_INFO_FILE);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qFatal("open log file failed!");
        mutex.unlock();
        return;
    }

    QTextStream txt_stream(&file);
    txt_stream << curDate << curTime << text << msg << endl;
    file.flush();
    file.close();

    mutex.unlock();
}
int main(int argc, char *argv[])
{
#ifdef Q_WS_QWS
    qInstallMsgHandler(customMessageHandler);
    //qInstallMsgHandler(0);
    qDebug() << "Version: v2.0; Build time: " << __DATE__ << __TIME__;
#endif
    QApplication a(argc, argv);
#ifdef Q_WS_QWS
    QWSServer::setCursorVisible(false);
#endif
    MainWindow w;
    w.show();
    return a.exec();
}

