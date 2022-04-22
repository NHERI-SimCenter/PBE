// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include <MainWindowWorkflowApp.h>
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>

#include <AgaveCurl.h>
#include <WorkflowAppPBE.h>

#include <QTime>
#include <QTextStream>
#include <GoogleAnalytics.h>
#include <QDir>
#include <QStandardPaths>

 // customMessgaeOutput code from web:
 // https://stackoverflow.com/questions/4954140/how-to-redirect-qdebug-qwarning-qcritical-etc-output

static QString logFilePath;
static bool logToFile = false;

void customMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QHash<QtMsgType, QString> msgLevelHash({{QtDebugMsg, "Debug"}, {QtInfoMsg, "Info"}, {QtWarningMsg, "Warning"}, {QtCriticalMsg, "Critical"}, {QtFatalMsg, "Fatal"}});
    QByteArray localMsg = msg.toLocal8Bit();
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss.zzz");
    QByteArray formattedTimeMsg = formattedTime.toLocal8Bit();
    QString logLevelName = msgLevelHash[type];
    QByteArray logLevelMsg = logLevelName.toLocal8Bit();

    if (logToFile) {
        QString txt = QString("%1 %2: %3 (%4)").arg(formattedTime, logLevelName, msg,  context.file);
        QFile outFile(logFilePath);
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
        outFile.close();
    } else {
        fprintf(stderr, "%s %s: %s (%s:%u, %s)\n", formattedTimeMsg.constData(), logLevelMsg.constData(), localMsg.constData(), context.file, context.line, context.function);
        fflush(stderr);
    }

    if (type == QtFatalMsg)
        abort();
}


int main(int argc, char *argv[])
{
    //Setting Core Application Name, Organization and Version
    QCoreApplication::setApplicationName("PBE");
    QCoreApplication::setOrganizationName("SimCenter");
    QCoreApplication::setApplicationVersion("3.0.0");
    //    GoogleAnalytics::SetTrackingId("UA-126256136-1");
    GoogleAnalytics::StartSession();
    GoogleAnalytics::ReportStart();

#ifdef Q_OS_WIN
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
#else
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif

  //
  // set up logging of output messages for user debugging
  //

    logFilePath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
      + QDir::separator() + QCoreApplication::applicationName();

    // make sure tool dir exists in Documentss folder
    QDir dirWork(logFilePath);
    if (!dirWork.exists())
      if (!dirWork.mkpath(logFilePath)) {
	qDebug() << QString("Could not create Working Dir: ") << logFilePath;
      }

    // full path to debug.log file
    logFilePath = logFilePath + QDir::separator() + QString("debug.log");

    // remove old log file
    QFile debugFile(logFilePath);
    debugFile.remove();

    QByteArray envVar = qgetenv("QTDIR");       //  check if the app is run in Qt Creator

    if (envVar.isEmpty())
        logToFile = true;

    qInstallMessageHandler(customMessageOutput);


  // window scaling for mac
   QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);

  //
  // create a remote interface
  //

  QString tenant("designsafe");
  QString storage("agave://designsafe.storage.default/");
  QString dirName("PBE");

  AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);


  //
  // create the main window
  //
  WorkflowAppWidget *theInputApp = new WorkflowAppPBE(theRemoteService);
  MainWindowWorkflowApp w(QString("PBE - Performance Based Engineering Application"),theInputApp, theRemoteService);


  

// connect(theInputApp,SIGNAL(sendErrorMessage(QString)), *w, SLOT(errorM))


  QString aboutTitle = "About the SimCenter PBE Application"; // this is the title displayed in the on About dialog
  QString aboutSource = ":/resources/docs/textAboutPBE.html";  // this is an HTML file stored under resources
  w.setAbout(aboutTitle, aboutSource);

  QString version("Version 3.0.0");
  w.setVersion(version);

  QString citeText("1)Adam Zsarnoczay, Frank McKenna, Charles Wang, Wael Elhaddad, & Michael Gardner. (2019, October 15). NHERI-SimCenter/PBE: Release v2.0.0 (Version v2.0.00). Zenodo. http://doi.org/10.5281/zenodo.3491145 \n\n2) Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matt J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706");
  w.setCite(citeText);

  QString manualURL("https://nheri-simcenter.github.io/PBE-Documentation/");
  w.setDocumentationURL(manualURL);

  QString messageBoardURL("https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=7.0");
  w.setFeedbackURL(messageBoardURL);

  //
  // move remote interface to a thread
  //

  QThread *thread = new QThread();
  theRemoteService->moveToThread(thread);

  QWidget::connect(thread, SIGNAL(finished()), theRemoteService, SLOT(deleteLater()));
  QWidget::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

  thread->start();

  //
  // show the main window & start the event loop
  //

  w.show();

  /*
  QFile file(":/styleCommon/common_experimental.qss");
  if(file.open(QFile::ReadOnly)) {
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
  }
*/
#ifdef Q_OS_WIN
    QFile file(":/styleCommon/stylesheetWIN.qss");
#endif

#ifdef Q_OS_MACOS
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif

#ifdef Q_OS_LINUX
    QFile file(":/styleCommon/stylesheetMAC.qss");
#endif


    if(file.open(QFile::ReadOnly)) {
        a.setStyleSheet(file.readAll());
        file.close();
    } else {
        qDebug() << "could not open stylesheet";
    }



  int res = a.exec();

  //
  // on done with event loop, logout & stop the thread
  //

  theRemoteService->logout();
  thread->quit();

  GoogleAnalytics::EndSession();
  // done
  return res;
}
