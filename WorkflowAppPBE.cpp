/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include "WorkflowAppPBE.h"

// Qt Widgets
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QLabel>
#include <QDebug>
#include <QHBoxLayout>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <RemoteService.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QSettings>
#include <StandardEarthquakeEDP.h>

// SimCenter Widgets
#include <SimCenterComponentSelection.h>
#include <EarthquakeEventSelection.h>
#include <RunLocalWidget.h>
#include <RemoteService.h>
#include <GeneralInformationWidget.h>
#include <SIM_Selection.h>
#include <RandomVariablesContainer.h>
#include <UQ_EngineSelection.h>
#include <FEA_Selection.h>
#include <LocalApplication.h>
#include <RemoteApplication.h>
#include <RemoteJobManager.h>
#include <LossModel/LossModelSelection.h>
#include <RunWidget.h>
#include <InputWidgetBIM.h>
#include <ResultsPelicun.h>
#include <Utils/PythonProgressDialog.h>

#include <GoogleAnalytics.h>

// static pointer for global procedure set in constructor
static WorkflowAppPBE *theApp = nullptr;

// global procedure
int getNumParallelTasks() {
    return theApp->getMaxNumParallelTasks();
}

WorkflowAppPBE::WorkflowAppPBE(RemoteService *theService, QWidget *parent)
    : WorkflowAppWidget(theService, parent)
{

    theApp = this;

    //
    // create the various widgets
    //

    theRVs = RandomVariablesContainer::getInstance();
    theGI = GeneralInformationWidget::getInstance();
    theSIM = new SIM_Selection();
    theEvent = new EarthquakeEventSelection(theRVs, theGI);
    theAnalysis = new FEA_Selection();
    theUQ_Selection = new UQ_EngineSelection(ForwardOnly);
    theDLModelSelection = new LossModelSelection();
    theResults = new ResultsPelicun();

    localApp = new LocalApplication("sWHALE.py");
    remoteApp = new RemoteApplication("sWHALE.py", theService);
    //    localApp = new LocalApplication("femUQ.py");
    //    remoteApp = new RemoteApplication("femUQ.py", theService);
    theJobManager = new RemoteJobManager(theService);

    // theRunLocalWidget = new RunLocalWidget(theUQ_Method);
    SimCenterWidget *theWidgets[1];
    //    theWidgets[0] = theAnalysis;
    //    theWidgets[1] = theUQ_Method;
    //int numWidgets = 2;
    theRunWidget = new RunWidget(localApp, remoteApp, theWidgets, 0);

    //
    // connect signals and slots
    //

    connect(localApp,SIGNAL(setupForRun(QString &,QString &)),
            this, SLOT(setUpForApplicationRun(QString &,QString &)));
    connect(remoteApp,SIGNAL(setupForRun(QString &,QString &)),
            this, SLOT(setUpForApplicationRun(QString &,QString &)));

    connect(this, SIGNAL(setUpForApplicationRunDone(QString&, QString &)), 
            theRunWidget, SLOT(setupForRunApplicationDone(QString&, QString &)));

    connect(localApp, SIGNAL(processResults(QString &)),
            this, SLOT(processResults(QString &)));

    connect(localApp,SIGNAL(runComplete()), this, SLOT(runComplete()));
    connect(remoteApp,SIGNAL(successfullJobStart()), this, SLOT(runComplete()));
    connect(theService, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    connect(theJobManager, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    connect(localApp,SIGNAL(runComplete()), this, SLOT(runComplete()));    
    
    connect(theJobManager,
            SIGNAL(processResults(QString &)),
            this,
            SLOT(processResults(QString &)));
    connect(theJobManager,SIGNAL(loadFile(QString&)), this, SLOT(loadFile(QString&)));

    connect(remoteApp,SIGNAL(successfullJobStart()), theRunWidget, SLOT(hide()));

    //
    // create layout, create component selction & add to layout & then add components to cmponentselection
    //

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    this->setContentsMargins(0,5,0,5);
    horizontalLayout->setMargin(0);

    theComponentSelection = new SimCenterComponentSelection();
    horizontalLayout->addWidget(theComponentSelection);

    theComponentSelection->addComponent(QString("UQ"),  theUQ_Selection);
    theComponentSelection->addComponent(QString("GI"),  theGI);
    theComponentSelection->addComponent(QString("SIM"), theSIM);
    theComponentSelection->addComponent(QString("EVT"), theEvent);
    theComponentSelection->addComponent(QString("FEM"), theAnalysis);
    theComponentSelection->addComponent(QString("RV"),  theRVs);
    theComponentSelection->addComponent(QString("DL"),  theDLModelSelection);
    theComponentSelection->addComponent(QString("RES"), theResults);

    theComponentSelection->displayComponent("UQ");

    // access a web page which will increment the usage count for this tool
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/eeuq/use.php")));

    theGI->setDefaultProperties(1,144,360,360,37.426,-122.171);
    PythonProgressDialog *theDialog=PythonProgressDialog::getInstance();
    theDialog->appendInfoMessage("Welcome to PBE");
    //    theDialog->hideAfterElapsedTime(1);
}

WorkflowAppPBE::~WorkflowAppPBE()
{  
   // hack to get around a sometimes occuring seg fault
   // as some classes in destructur remove RV from the RVCOntainer
   // which may already have been destructed .. so removing so no destructor calles
   //QWidget *newUQ = new QWidget();
   //theComponentSelection->swapComponent("RV",newUQ);
}

bool
WorkflowAppPBE::outputToJSON(QJsonObject &jsonObjectTop) {

  bool result = true;
  
    //
    // get each of the main widgets to output themselves
    //

    QJsonObject apps;

    QJsonObject jsonObjGenInfo;
    result = theGI->outputToJSON(jsonObjGenInfo);    
    if (result == false)
        return result;
    
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    result = theRVs->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;    

    // built in EDP
    QJsonObject appsEDP;
    appsEDP["Application"] = "StandardEarthquakeEDP";
    QJsonObject dataObj;
    appsEDP["ApplicationData"] = dataObj;
    apps["EDP"] = appsEDP;
    
    QJsonObject edpData;
    jsonObjectTop["EDP"] = edpData;
    
    if (theSIM->outputAppDataToJSON(apps) == false)
        return result;

    if (theSIM->outputToJSON(jsonObjectTop) == false)
      return false;;
    
    result = theUQ_Selection->outputAppDataToJSON(apps);
    if (result == false)
        return result;
    
    result = theUQ_Selection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;    

    result = theAnalysis->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    result = theAnalysis->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

   // NOTE: Events treated differently, due to array nature of objects
    result = theEvent->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    result = theEvent->outputAppDataToJSON(apps);
    if (result == false)
        return result;

    theRunWidget->outputToJSON(jsonObjectTop);

    QJsonObject jsonLossModel;
    if (theDLModelSelection->outputToJSON(jsonLossModel) == false)
      return false;
    
    jsonObjectTop["DamageAndLoss"] = jsonLossModel;

    QJsonObject appsDL;
    if (theDLModelSelection->outputAppDataToJSON(appsDL, jsonLossModel) == false)
      return false;
    
    apps["DL"] = appsDL;

    jsonObjectTop["Applications"]=apps;

    QJsonObject defaultValues;
    defaultValues["workflowInput"]=QString("scInput.json");    
    defaultValues["filenameBIM"]=QString("BIM.json");
    defaultValues["filenameEVENT"] = QString("EVENT.json");
    defaultValues["filenameSAM"]= QString("SAM.json");
    defaultValues["filenameEDP"]= QString("EDP.json");
    defaultValues["filenameSIM"]= QString("SIM.json");
    defaultValues["driverFile"]= QString("driver");
    defaultValues["filenameDL"]= QString("BIM.json");
    defaultValues["workflowOutput"]= QString("EDP.json");
    QJsonArray rvFiles, edpFiles;
    rvFiles.append(QString("BIM.json"));
    rvFiles.append(QString("SAM.json"));
    rvFiles.append(QString("EVENT.json"));
    rvFiles.append(QString("SIM.json"));
    edpFiles.append(QString("EDP.json"));
    defaultValues["rvFiles"]= rvFiles;
    defaultValues["edpFiles"]=edpFiles;
    jsonObjectTop["DefaultValues"]=defaultValues;
    
    return true;
}


void
WorkflowAppPBE::processResults(QString &dirPath) {
  
  theResults->processResults(loadedFile, dirPath);
  theRunWidget->hide();
  theComponentSelection->displayComponent("RES");
  this->runComplete();
}

void
WorkflowAppPBE::clear(void)
{
    theGI->clear();
    theSIM->clear();
}

bool
WorkflowAppPBE::inputFromJSON(QJsonObject &jsonObject)
{
    //
    // get each of the main widgets to input themselves
    //


    //qDebug() << "General Info";
    if (jsonObject.contains("GeneralInformation")) {
        QJsonObject jsonObjGeneralInformation = jsonObject["GeneralInformation"].toObject();
        if (theGI->inputFromJSON(jsonObjGeneralInformation) == false) {
            this->errorMessage(": ERROR: failed to read GeneralInformation");
        }
    } else {
        this->errorMessage(" ERROR: failed to find GeneralInformation");
        return false;
    }

    //qDebug() << "Applications";
    if (jsonObject.contains("Applications")) {

        QJsonObject theApplicationObject = jsonObject["Applications"].toObject();

        // note: Events is different because the object is an Array
        //qDebug() << "Events";
        if (theApplicationObject.contains("Events")) {
            //  QJsonObject theObject = theApplicationObject["Events"].toObject(); it is null object, actually an array
            if (theEvent->inputAppDataFromJSON(theApplicationObject) == false) {
                this->errorMessage(" ERROR: failed to read Event Application");
            }

        } else {
            this->errorMessage(" ERROR: failed to find Event Application");
            return false;
        }


        if (theUQ_Selection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("PBE: failed to read UQ application");

        if (theSIM->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("EE_UQ: failed to read SIM application");
	
        if (theAnalysis->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("EE_UQ: failed to read FEM application");

    } else
        return false;

    /*
    ** Note to me - RVs and Events treated differently as both use arrays .. rethink API!
    */

    theEvent->inputFromJSON(jsonObject);
    theRVs->inputFromJSON(jsonObject);
    theRunWidget->inputFromJSON(jsonObject);

    if (theUQ_Selection->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read UQ Method data");

    if (theSIM->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read FEM Method data");    
    
    if (theAnalysis->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read FEM Method data");

    if (jsonObject.contains("DamageAndLoss")) {
        QJsonObject jsonObjLossModel = jsonObject["DamageAndLoss"].toObject();
        if (theDLModelSelection->inputFromJSON(jsonObjLossModel) == false)
            this->errorMessage(" ERROR: failed to find Damage and Loss Model");
    } else {
        this->errorMessage("WARNING: failed to find Damage and Loss Model");
        return false;
    }
    
    this->runComplete();
    
    return true;
}


void
WorkflowAppPBE::onRunButtonClicked() {
    theRunWidget->showLocalApplication();
    GoogleAnalytics::ReportLocalRun();
}

void
WorkflowAppPBE::onRemoteRunButtonClicked(){
    this->errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {

        theRunWidget->hide();
        theRunWidget->showRemoteApplication();

    } else {
        errorMessage("ERROR - You Need to Login");
    }

    GoogleAnalytics::ReportDesignSafeRun();
}

void
WorkflowAppPBE::onRemoteGetButtonClicked(){

    this->errorMessage("");

    bool loggedIn = theRemoteService->isLoggedIn();

    if (loggedIn == true) {

        theJobManager->hide();
        theJobManager->updateJobTable("");
        theJobManager->show();

    } else {
        errorMessage("ERROR - You Need to Login");
    }
}

void
WorkflowAppPBE::onExitButtonClicked(){

}

void
WorkflowAppPBE::setUpForApplicationRun(QString &workingDir, QString &subDir) {

    errorMessage("");

    //
    // create temporary directory in working dir
    // and copy all files needed to this directory by invoking copyFiles() on app widgets
    //

    QString tmpDirName = QString("tmp.SimCenter");
    qDebug() << "TMP_DIR: " << tmpDirName;
    QDir workDir(workingDir);

    QString tmpDirectory = workDir.absoluteFilePath(tmpDirName);
    QDir destinationDirectory(tmpDirectory);

    if(destinationDirectory.exists()) {
      destinationDirectory.removeRecursively();
    } else
      destinationDirectory.mkpath(tmpDirectory);

    QString templateDirectory  = destinationDirectory.absoluteFilePath(subDir);
    destinationDirectory.mkpath(templateDirectory);

    qDebug() << "templateDir: " << templateDirectory;

    theSIM->copyFiles(templateDirectory);
    theEvent->copyFiles(templateDirectory);
    theAnalysis->copyFiles(templateDirectory);
    theUQ_Selection->copyFiles(templateDirectory);

    //
    // in new templatedir dir save the UI data into scInput.json file (same result as using saveAs)
    // NOTE: we append object workingDir to this which points to template dir
    //

    QString inputFile = templateDirectory + QDir::separator() + tr("scInput.json");

    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        //errorMessage();
        return;
    }
    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        this->errorMessage("WorkflowApp - failed in outputToJson");
        return;
    }    

    json["runDir"]=tmpDirectory;
    json["WorkflowType"]="Building Simulation";

    // if the EDPs are loaded for an external file, then there is no need
    // to run the whole simulation
    QJsonObject DL_app_data;
    QString runType = QString("run");
    DL_app_data = ((json["Applications"].toObject())["DL"].toObject())["ApplicationData"].toObject();
    if (DL_app_data.contains("filenameEDP")) {
        runType = QString("loss_only");
    }

    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    statusMessage("SetUp Done .. Now starting application");

    emit setUpForApplicationRunDone(tmpDirectory, inputFile);
}

int
WorkflowAppPBE::loadFile(QString &fileName){

    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        this->errorMessage(QString("Could Not Open File: ") + fileName);
        return -1;
    }

    //
    // place contents of file into json object
    //

    QString val;
    val=file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject jsonObj = doc.object();

    // close file
    file.close();

    //
    // clear current and input from new JSON
    //

    this->clear();
    bool result = this->inputFromJSON(jsonObj);
    if (result == false)
      return -1;
    
    this->runComplete();

    loadedFile = fileName;

    this->statusMessage("Done Loading File");
    return 0;
}


int
WorkflowAppPBE::getMaxNumParallelTasks() {
    return theUQ_Selection->getNumParallelTasks();
}
