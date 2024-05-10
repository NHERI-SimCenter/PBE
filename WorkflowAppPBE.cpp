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
#include "MainWindowWorkflowApp.h"
#include "Utils/RelativePathResolver.h"

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
#include <PerformanceMethodSelection.h>
#include <RunWidget.h>
#include <InputWidgetBIM.h>
#include <ResultsPelicun.h>
#include <Utils/ProgramOutputDialog.h>

#include <GoogleAnalytics.h>

WorkflowAppPBE *
WorkflowAppPBE::getInstance(RemoteService *theService, QWidget *parent) {
  if (theInstance == 0)
    theInstance = new WorkflowAppPBE(theService, parent);

  return theInstance;
}

WorkflowAppPBE *WorkflowAppPBE::theInstance = 0;

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

    ProgramOutputDialog *theDialog=ProgramOutputDialog::getInstance();
    theDialog->appendInfoMessage("Initializing application...<br>");

    //
    // create the various widgets
    //

    theRVs = RandomVariablesContainer::getInstance();
    theGI = GeneralInformationWidget::getInstance();
    theSIM_Selection = new SIM_Selection();
    theEventSelection = new EarthquakeEventSelection(theRVs, theGI);
    theAnalysisSelection = new FEA_Selection();
    theUQ_Selection = new UQ_EngineSelection(ForwardOnly);
    theDLModelSelection = new LossModelSelection();
    thePrfMethodSelection = new PerformanceMethodSelection();
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
    connect(localApp,SIGNAL(sendErrorMessage(QString)),
	    this,SLOT(errorMessage(QString)));
    connect(localApp,SIGNAL(sendStatusMessage(QString)),
	    this,SLOT(statusMessage(QString)));
    connect(localApp,SIGNAL(sendFatalMessage(QString)),
	    this,SLOT(fatalMessage(QString)));
    connect(localApp,SIGNAL(runComplete()), this, SLOT(runComplete()));        
    connect(localApp, SIGNAL(processResults(QString &)),
            this, SLOT(processResults(QString &)));
    
    connect(remoteApp,SIGNAL(setupForRun(QString &,QString &)),
            this, SLOT(setUpForApplicationRun(QString &,QString &)));
    connect(remoteApp,SIGNAL(successfullJobStart()),
	    this, SLOT(runComplete()));
    connect(remoteApp,SIGNAL(sendErrorMessage(QString)),
	    this,SLOT(errorMessage(QString)));
    connect(remoteApp,SIGNAL(sendStatusMessage(QString)),
	    this,SLOT(statusMessage(QString)));
    connect(remoteApp,SIGNAL(sendFatalMessage(QString)),
	    this,SLOT(fatalMessage(QString)));    
    
    connect(this, SIGNAL(setUpForApplicationRunDone(QString&, QString &)), 
            theRunWidget, SLOT(setupForRunApplicationDone(QString&, QString &)));

    connect(theService, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    
    connect(theJobManager, SIGNAL(closeDialog()), this, SLOT(runComplete()));
    connect(theJobManager,
            SIGNAL(processResults(QString &)),
            this,
            SLOT(processResults(QString &)));
    
    connect(theJobManager,SIGNAL(loadFile(QString&)), this, SLOT(loadFile(QString&)));

    //connect(theJobManager,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    // connect(theJobManager,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));    
    
    connect(remoteApp,SIGNAL(successfullJobStart()), theRunWidget, SLOT(hide()));

    //
    // create layout, create component selction & add to layout & then add components to cmponentselection
    //

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);
    this->setContentsMargins(0,5,0,5);
    horizontalLayout->setContentsMargins(0,0,0,0);

    theComponentSelection = new SimCenterComponentSelection();
    horizontalLayout->addWidget(theComponentSelection);

    theComponentSelection->addComponent(QString("UQ"),  theUQ_Selection);
    theComponentSelection->addComponent(QString("GI"),  theGI);
    theComponentSelection->addComponent(QString("SIM"), theSIM_Selection);
    theComponentSelection->addComponent(QString("EVT"), theEventSelection);
    theComponentSelection->addComponent(QString("FEM"), theAnalysisSelection);
    theComponentSelection->addComponent(QString("RV"),  theRVs);
    theComponentSelection->addComponent(QString("DL"),  theDLModelSelection);
    theComponentSelection->addComponent(QString("PRF"), thePrfMethodSelection);
    theComponentSelection->addComponent(QString("RES"), theResults);

    theComponentSelection->displayComponent("DL");

    // access a web page which will increment the usage count for this tool
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/eeuq/use.php")));

    theGI->setDefaultProperties(1,144,360,360,37.426,-122.171);
    theGI->setLengthUnit(QString("ft")); // set default length unit to ft
    theDialog->appendBlankLine();
    theDialog->appendInfoMessage("Welcome to PBE<br>");
    //    theDialog->hideAfterElapsedTime(1);

//    QString test = "inputFileName";
//    QString test2 =  "/Users/stevan.gavrilovic/Documents/PBE/LocalWorkDir/tmp.SimCenter";

//    theResults->processResults(test,test2);
//    theResults->processREDiResults(test,test2);
}

WorkflowAppPBE::~WorkflowAppPBE()
{  
   // hack to get around a sometimes occuring seg fault
   // as some classes in destructur remove RV from the RVCOntainer
   // which may already have been destructed .. so removing so no destructor calls
   //QWidget *newUQ = new QWidget();
   //theComponentSelection->swapComponent("RV",newUQ);
}

bool
WorkflowAppPBE::outputToJSON(QJsonObject &jsonObjectTop) {

  bool result = true;
  
    //
    // get each of the main widgets to output themselves
    //

    this->outputFilePath = this->getTheMainWindow()->outputFilePath;

    qDebug() << "OUTPUT PATH: " << this->outputFilePath;
    
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

    result = theUQ_Selection->outputAppDataToJSON(apps);
    if (result == false) {

      qDebug() << "WorkflowAppPBE::outputToJSON - UQ_Selection failed appData";
        return result;
    }
    
    result = theUQ_Selection->outputToJSON(jsonObjectTop);
    if (result == false)
        return result;

    result = theSIM_Selection->outputAppDataToJSON(apps);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - SIM_Selection failed appData";      
        return result;
    }

    result = theSIM_Selection->outputToJSON(jsonObjectTop);
    if (result == false){
      qDebug() << "WorkflowAppPBE::outputToJSON - SIM_Selection";            
        return result;
    }
    
    result = theAnalysisSelection->outputAppDataToJSON(apps);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - ANA_Selection failed appData";      
        return result;
    }

    result = theAnalysisSelection->outputToJSON(jsonObjectTop);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - ANA_Selection failed";
        return result;
    }

   // NOTE: Events treated differently, due to array nature of objects
    result = theEventSelection->outputToJSON(jsonObjectTop);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - EVENT_Selection failed";      
        return result;
    }

    result = theEventSelection->outputAppDataToJSON(apps);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - EVENT_Selection failed appData";            
        return result;
    }

    result = theRunWidget->outputToJSON(jsonObjectTop);
    if (result == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - Run failed";      
        return result;
    }
    
    if (theDLModelSelection->outputToJSON(jsonObjectTop) == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - DL_Selection failed";
      return false;
    }

    if (theDLModelSelection->outputAppDataToJSON(apps) == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - DL_Selection failed appData";
      return false;
    }

    if (thePrfMethodSelection->outputToJSON(jsonObjectTop) == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - PRF_Selection failed";
      return false;
    }

    if (thePrfMethodSelection->outputAppDataToJSON(apps) == false) {
      qDebug() << "WorkflowAppPBE::outputToJSON - Prf Method Selection failed appData";
      return false;
    } 

    jsonObjectTop["Applications"]=apps;

    QJsonObject defaultValues;
    defaultValues["workflowInput"]=QString("scInput.json");    
    defaultValues["filenameAIM"]=QString("AIM.json");
    defaultValues["filenameEVENT"] = QString("EVENT.json");
    defaultValues["filenameSAM"]= QString("SAM.json");
    defaultValues["filenameEDP"]= QString("EDP.json");
    defaultValues["filenameSIM"]= QString("SIM.json");
    defaultValues["driverFile"]= QString("driver");
    defaultValues["filenameDL"]= QString("AIM.json");
    defaultValues["workflowOutput"]= QString("EDP.json");
    QJsonArray rvFiles, edpFiles;
    rvFiles.append(QString("AIM.json"));
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

  statusMessage("Backend Done. Processing Results ...");  
  theResults->processResults(loadedFile, dirPath);
  theResults->processREDiResults(loadedFile, dirPath);

  theRunWidget->hide();
  theComponentSelection->displayComponent("RES");
  this->runComplete();
}

void
WorkflowAppPBE::clear(void)
{
    theGI->clear();
    theSIM_Selection->clear();
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
            if (theEventSelection->inputAppDataFromJSON(theApplicationObject) == false) {
                this->errorMessage(" ERROR: failed to read Event Application");
            }

        } else {
            this->errorMessage(" ERROR: failed to find Event Application");
            return false;
        }

        if (theUQ_Selection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("PBE: failed to read UQ application");

        if (theSIM_Selection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("PBE: failed to read SIM application");
	
        if (theAnalysisSelection->inputAppDataFromJSON(theApplicationObject) == false)
            this->errorMessage("PBE: failed to read FEM application");

        if (thePrfMethodSelection->inputAppDataFromJSON(theApplicationObject) == false) 
	  this->errorMessage("PBE: failed to read PRF application");

        if (theDLModelSelection->inputAppDataFromJSON(theApplicationObject) == false) 
	  this->errorMessage("PBE: failed to read DL application");	

    } else
        return false;

    /*
    ** Note to me - RVs and Events treated differently as both use arrays .. rethink API!
    */

    theEventSelection->inputFromJSON(jsonObject);
    theRVs->inputFromJSON(jsonObject);
    theRunWidget->inputFromJSON(jsonObject);

    if (theUQ_Selection->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read UQ Method data");

    if (theSIM_Selection->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read FEM Method data");    
    
    if (theAnalysisSelection->inputFromJSON(jsonObject) == false)
        this->errorMessage("PBE: failed to read FEM Method data");

    if (thePrfMethodSelection->inputFromJSON(jsonObject) == false)
      this->errorMessage("PBE: failed to read PRF Method data");

    if (theDLModelSelection->inputFromJSON(jsonObject) == false)
      this->errorMessage("PBE: failed to read PRF Method data");    
    /*
    if (jsonObject.contains("DL")) {
        QJsonObject jsonObjLossModel = jsonObject["DL"].toObject();
        if (theDLModelSelection->inputFromJSON(jsonObjLossModel) == false)
            this->errorMessage(" ERROR: failed to find Damage and Loss Model");
    } else {
        this->errorMessage("WARNING: failed to find Damage and Loss Model");
        return false;
    }
    */
    
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

    theSIM_Selection->copyFiles(templateDirectory);
    theEventSelection->copyFiles(templateDirectory);
    theAnalysisSelection->copyFiles(templateDirectory);
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

    //QString outFilePath = tmpDirectory + QDir::separator() + tr("dakota.json");
    this->getTheMainWindow()->outputFilePath = inputFile;

    QJsonObject json;
    if (this->outputToJSON(json) == false) {
        this->errorMessage("WorkflowApp - failed in outputToJson");
        return;
    }    

    json["runDir"]=tmpDirectory;
    json["WorkflowType"]="Building Simulation";


    QJsonObject citations;
    QString citeFile = templateDirectory + QDir::separator() + tr("please_cite.json");    
    // QString citeFile = destinationDirectory.filePath("plases_cite.json"); // file getting deleted
    this->createCitation(citations, citeFile);
    // json.insert("citations",citations);    

    
    // if the EDPs are loaded from an external file, then there is no need
    // to run the whole simulation
    QJsonObject DL_app_data;    
    DL_app_data = (json["DL"].toObject())["Demands"].toObject();

    QString runType;
    if (DL_app_data.contains("DemandFilePath")) {
        runType = QString("loss_only");
    } else {
        runType = QString("run");
    }



    QJsonDocument doc(json);
    file.write(doc.toJson());
    file.close();

    
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

    // resolve file paths
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveAbsolutePaths(jsonObj, fileInfo.dir());

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

int
WorkflowAppPBE::createCitation(QJsonObject &citation, QString citeFile) {

  //
  // put PBE citation in
  // 

  QJsonObject citationPBE;
  citationPBE.insert("citation",
"Adam Zsarnoczay, Frank McKenna, Charles Wang, Stevan Gavrilovic, Michael Gardner, \
Sang-ri Yi, Aakash Bangalore Satish, & Wael Elhaddad. (2024). \
NHERI-SimCenter/PBE: Version 3.4.0 (V3.4.0). \
Zenodo. https://doi.org/10.5281/zenodo.10902085)");
  citationPBE.insert("description",
"This is the overall tool reference used to indicate the version of the tool.");

  QJsonObject citationSimCenterMarker;
  citationSimCenterMarker.insert("citation",
"Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, \
Ahsan Kareem, Wael Elhaddad, Laura Lowes, Matthew J. Schoettler, and Sanjay Govindjee (2020) \
A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural \
Hazards on the Built Environment. \
Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706");
  citationSimCenterMarker.insert("description",
"This marker paper describes the SimCenter application framework, which was designed \
to simulate the impacts of natural hazards on the built environment. It is a \
necessary attribute for publishing work resulting from the use of SimCenter tools, \
software, and datasets.");

  QJsonArray citationsArray;
  citationsArray.push_back(citationPBE);
  citationsArray.push_back(citationSimCenterMarker);

  QJsonObject unnecessaryCitationsElement;
  unnecessaryCitationsElement.insert("citations", citationsArray);

  citation.insert("PBE",unnecessaryCitationsElement);

  /*
  QString cit("{\"PBE\": { \"citations\": [{\"citation\": \"Adam Zsarnoczay, Frank McKenna, Charles Wang, Stevan Gavrilovic, Michael Gardner, Sang-ri Yi, Aakash Bangalore Satish, & Wael Elhaddad. (2024). NHERI-SimCenter/PBE: Version 3.4.0 (V3.4.0). Zenodo. https://doi.org/10.5281/zenodo.10902085\",\"description\": \"This is the overall tool reference used to indicate the version of the tool.\"},{\"citation\": \"Gregory G. Deierlein, Frank McKenna, Adam Zsarnóczay, Tracy Kijewski-Correa, Ahsan Kareem, Wael Elhaddad, Laura Lowes, Mat J. Schoettler, and Sanjay Govindjee (2020) A Cloud-Enabled Application Framework for Simulating Regional-Scale Impacts of Natural Hazards on the Built Environment. Frontiers in the Built Environment. 6:558706. doi: 10.3389/fbuil.2020.558706\",\"description\": \" This marker paper describes the SimCenter application framework, which was designed to simulate the impacts of natural hazards on the built environment.It  is a necessary attribute for publishing work resulting from the use of SimCenter tools, software, and datasets.\"}]}}");

  QJsonDocument docC = QJsonDocument::fromJson(cit.toUtf8());

  if(!docC.isNull()) {
    if(docC.isObject()) {
      citation = docC.object();        
    }  else {
      qDebug() << "WorkflowdAppEE_UQ citation text is not valid JSON: \n" << cit << endl;
    }
  }
  */

  theSIM_Selection->outputCitation(citation);
  theEventSelection->outputCitation(citation);
  theAnalysisSelection->outputCitation(citation);
  theUQ_Selection->outputCitation(citation);
  theDLModelSelection->outputCitation(citation);
  thePrfMethodSelection->outputCitation(citation);

  //
  // write the citation to a file if name of one provided
  //
  
  if (!citeFile.isEmpty()) {
    
    QFile file(citeFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
      errorMessage(QString("writeCitation - could not open file") + citeFile);
      progressDialog->hideProgressBar();
      return 0;
    }

    QJsonDocument doc(citation);
    file.write(doc.toJson());
    file.close();
  }
  
  return 0;    
}
