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
    QCoreApplication::setApplicationVersion("2.0.0");
    //    GoogleAnalytics::SetTrackingId("UA-126256136-1");
    GoogleAnalytics::StartSession();
    GoogleAnalytics::ReportStart();

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

  QString textAboutPBE = "\
          <p> \
          This is the Performance Based Engineering (PBE) Application.\
          <p> \
          The PBE Application (PBE App) is an open-source software \
          (https://github.com/NHERI-SimCenterPBE) that provides researchers a tool to \
          assess the performance of a building in an earthquake scenario. The application \
          focuses on quantifying building performance through decision variables. Given \
          that the properties of the buildings and the earthquake events are not known \
          exactly, and that the simulation software and the user make simplifying \
          assumptions in the numerical modeling of the structure, the estimate response of\
           the structure already exhibits significant variability. Such response can be \
           estimated using our EE-UQ Application (https://simcenter.designsafe-ci.org/\
              research-tools/ee-uq-application/). The PBE App builds on the EE-UQ App and \
           uses its response estimates to assess the damage to building components and the\
            consequences of such damage.\
          <p> \
          The user can characterize the simulation model, and the damage and loss model of\
           the structure, and the seismic hazard model in the PBE App. All models are \
           interconnected by an uncertainty quantification framework that allows the user \
           to define a flexible stochastic model for the problem. Given the stochastic \
           model, the application first performs nonlinear response history simulations to\
            get the Engineering Demand Parameters (EDPs) that describe structural response\
            . Then those EDPs are used to assess the Damage Measures (DM) and Decision \
            Variables (DV) that characterize structural performance. \
          <p> \
          Depending on the type of structural system, the fidelity of the numerical model \
          and the number of EDP samples required, the response history simulations can be \
          computationally prohibitively expensive. To overcome this impediment, the user \
          has the option to perform the response simulations on the Stampede2 \
          supercomputer. Stampede2 is located at the Texas Advanced Computing Center and \
          made available to the user through NHERI DesignSafe-CI, the cyberinfrastructure \
          provider for the distributed NSF funded Natural Hazards in Engineering Research \
          Infrastructure (NHERI) facility.\
          <p> \
          The computations are performed in a workflow application. That is, the numerical\
           simulations are actually performed by a sequence of different applications. The\
            PBE backend software runs these various applications for the user, taking the \
            outputs from some programs and providing them as inputs to others. The design \
            of the PBE App is such that researchers are able to modify the backend \
            application to utilize their own application in the workflow computations. \
            This will ensure researchers are not limited to using the default applications\
             we provide and will be enthused to provide their own applications for others \
             to use. \
          <p>\
          This is Version 2.0.0 of the tool and as such is limited in scope. Researchers are\
           encouraged to comment on what additional features and applications they would \
           like to see in the PBE App. If you want a feature, chances are many of your \
           colleagues would also benefit from it.\
          <p>";

     w.setAbout(textAboutPBE);

  QString version("Version 2.0.0");
  w.setVersion(version);

  QString citeText("Adam Zsarnoczay, Frank McKenna, Charles Wang, Wael Elhaddad, & Michael Gardner. (2019, October 15). NHERI-SimCenter/PBE: Release v2.0.0 (Version v2.0.00). Zenodo. http://doi.org/10.5281/zenodo.3491145");
  w.setCite(citeText);

  QString manualURL("https://www.designsafe-ci.org/data/browser/public/designsafe.storage.community//SimCenter/Software/PBE");
  w.setDocumentationURL(manualURL);


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
