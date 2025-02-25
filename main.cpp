// Written: fmckenna

// Purpose: the typical Qt main for running a QMainWindow

#include <MainWindowWorkflowApp.h>
#include <QApplication>
#include <QFile>
#include <QThread>
#include <QObject>
#include <QTime>
#include <QTextStream>
#include <QDir>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QWebEngineView>

#include <WorkflowAppPBE.h>
#include <TapisV3.h>
#include <GoogleAnalytics.h>


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
        ts << txt << Qt::endl;
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
    QCoreApplication::setApplicationVersion("4.2.2");

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

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();    
    QString workDir = env.value("SIMCENTER_WORKDIR","None");
    if (workDir != "None") {
      logFilePath = workDir;
    }    
    
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
  //QString storage("agave://designsafe.storage.default/");
  QString storage("designsafe.storage.default/");  
  QString dirName("PBE");

  //AgaveCurl *theRemoteService = new AgaveCurl(tenant, storage, &dirName);
  TapisV3 *theRemoteService = new TapisV3(tenant, storage, &dirName);  

  //
  // create the main window
  //
  
  //WorkflowAppWidget *theInputApp = new WorkflowAppPBE(theRemoteService);
  WorkflowAppWidget *theInputApp = WorkflowAppPBE::getInstance(theRemoteService);
  MainWindowWorkflowApp mainWindow(
    QString("PBE - Performance Based Engineering Application"),
    theInputApp, theRemoteService);


  QString aboutTitle = "About the SimCenter PBE Application"; // this is the title displayed in the on About dialog
  QString aboutSource = ":/resources/docs/textAboutPBE.html";  // this is an HTML file stored under resources
  mainWindow.setAbout(aboutTitle, aboutSource);

  QString version = QString("Version ") + QCoreApplication::applicationVersion();

  mainWindow.setVersion(version);

  QString citeText("Adam Zsarnoczay, Frank McKenna, Sang-ri Yi, Aakash Bangalore Satish, Michael Gardner, Charles Wang, Amin Pakzad, Barbaros Cetiner, & Wael Elhaddad. (2025). NHERI-SimCenter/PBE: Version 4.2.0 (v4.2.0). Zenodo. https://doi.org/10.5281/zenodo.14807364 \n\n2) Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matthew J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706");

  mainWindow.setCite(citeText);

  QString manualURL("https://nheri-simcenter.github.io/PBE-Documentation/");
  mainWindow.setDocumentationURL(manualURL);

  //QString messageBoardURL("https://simcenter-messageboard.designsafe-ci.org/smf/index.php?board=7.0");
  QString messageBoardURL("https://github.com/orgs/NHERI-SimCenter/discussions/categories/pbe");  
  mainWindow.setFeedbackURL(messageBoardURL);

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

  mainWindow.show();

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

  //Setting Google Analytics Tracking Information
#ifdef _SC_RELEASE

  qDebug() << "Running a Release Version of PBE";  
  GoogleAnalytics::SetMeasurementId("G-JWNPJMZVTK");
  GoogleAnalytics::SetAPISecret("CL5znZLfQv6N2Tk1RJVMWg");
  GoogleAnalytics::CreateSessionId();
  GoogleAnalytics::StartSession();


  // Opening a QWebEngineView and using github to get app geographic usage
  QWebEngineView view;
  view.setUrl(QUrl("https://nheri-simcenter.github.io/PBE/GA4.html"));
  view.resize(1024, 750);
  view.show();
  view.hide();

#endif

#ifdef _ANALYTICS

  qDebug() << "compiled with: ANALYTICS";  
  GoogleAnalytics::SetMeasurementId("G-JWNPJMZVTK");
  GoogleAnalytics::SetAPISecret("CL5znZLfQv6N2Tk1RJVMWg");
  GoogleAnalytics::CreateSessionId();
  GoogleAnalytics::StartSession();

#endif  


  //
  // RUN the GUI
  //
  
  int res = a.exec();

#ifdef _GA_AFTER

  qDebug() << "compiled with: _GA_AFTER";
  // Opening a QWebEngineView and using github to get app geographic usage
  QWebEngineView view;
  view.setUrl(QUrl("https://nheri-simcenter.github.io/PBE/GA4.html"));
  view.resize(1024, 750);
  view.show();
  view.hide();

#endif
  
  //
  // on done with event loop, logout & stop the thread
  //

  theRemoteService->logout();
  thread->quit();

  GoogleAnalytics::EndSession();
  // done
  return res;
}
