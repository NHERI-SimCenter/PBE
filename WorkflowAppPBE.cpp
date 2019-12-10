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
#include <QTreeView>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QStackedWidget>
#include <QProcess>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <RemoteService.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QHostInfo>
#include <QUuid>
#include <QSettings>
#include "CustomizedItemModel.h"
#include <StandardEarthquakeEDP.h>

// SimCenter Widgets
#include <EarthquakeEventSelection.h>
#include <RunLocalWidget.h>
#include <RemoteService.h>
#include <GeneralInformationWidget.h>
#include <SIM_Selection.h>
#include <RandomVariablesContainer.h>
#include <UQ_EngineSelection.h>
#include <InputWidgetOpenSeesAnalysis.h>
#include <LocalApplication.h>
#include <RemoteApplication.h>
#include <RemoteJobManager.h>
#include <LossModel/LossModelSelection.h>
#include <RunWidget.h>
#include <InputWidgetBIM.h>
#include <InputWidgetUQ.h>
#include <ResultsPelicun.h>

#include "CustomizedItemModel.h"
#include <ResultsPelicun.h>
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

    theRVs = new RandomVariablesContainer();
    theGI = GeneralInformationWidget::getInstance();
    theSIM = new SIM_Selection(theRVs);
    theEvent = new EarthquakeEventSelection(theRVs);
    theAnalysis = new InputWidgetOpenSeesAnalysis(theRVs);
    theUQ_Selection = new UQ_EngineSelection(theRVs);
    theDLModelSelection = new LossModelSelection(theRVs);
    theResults = new ResultsPelicun();

    localApp = new LocalApplication("PBE_workflow.py");
    remoteApp = new RemoteApplication("PBE_workflow.py", theService);
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

    // error messages and signals
    connect(theResults,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theResults,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theResults,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(theGI,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theGI,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theGI,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(theSIM,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theSIM,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theSIM,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(theEvent,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theEvent,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theEvent,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(theRunWidget,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(theRunWidget,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(theRunWidget,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));


    connect(localApp,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(localApp,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(localApp,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));

    connect(remoteApp,SIGNAL(sendErrorMessage(QString)), this,SLOT(errorMessage(QString)));
    connect(remoteApp,SIGNAL(sendStatusMessage(QString)), this,SLOT(statusMessage(QString)));
    connect(remoteApp,SIGNAL(sendFatalMessage(QString)), this,SLOT(fatalMessage(QString)));


    connect(localApp,SIGNAL(setupForRun(QString &,QString &)),
            this, SLOT(setUpForApplicationRun(QString &,QString &)));
    connect(remoteApp,SIGNAL(setupForRun(QString &,QString &)),
            this, SLOT(setUpForApplicationRun(QString &,QString &)));

    connect(this, SIGNAL(setUpForApplicationRunDone(QString&, QString &, QString)),
            theRunWidget, SLOT(setupForRunApplicationDone(QString&, QString &, QString)));

    connect(localApp, SIGNAL(processResults(QString, QString, QString)),
            this, SLOT(processResults(QString, QString, QString)));



    connect(theJobManager,
            SIGNAL(processResults(QString , QString, QString)),
            this,
            SLOT(processResults(QString, QString, QString)));
    connect(theJobManager,SIGNAL(loadFile(QString)), this, SLOT(loadFile(QString)));

    connect(remoteApp,SIGNAL(successfullJobStart()), theRunWidget, SLOT(hide()));

    //connect(theRunLocalWidget, SIGNAL(runButtonPressed(QString, QString)), this, SLOT(runLocal(QString, QString)));

    //
    // some of above widgets are inside some tabbed widgets
    //

    //theBIM = new InputWidgetBIM(theGI, theSIM);
    theUQ = new InputWidgetUQ(theUQ_Selection,theRVs);

    //
    //  NOTE: for displaying the widgets we will use a QTree View to label the widgets for selection
    //  and we will use a QStacked widget for displaying the widget. Which of widgets displayed in StackedView depends on
    //  item selected in tree view.
    //

    //
    // create layout to hold tree view and stackedwidget
    //

    horizontalLayout = new QHBoxLayout();
    this->setLayout(horizontalLayout);

    //
    // create a TreeView widget & provide items for each widget to be displayed & add to layout
    //

    treeView = new QTreeView();
    standardModel = new CustomizedItemModel;// QStandardItemModel ;
    QStandardItem *rootNode = standardModel->invisibleRootItem();

    //defining bunch of items for inclusion in model
    QStandardItem *giItem = new QStandardItem("GI");
    QStandardItem *bimItem = new QStandardItem("SIM");
    QStandardItem *evtItem = new QStandardItem("EVT");
    QStandardItem *uqItem   = new QStandardItem("UQ");
    QStandardItem *femItem = new QStandardItem("FEM");
    QStandardItem *contentsItem = new QStandardItem("DL");
    QStandardItem *resultsItem = new QStandardItem("RES");

    //building up the hierarchy of the model
    rootNode->appendRow(giItem);
    rootNode->appendRow(bimItem);
    rootNode->appendRow(evtItem);
    rootNode->appendRow(femItem);
    rootNode->appendRow(uqItem);
    rootNode->appendRow(contentsItem);
    rootNode->appendRow(resultsItem);

    infoItemIdx = rootNode->index();

    //register the model
    treeView->setModel(standardModel);
    treeView->expandAll();
    treeView->setHeaderHidden(true);
    treeView->setMaximumWidth(100);
    treeView->setMinimumWidth(100);
    treeView->setEditTriggers(QTreeView::EditTrigger::NoEditTriggers);//Disable Edit for the TreeView

    //
    // customize the apperance of the menu on the left
    //

    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff ); // hide the horizontal scroll bar
    treeView->setObjectName("treeViewOnTheLeft");
    treeView->setIndentation(0);
    QFile fileeeuq(":/styles/stylesheet_eeuq.qss");
    QFile filebar(":/styles/menuBar.qss");
    if(fileeeuq.open(QFile::ReadOnly) && filebar.open(QFile::ReadOnly)) {
        QString styleeeuq = QLatin1String(fileeeuq.readAll());
        QString stylebar = QLatin1String(filebar.readAll());
        this->setStyleSheet(styleeeuq + stylebar);
        fileeeuq.close();
        filebar.close();
    }
    else
        qDebug() << "Open Style File Failed!";



    //
    // set up so that a slection change triggers the selectionChanged slot
    //

    QItemSelectionModel *selectionModel= treeView->selectionModel();
    connect(selectionModel,
            SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this,
            SLOT(selectionChangedSlot(const QItemSelection &, const QItemSelection &)));

    // add the TreeView widget to the layout
    horizontalLayout->addWidget(treeView);

    //
    // create the staked widget, and add to it the widgets to be displayed, and add the stacked widget itself to layout
    //

    theStackedWidget = new QStackedWidget();
    theStackedWidget->addWidget(theGI);
    theStackedWidget->addWidget(theSIM);
    theStackedWidget->addWidget(theEvent);
    theStackedWidget->addWidget(theAnalysis);
    theStackedWidget->addWidget(theUQ);
    theStackedWidget->addWidget(theDLModelSelection);
    theStackedWidget->addWidget(theResults);

    // add stacked widget to layout
    horizontalLayout->addWidget(theStackedWidget);

    // set current selection to GI
    treeView->setCurrentIndex( infoItemIdx );

    // access a web page which will increment the usage count for this tool
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/eeuq/use.php")));

    // access a web page which will increment the usage count for this tool
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));

    manager->get(QNetworkRequest(QUrl("http://opensees.berkeley.edu/OpenSees/developer/bfm/use.php")));

    theGI->setDefaultProperties(1,144,360,360,37.426,-122.171);
}

WorkflowAppPBE::~WorkflowAppPBE()
{

}


void
WorkflowAppPBE::selectionChangedSlot(const QItemSelection & /*newSelection*/, const QItemSelection &/*oldSelection*/) {

    //get the text of the selected item
    const QModelIndex index = treeView->selectionModel()->currentIndex();
    QString selectedText = index.data(Qt::DisplayRole).toString();

    if (selectedText == "GI")
        theStackedWidget->setCurrentIndex(0);
    else if (selectedText == "SIM")
        theStackedWidget->setCurrentIndex(1);
    else if (selectedText == "EVT")
        theStackedWidget->setCurrentIndex(2);
    else if (selectedText == "FEM")
        theStackedWidget->setCurrentIndex(3);
    else if (selectedText == "UQ")
        theStackedWidget->setCurrentIndex(4);
    else if (selectedText == "DL")
        theStackedWidget->setCurrentIndex(5);
    else if (selectedText == "RES")
        theStackedWidget->setCurrentIndex(6);
}


bool
WorkflowAppPBE::outputToJSON(QJsonObject &jsonObjectTop) {
    //
    // get each of the main widgets to output themselves
    //

    QJsonObject apps;

    QJsonObject jsonObjGenInfo;
    theGI->outputToJSON(jsonObjGenInfo);
    jsonObjectTop["GeneralInformation"] = jsonObjGenInfo;

    QJsonObject jsonObjStructural;
    theSIM->outputToJSON(jsonObjStructural);
    jsonObjectTop["StructuralInformation"] = jsonObjStructural;
    QJsonObject appsSIM;
    theSIM->outputAppDataToJSON(appsSIM);
    apps["Modeling"]=appsSIM;

    // FMK - note to self, random varaibales need to be changed
    //QJsonObject jsonObjectRVs;
    //theRVs->outputToJSON(jsonObjectRVs);
    //jsonObjectTop["RandomVariables"] = jsonObjectRVs;
    theRVs->outputToJSON(jsonObjectTop);

    QJsonObject appsEDP;
    appsEDP["Application"] = "StandardEarthquakeEDP";
    QJsonObject dataObj;
    appsEDP["ApplicationData"] = dataObj;
    apps["EDP"] = appsEDP;

    //QJsonObject jsonObjectUQ;
    //theUQ_Method->outputToJSON(jsonObjectUQ);
    //jsonObjectTop["UQ_Method"] = jsonObjectUQ;

    theUQ_Selection->outputToJSON(jsonObjectTop);

    //    QJsonObject appsUQ;
    //    theUQ_Method->outputAppDataToJSON(appsUQ);
    // apps["UQ"]=appsUQ;

    theUQ_Selection->outputAppDataToJSON(apps);

    QJsonObject jsonObjectAna;
    theAnalysis->outputToJSON(jsonObjectAna);
    jsonObjectTop["Simulation"] = jsonObjectAna;

    QJsonObject appsAna;
    theAnalysis->outputAppDataToJSON(appsAna);
    apps["Simulation"]=appsAna;

    QJsonObject edpData;
    jsonObjectTop["EDP"] = edpData;

   // NOTE: Events treated differently, due to array nature of objects
    theEvent->outputToJSON(jsonObjectTop);
    theEvent->outputAppDataToJSON(apps);

    theRunWidget->outputToJSON(jsonObjectTop);

    QJsonObject jsonLossModel;
    theDLModelSelection->outputToJSON(jsonLossModel);
    jsonObjectTop["DamageAndLoss"] = jsonLossModel;

    QJsonObject appsDL;
    theDLModelSelection->outputAppDataToJSON(appsDL, jsonLossModel);
    apps["DL"] = appsDL;

    jsonObjectTop["Applications"]=apps;
    
    return true;
}


 void
 WorkflowAppPBE::processResults(QString dakotaOut, QString dakotaTab, QString inputFile) {

   theResults->processResults(dakotaTab);
   theRunWidget->hide();
   treeView->setCurrentIndex(infoItemIdx);
   theStackedWidget->setCurrentIndex(6);
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
            emit errorMessage(": ERROR: failed to read GeneralInformation");
        }
    } else {
        emit errorMessage(" ERROR: failed to find GeneralInformation");
        return false;
    }

    //qDebug() << "Applications";
    if (jsonObject.contains("Applications")) {

        QJsonObject theApplicationObject = jsonObject["Applications"].toObject();

        //qDebug() << "Modeling";
        if (theApplicationObject.contains("Modeling")) {
            QJsonObject theObject = theApplicationObject["Modeling"].toObject();
            if (theSIM->inputAppDataFromJSON(theObject) == false) {
                emit errorMessage(" ERROR: failed to read Modeling Application");
            }
        } else {
            emit errorMessage(" ERROR: failed to find Modeling Application");
            return false;
        }

        // note: Events is different because the object is an Array
        //qDebug() << "Events";
        if (theApplicationObject.contains("Events")) {
            //  QJsonObject theObject = theApplicationObject["Events"].toObject(); it is null object, actually an array
            if (theEvent->inputAppDataFromJSON(theApplicationObject) == false) {
                emit errorMessage(" ERROR: failed to read Event Application");
            }

        } else {
            emit errorMessage(" ERROR: failed to find Event Application");
            return false;
        }


        //qDebug() << "UQ";
	/*
        if (theApplicationObject.contains("UQ")) {
            QJsonObject theObject = theApplicationObject["UQ"].toObject();
            if (theUQ_Method->inputAppDataFromJSON(theObject) == false)
                emit errorMessage(" ERROR: failed to read UQ application");
        } else {
            emit errorMessage(" ERROR: failed to find UQ application");
            return false;
        }
	*/

        if (theUQ_Selection->inputAppDataFromJSON(theApplicationObject) == false)
            emit errorMessage("PBE: failed to read UQ application");

        //qDebug() << "Simulation";
        if (theApplicationObject.contains("Simulation")) {
            QJsonObject theObject = theApplicationObject["Simulation"].toObject();
            if (theAnalysis->inputAppDataFromJSON(theObject) == false)
                emit errorMessage(" ERROR: failed to read Simulation Application");
        } else {
            emit errorMessage(" ERROR: failed to find Simulation Application");
            return false;
        }


    } else
        return false;

    /*
    ** Note to me - RVs and Events treated differently as both use arrays .. rethink API!
    */

    theEvent->inputFromJSON(jsonObject);
    theRVs->inputFromJSON(jsonObject);
    theRunWidget->inputFromJSON(jsonObject);

    //qDebug() << "Structural Information";
    if (jsonObject.contains("StructuralInformation")) {
        QJsonObject jsonObjStructuralInformation = jsonObject["StructuralInformation"].toObject();
        if (theSIM->inputFromJSON(jsonObjStructuralInformation) == false) {
            emit errorMessage(" ERROR: failed to read StructuralInformation");
        }
    } else {
        emit errorMessage(" ERROR: failed to find StructuralInformation");
        return false;
    }

    //qDebug() << "UQ Method";
    /*
    if (jsonObject.contains("UQ_Method")) {
        QJsonObject jsonObjUQInformation = jsonObject["UQ_Method"].toObject();
        if (theUQ_Method->inputFromJSON(jsonObjUQInformation) == false)
            emit errorMessage(" ERROR: failed to read UQ Method data");
    } else {
        emit errorMessage(" ERROR: failed to find UQ Method data");
        return false;
    }
    */

    if (theUQ_Selection->inputFromJSON(jsonObject) == false)
        emit errorMessage("PBE: failed to read UQ Method data");

    //qDebug() << "Simulation";
    if (jsonObject.contains("Simulation")) {
        QJsonObject jsonObjSimInformation = jsonObject["Simulation"].toObject();
        if (theAnalysis->inputFromJSON(jsonObjSimInformation) == false)
            emit errorMessage(" ERROR: failed to read Simulation data");
    } else {
        emit errorMessage(" ERROR: failed to find Simulation data");
        return false;
    }

    //qDebug() << "DamageAndLoss";
    if (jsonObject.contains("DamageAndLoss")) {
        QJsonObject jsonObjLossModel = jsonObject["DamageAndLoss"].toObject();
        if (theDLModelSelection->inputFromJSON(jsonObjLossModel) == false)
            emit errorMessage(" ERROR: failed to find Damage and Loss Model");
    } else {
        emit errorMessage("WARNING: failed to find Damage and Loss Model");
        return false;
    }

    return true;
}


void
WorkflowAppPBE::onRunButtonClicked() {
    theRunWidget->showLocalApplication();
    GoogleAnalytics::ReportLocalRun();
}

void
WorkflowAppPBE::onRemoteRunButtonClicked(){
    emit errorMessage("");

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

    emit errorMessage("");

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
    // in new templatedir dir save the UI data into dakota.json file (same result as using saveAs)
    // NOTE: we append object workingDir to this which points to template dir
    //

    QString inputFile = templateDirectory + QDir::separator() + tr("dakota.json");

    QFile file(inputFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        //errorMessage();
        return;
    }
    QJsonObject json;
    this->outputToJSON(json);

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

    emit setUpForApplicationRunDone(tmpDirectory, inputFile, runType);
}

void
WorkflowAppPBE::loadFile(const QString fileName){

    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        emit errorMessage(QString("Could Not Open File: ") + fileName);
        return;
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
    this->inputFromJSON(jsonObj);

}


int
WorkflowAppPBE::getMaxNumParallelTasks() {
    return theUQ_Selection->getNumParallelTasks();
}
