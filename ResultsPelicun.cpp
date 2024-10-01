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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

#include <ResultsPelicun.h>
#include <QProcess>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QApplication>

#include <QTabWidget>
#include <QTextEdit>
#include <MyTableWidget.h>
#include <QDebug>
#include <QHBoxLayout>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QFileInfo>
#include <QScrollArea>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QVXYModelMapper>
#include <math.h>
#include <QValueAxis>
#include <QXYSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QBarCategoryAxis>
#include <QLabel>
#include <QSettings>

#include <SimCenterPreferences.h>

#define NUM_DIVISIONS 10

ResultsPelicun::ResultsPelicun(QWidget *parent)
    : SimCenterWidget(parent)
{
    // title & add button
    tabWidget = new QTabWidget(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(tabWidget,1);
    mLeft = true;
    col1 = 0;
    col2 = 0;
    this->setLayout(layout);
}

ResultsPelicun::~ResultsPelicun()
{

}


void ResultsPelicun::clear(void)
{
  //
  // get the tab widgets and delete them
  //

    QWidget *res=tabWidget->widget(0);
    QWidget *dat=tabWidget->widget(1);

   if (res != NULL)
       delete res;
   if (dat != NULL)
       delete dat;

    tabWidget->clear();

    //
    // clear any data we have stored
    // 

    theHeadings.clear();
    theNames.clear();
    theMeans.clear();
    theStdDevs.clear();
    
}



bool
ResultsPelicun::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    int numEDP = theNames.count();

    // quick return .. noEDP -> no analysis done -> no results out
    if (numEDP == 0)
      return true;

    jsonObject["resultType"]=QString(tr("ResultsPelicun"));

    //
    // add summary data
    //

    QJsonArray resultsData;
    for (int i=0; i<numEDP; i++) {
        QJsonObject edpData;
        edpData["name"]=theNames.at(i);
        edpData["mean"]=theMeans.at(i);
        edpData["stdDev"]=theStdDevs.at(i);
        resultsData.append(edpData);
    }
    jsonObject["summary"]=resultsData;
    jsonObject["dataType"]=dataType;


    // add general data
    jsonObject["general"]=dakotaText->toPlainText();

    //
    // add spreadsheet data
    //

    QJsonObject spreadsheetData;

    int numCol = spreadsheet->columnCount();
    int numRow = spreadsheet->rowCount();

    spreadsheetData["numRow"]=numRow;
    spreadsheetData["numCol"]=numCol;

    QJsonArray headingsArray;
    for (int i = 0; i <theHeadings.size(); ++i) {
        headingsArray.append(QJsonValue(theHeadings.at(i)));
    }

    spreadsheetData["headings"]=headingsArray;

    QJsonArray dataArray;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    for (int row = 0; row < numRow; ++row) {
        for (int column = 0; column < numCol; ++column) {
            QTableWidgetItem *theItem = spreadsheet->item(row,column);
            QString textData = theItem->text();
            dataArray.append(textData.toDouble());
        }
    }
    QApplication::restoreOverrideCursor();
    spreadsheetData["data"]=dataArray;

    jsonObject["spreadsheet"] = spreadsheetData;
    return result;
}


bool
ResultsPelicun::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    this->clear();

    //
    // create a summary widget in which place basic output (name, mean, stdDev)
    //

    QWidget *summaryWidget = new QWidget();
    QVBoxLayout *summaryLayout = new QVBoxLayout();
    summaryWidget->setLayout(summaryLayout);

    QJsonArray edpArray = jsonObject["summary"].toArray();
    QJsonValue type = jsonObject["dataType"];
    if (!type.isNull()) {
        dataType = type.toInt();
    } else
        dataType = 0;

    foreach (const QJsonValue &edpValue, edpArray) {
        QString name;
        double mean, stdDev;
        QJsonObject edpObject = edpValue.toObject();
        QJsonValue theNameValue = edpObject["name"];
        name = theNameValue.toString();

        QJsonValue theMeanValue = edpObject["mean"];
        mean = theMeanValue.toDouble();

        QJsonValue theStdDevValue = edpObject["stdDev"];
        stdDev = theStdDevValue.toDouble();
    }
    summaryLayout->addStretch();


    //
    // place widget in scrollable area
    //

    QScrollArea *summary = new QScrollArea;
    summary->setWidgetResizable(true);
    summary->setLineWidth(0);
    summary->setFrameShape(QFrame::NoFrame);
    summary->setWidget(summaryWidget);


    //
    // into a QTextEdit place more detailed Dakota text
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    QJsonValue theValue = jsonObject["general"];
    dakotaText->setText(theValue.toString());

    //
    // into a spreadsheet place all the data returned
    //

    
    spreadsheet = new MyTableWidget();
    QJsonObject spreadsheetData = jsonObject["spreadsheet"].toObject();
    int numRow = spreadsheetData["numRow"].toInt();
    int numCol = spreadsheetData["numCol"].toInt();
    spreadsheet->setColumnCount(numCol);
    spreadsheet->setRowCount(numRow);

    QJsonArray headingData= spreadsheetData["headings"].toArray();
    for (int i=0; i<numCol; i++) {
        theHeadings << headingData.at(i).toString();
    }

    spreadsheet->setHorizontalHeaderLabels(theHeadings);

    QJsonArray dataData= spreadsheetData["data"].toArray();
    int dataCount =0;
    for (int row =0; row<numRow; row++) {
        for (int col=0; col<numCol; col++) {
            QModelIndex index = spreadsheet->model()->index(row, col);
            spreadsheet->model()->setData(index, dataData.at(dataCount).toDouble());
            dataCount++;
        }
    }
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);
    QScatterSeries *series = new QScatterSeries;
    col1 = 0;
    col2 = numCol-3;
    mLeft = true;

    this->onSpreadsheetCellClicked(0,numCol-3);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();

    //
    // create a widget into which we place the chart and the spreadsheet
    //

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(chartView, 1);
    //    layout->addWidget(spreadsheet, 1);

    QScrollArea *tableScroll = new QScrollArea;
    tableScroll->setWidgetResizable(true);
    tableScroll->setLineWidth(0);
    tableScroll->setFrameShape(QFrame::NoFrame);
    tableScroll->setWidget(spreadsheet);
    layout->addWidget(tableScroll);
    
    //
    // add 3 Widgets to TabWidget
    //

    tabWidget->addTab(summary,tr("Summmary"));
    tabWidget->addTab(widget, tr("Data Values"));

    tabWidget->adjustSize();
    return result;
}


static void merge_helper(double *input, int left, int right, double *scratch)
{
    // if one element: done  else: recursive call and then merge
    if(right == left + 1) {
        return;
    } else {
        int length = right - left;
        int midpoint_distance = length/2;
        /* l and r are to the positions in the left and right subarrays */
        int l = left, r = left + midpoint_distance;

        // sort each subarray
        merge_helper(input, left, left + midpoint_distance, scratch);
        merge_helper(input, left + midpoint_distance, right, scratch);

        // merge the arrays together using scratch for temporary storage
        for(int i = 0; i < length; i++) {
            /* Check to see if any elements remain in the left array; if so,
            * we check if there are any elements left in the right array; if
            * so, we compare them.  Otherwise, we know that the merge must
            * use take the element from the left array */
            if(l < left + midpoint_distance &&
                    (r == right || fmin(input[l], input[r]) == input[l])) {
                scratch[i] = input[l];
                l++;
            } else {
                scratch[i] = input[r];
                r++;
            }
        }
        // Copy the sorted subarray back to the input
        for(int i = left; i < right; i++) {
            input[i] = scratch[i - left];
        }
    }
}

static int mergesort(double *input, int size)
{
    double *scratch = new double[size];
    if(scratch != NULL) {
        merge_helper(input, 0, size, scratch);
        delete [] scratch;
        return 1;
    } else {
        return 0;
    }
}

int ResultsPelicun::runPelicunAfterHPC(QString &resultsDirName,
                                       QString &runDirName,
                                       QString &appDirName){

    //
    // create workdir & copy dakotaTab.out & scInput there
    //
    
    if (!QDir(runDirName).exists()) {
        QDir().mkdir(runDirName);
    }
    
    QDir runDir(runDirName);
    QDir resultsDir(resultsDirName);
    QDir downloadDir(resultsDirName); downloadDir.cdUp(); // templateDir.cd("templatedir");

    QString downloadedTemplateDir = downloadDir.absoluteFilePath("templatedir");
    QString runTemplateDir = runDir.absoluteFilePath("templatedir");
    QDir templateDir(runTemplateDir);
    
    runDir.rename(downloadedTemplateDir, runTemplateDir);

    QFile::copy(resultsDir.absoluteFilePath("dakotaTab.out"),
                runDir.absoluteFilePath("dakotaTab.out"));

    QDir tmpDir(runTemplateDir);
    QFile::copy(templateDir.absoluteFilePath("scInput.json"),
                runDir.absoluteFilePath("scInput.json"));

    //
    // run the loss assessment
    //
    
    QDir scriptDir(appDirName);
    scriptDir.cd("applications");
    scriptDir.cd("Workflow");
    QString pySCRIPT = scriptDir.absoluteFilePath("sWHALE.py");
    QString createResponseCSV = scriptDir.absoluteFilePath("createResponseCSV.py");    
    QString registryFile = scriptDir.absoluteFilePath("WorkflowApplications.json");
    QString inputFileName = runDir.absoluteFilePath("scInput.json");    

    QProcess *proc = new QProcess();
    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    QString python = preferences->getPython();

    QFileInfo pythonFile(python);
    if (pythonFile.exists()) {
      QString pythonPath = pythonFile.absolutePath();
    } else {
      errorMessage("No python found, see the manual");
      return 0;
    }

    errorMessage("Now Running Pelicun to deremine losses");

#ifdef Q_OS_WIN
    python = QString("\"") + python + QString("\"");
    QStringList argsCSV{createResponseCSV,"--inputFile",inputFileName};
    proc->execute(python, argsCSV);    
    QStringList args{pySCRIPT, "loss_only",inputFileName,registryFile};
    proc->execute(python, args);

#else
    
    // note the above not working under linux because basrc not being called so no env variables!!
    QString sourceBash("");
    QDir homeDir(QDir::homePath());    
    if (homeDir.exists(".bash_profile")) {
        sourceBash = QString("source $HOME/.bash_profile; \"");
    } else if (homeDir.exists(".bashrc")) {
        sourceBash = QString("source $HOME/.bashrc; \"");
    } else if (homeDir.exists(".zprofile")) {
        sourceBash = QString("source $HOME/.zprofile; \"");
    } else if (homeDir.exists(".zshrc")) {
        sourceBash = QString("source $HOME/.zshrc; \"");
    } else
        this->errorMessage( "No .bash_profile, .bashrc, .zprofile or .zshrc file found. This may not find Dakota or OpenSees");
    
    QString command = sourceBash + python + QString("\" \"") +
      createResponseCSV + QString("\" --inputFile \"") + inputFileName +
      QString("\" > /Users/fmckenna/output.log 2>&1; \"") + python + QString("\" \"") +      
      pySCRIPT + QString("\"  \"loss_only\" \"") + inputFileName + QString("\"  \"") +
      registryFile + QString("\"");

    qDebug() << "PYTHON COMMAND: " << command;
    proc->execute("bash", QStringList() << "-c" <<  command);

#endif

    proc->waitForStarted();

    return 0;
}

int ResultsPelicun::processResults(QString &inputFileName,
				   QString &resultsDirName) {

  QDir rDir(resultsDirName);
  QFile inputFile(inputFileName);

  inputFile.open(QFile::ReadOnly | QFile::Text);
  
  QString val;
  val=inputFile.readAll();
  QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
  QJsonObject inputData = doc.object();
  inputFile.close();

  // If the runType is HPC, then we need to do additional operations
  QString runType = inputData["runType"].toString();

  qDebug() << "inputFile: " << inputFileName;
  qDebug() << "resultsDIR: " << resultsDirName;
  qDebug() << "runTYPE: " << runType;
  
    if (runType == "HPC" || runType == "runningRemote"){

        qDebug() << "Loaded response data from HPC, running performance assessment locally.";

        QString runDirName = inputData["runDir"].toString();
        QString appDirName = SimCenterPreferences::getInstance()->getAppDir();

        this->runPelicunAfterHPC(resultsDirName, runDirName, appDirName);

        // move the resultsDir to the runDir
        resultsDirName = runDirName;

        qDebug() << "Performance assessment finished successfully.";
    }

    qDebug() << "Loading performance assessment results";

    this->clear();
    mLeft = true;
    col1 = 0;
    col2 = 0;

    //
    // get a Qwidget ready to place summary data, the EDP name, mean, stdDev into
    //

    QWidget *summaryWidget = new QWidget();
    summaryLayout = new QVBoxLayout();
    summaryWidget->setLayout(summaryLayout);

    //
    // place contents of Dakota more detailed output into a QTextEdit
    //

    dakotaText = new QTextEdit();
    dakotaText->setReadOnly(true); // make it so user cannot edit the contents
    dakotaText->setText("\n");

    // check if the main DL result file is available
    QString resultsStatsFile = resultsDirName + "/DL_summary_stats.csv";
    std::ifstream fileResultsStats(resultsStatsFile.toStdString().c_str());
    if (!fileResultsStats.is_open()) {

        // Now we know that something is wrong...

        /*
        // skip all of the dakota messages because they are confusing users

        //
        // check dakota actually ran the FE simulations so that blame may
        // be properly assessed .. i.e. not always the fault of pelicun.
        //

        QString filenameTab = resultsDirName + QDir::separator() + "dakotaTab.out";
      
        QFileInfo fileTabInfo(filenameTab);
        QString filenameErrorString = fileTabInfo.absolutePath() + QDir::separator() + QString("dakota.err");

        QFileInfo filenameErrorInfo(filenameErrorString);
        if (!filenameErrorInfo.exists()) {
            errorMessage("No dakota.err file - dakota did not run - problem with dakota setup or the applicatins failed with inputs provided");
            return 0;
        }
        QFile fileError(filenameErrorString);
        QString line("");
        if (fileError.open(QIODevice::ReadOnly)) {
           QTextStream in(&fileError);
           while (!in.atEnd()) {
              line = in.readLine();
           }
           fileError.close();
        }

        if (line.length() != 0) {
            qDebug() << line.length() << " " << line;
            errorMessage(QString(QString("Error Running Dakota: ") + line));
            return 0;
        }

        QFileInfo filenameTabInfo(filenameTab);
        if (!filenameTabInfo.exists()) {
            errorMessage("No dakotaTab.out file - dakota failed .. possibly no QoI");
            return 0;
        }
        */

        errorMessage(
            QString("Could not open file: ") + resultsStatsFile + 
            QString(".<br>Damage and loss results are not available. See the "
                "log (above) for more information on the error."));
        return -1;
    }

    // If we get until this point, then the DL_summary_stats file is available.

    //
    // first 4 lines contain summary data
    //

    std::string summaryDummy;
    std::string summaryName, summaryMean, summaryStdDev, summaryLogStdDev;
    std::string summaryMin, summary10, summary50, summary90, summaryMax;

    std::string tokenName, tokenMean, tokenStd, tokenLogStd;
    std::string tokenMin, token10, token50, token90, tokenMax;

    std::getline(fileResultsStats, summaryName);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summaryMean);
    std::getline(fileResultsStats, summaryStdDev);
    std::getline(fileResultsStats, summaryLogStdDev);
    std::getline(fileResultsStats, summaryMin);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summary10);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summary50);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summary90);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summaryDummy);
    std::getline(fileResultsStats, summaryMax);

    std::istringstream ssName(summaryName);
    std::istringstream ssMean(summaryMean);
    std::istringstream ssStd(summaryStdDev);
    std::istringstream ssLogStd(summaryLogStdDev);
    std::istringstream ssMin(summaryMin);
    std::istringstream ss10(summary10);
    std::istringstream ss50(summary50);
    std::istringstream ss90(summary90);
    std::istringstream ssMax(summaryMax);

    std::getline(ssName, tokenName, ',');
    std::getline(ssMean, tokenMean, ',');
    std::getline(ssStd, tokenStd, ',');
    std::getline(ssLogStd, tokenLogStd, ',');
    std::getline(ssMin, tokenMin, ',');
    std::getline(ss10, token10, ',');
    std::getline(ss50, token50, ',');
    std::getline(ss90, token90, ',');
    std::getline(ssMax, tokenMax, ',');

    // ignore first

    int colCount = 1;

    //theHeadings << "Percent";
    theHeadings << "Realization";

    QWidget *theSummaryHeader = this->createSummaryHeader();
    summaryLayout->addWidget(theSummaryHeader);
    summaryLayout->addSpacing(10);

    QFrame *sepLine = new QFrame;
    sepLine->setFrameShape(QFrame::HLine);
    sepLine->setFrameShadow(QFrame::Sunken);
    summaryLayout->addWidget(sepLine);

    resultsToShow.clear();
    
    //resultsToShow.insert("inhabitants/","inhabitants");
    //resultsToShow.insert("red_tagged/","red tagged?");

    resultsToShow.insert("collapse","Collapsed?");
    
    resultsToShow.insert("irreparable","Irreparable?");

    resultsToShow.insert("repair_cost","Repair Cost");
    resultsToShow.insert("repair_cost-","Repair Cost");

    resultsToShow.insert("repair_time","Repair Time");
    resultsToShow.insert("repair_time-","Repair Time");
    resultsToShow.insert("repair_time-sequential","Repair Time - sequential");
    resultsToShow.insert("repair_time-parallel","Repair Time - parallel");
    
    resultsToShow.insert("repair_carbon","Embodied Carbon in Repairs");
    resultsToShow.insert("repair_carbon-","Embodied Carbon in Repairs");
    resultsToShow.insert("repair_energy","Embodied Energy in Repairs");
    resultsToShow.insert("repair_energy-","Embodied Energy in Repairs");

    //resultsToShow.insert("injuries/sev1","injuries-1");
    //resultsToShow.insert("injuries/sev2","injuries-2");
    //resultsToShow.insert("injuries/sev3","injuries-3");
    //resultsToShow.insert("injuries/sev4","injuries-4");
    //resultsToShow.insert("highest_damage_state/S","top DS S");
    //resultsToShow.insert("highest_damage_state/NSA","top DS NSA");
    //resultsToShow.insert("highest_damage_state/NSD","top DS NSD");

    // go along the column names in the header
    while(std::getline(ssName, tokenName, ',')) {

        std::getline(ssMean, tokenMean, ',');
        std::getline(ssStd, tokenStd, ',');
        std::getline(ssLogStd, tokenLogStd, ',');
        std::getline(ssMin, tokenMin, ',');
        std::getline(ss10, token10, ',');
        std::getline(ss50, token50, ',');
        std::getline(ss90, token90, ',');
        std::getline(ssMax, tokenMax, ',');

        QString DV_name(tokenName.c_str());
        std::string::size_type sz;

        double DV_mean = std::stod(tokenMean.c_str(), &sz);
        double DV_stdDev = std::stod(tokenStd.c_str(), &sz);
        double DV_logStdDev;
        try{
            DV_logStdDev = std::stod(tokenLogStd.c_str(), &sz);
        }
        catch(...){
            DV_logStdDev = -1;
        }
        double DV_min = std::stod(tokenMin.c_str(), &sz);
        double DV_10 = std::stod(token10.c_str(), &sz);
        double DV_50 = std::stod(token50.c_str(), &sz);
        double DV_90 = std::stod(token90.c_str(), &sz);
        double DV_max = std::stod(tokenMax.c_str(), &sz);

        theHeadings << DV_name;

        if (resultsToShow.contains(DV_name)) {

            QString DV_DisplayName = resultsToShow.value(DV_name);

            QWidget *theWidget = this->createSummaryItem2(DV_DisplayName,
                DV_mean, DV_stdDev, DV_logStdDev, DV_min, DV_10, DV_50, DV_90, DV_max);
            summaryLayout->addWidget(theWidget);

            // add a separator line after the row
            QFrame *sepLine = new QFrame;
            sepLine->setFrameShape(QFrame::HLine);
            sepLine->setFrameShadow(QFrame::Sunken);
            summaryLayout->addWidget(sepLine);
        }

        colCount++;
    }

    summaryLayout->addStretch();

    //
    // place summary widget in scrollable area
    //

    QScrollArea *summary = new QScrollArea;
    summary->setWidgetResizable(true);
    summary->setLineWidth(0);
    summary->setFrameShape(QFrame::NoFrame);
    summary->setWidget(summaryWidget);

    tabWidget->addTab(summary,"Summary");

    qDebug() << "Summary statistics successfully loaded.";

    //
    // now parse the file with all realizations
    //

    spreadsheet = new MyTableWidget();

    std::string inputLine;
   // std::getline(fileResults, inputLine);
   // std::istringstream iss(inputLine);


    resultsToShow.insert("#","#");
    //resultsToShow.insert("Realization","#");
    //resultsToShow.insert("event_time/month","month");
    //resultsToShow.insert("event_time/weekday?","weekday?");
    //resultsToShow.insert("event_time/hour","hour");
    //resultsToShow.insert("collapses/mode","collapse mode");

    QStringList modHeadings = QStringList();

    for (const auto& colHeader: theHeadings){
        modHeadings << resultsToShow.value(colHeader);
    }

    colCount = modHeadings.count();
    spreadsheet->setColumnCount(colCount);
    spreadsheet->setHorizontalHeaderLabels(modHeadings);

    // now read the file with the detailed results
    //DL_Summary
    QString resultsFile = resultsDirName + "/DL_summary.csv";
    std::ifstream fileResults(resultsFile.toStdString().c_str());
    if (!fileResults.is_open()) {
        errorMessage(
            QString("Could not open file: ") + resultsFile +
            QString(" . Damage and loss results are not available."));
        return -1;
    }

    std::getline(fileResults, summaryName);

    // now until end of file, read lines and place data into spreadsheet
    // (do not read more than 20000 lines to avoid visualization issues)
    int rowCount = 0;
    while ((std::getline(fileResults, inputLine)) && (rowCount <= 20000)) {
        spreadsheet->insertRow(rowCount);
        std::istringstream line(inputLine);
        std::string value;
        int col=0;
        while (std::getline(line, value, ',')) {
            QModelIndex index = spreadsheet->model()->index(rowCount, col);
            if (col != 0)
                spreadsheet->model()->setData(index, value.c_str());
            else
                spreadsheet->model()->setData(index, rowCount);

            col++;
        }
        rowCount++;
    }


    if (rowCount == 0) {
      errorMessage("Damage and loss result file is empty.");
      return -2;
    }
   // rowCount;
    spreadsheet->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(spreadsheet,SIGNAL(cellPressed(int,int)),this,SLOT(onSpreadsheetCellClicked(int,int)));

    //
    // create a chart, setting data points from first and last col of spreadsheet
    //

    chart = new QChart();
    chart->setAnimationOptions(QChart::AllAnimations);

    this->onSpreadsheetCellClicked(0,colCount-3);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->chart()->legend()->hide();

    //
    // into QWidget place chart and spreadsheet
    //

    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(chartView, 3);
    layout->addWidget(spreadsheet, 1);

    //
    // add summary, detained info and spreadsheet with chart to the tabed widget
    //

   // tabWidget->addTab(summary,tr("Summmary"));
   // tabWidget->addTab(dakotaText, tr("General"));
    tabWidget->addTab(widget, tr("Data Values"));

    tabWidget->adjustSize();
    tabWidget->setCurrentIndex(0);

    fileResultsStats.close();
    fileResults.close();

    // close input file

    // clear messages
    errorMessage("");
    return 0;
}



int ResultsPelicun::processREDiResults(QString &inputFileName,
                                   QString &resultsDirName) {

    if(summaryLayout == nullptr)
      return 0;

    QDir rDir(resultsDirName);
    QFile inputFile(inputFileName);

    inputFile.open(QFile::ReadOnly | QFile::Text);

    QString val;
    val=inputFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject inputData = doc.object();
    inputFile.close();

    auto redi_res_path = resultsDirName + QDir::separator() + "REDi_output" + QDir::separator() + "redi_summary_stats.json";

    QFile resultsSummaryFile(redi_res_path);

    if(!resultsSummaryFile.exists())
      return 0;

    resultsSummaryFile.open(QFile::ReadOnly | QFile::Text);

    QString val2;
    val2=resultsSummaryFile.readAll();
    QJsonDocument res_doc = QJsonDocument::fromJson(val2.toUtf8());
    QJsonObject resData = res_doc.object();
    resultsSummaryFile.close();


    if(resData.isEmpty())
      this->errorMessage("Error, the REDi results file "+redi_res_path+" is empty or failed to load as json");


    summaryLayout->addWidget(new QLabel("<b>REDi Recovery</b>"), 0, Qt::AlignCenter);

    // add a separator line after the row
    QFrame *sepLine = new QFrame;
    sepLine->setFrameShape(QFrame::HLine);
    sepLine->setFrameShadow(QFrame::Raised);
    summaryLayout->addWidget(sepLine);


    QJsonObject::const_iterator it;
    for (it = resData.constBegin(); it != resData.constEnd(); ++it) {
      QString key = it.key();
      QJsonValue value = it.value().toObject();

      auto DV_DisplayName = key;

      double DV_min = value["min"].toDouble();
      double DV_10 = value["0.10%"].toDouble();
      double DV_50 = value["50%"].toDouble();
      double DV_90 = value["90%"].toDouble();
      double DV_max = value["max"].toDouble();

      double DV_mean = value["mean"].toDouble();
      double DV_stdDev = value["std"].toDouble();
      double DV_logStdDev;
      if (value["log_std"] != "") {
        DV_logStdDev = value["log_std"].toDouble();
      } else {
        DV_logStdDev = -1;
      }

      QWidget *theWidget = this->createSummaryItem2(DV_DisplayName,
                                                    DV_mean, DV_stdDev, DV_logStdDev, DV_min, DV_10, DV_50, DV_90, DV_max);
      summaryLayout->addWidget(theWidget);

      // add a separator line after the row
      QFrame *sepLine = new QFrame;
      sepLine->setFrameShape(QFrame::HLine);
      sepLine->setFrameShadow(QFrame::Sunken);
      summaryLayout->addWidget(sepLine);


    }

    return 0;
}


void
ResultsPelicun::getColData(QVector<double> &data, int numRow, int col) {
    bool ok;
    double data0 = spreadsheet->item(0,col)->text().toDouble(&ok);
    if (ok == true) {
        for (int i=0; i<numRow; i++) {
            QTableWidgetItem *item = spreadsheet->item(i,col);
            data.append(item->text().toDouble());
        }
    } else { // it's a string create a map
         QMap<QString, int> map;
         int numDifferent = 1;
         for (int i=0; i<numRow; i++) {
             QTableWidgetItem *item = spreadsheet->item(i,col);
             QString text = item->text();
             if (map.contains(text))
                 data.append(map.value(text));
             else {
                 data.append(numDifferent);
                 map[text] = numDifferent++;
             }
         }
    }
    return;
}

void
ResultsPelicun::getColDataExt(QList<QPointF> &dataXY, int numRow, int colX,
                              int colY, bool doMap) {

    if (doMap == true) {
        //If doMap is set to True, then we assume a list of strings and create
        // a map of the results.

        if (colY != colX) {
            QMap<QString, int> mapX, mapY;
            int numDifferentX = 0, numDifferentY = 0;
            for (int i=0; i<numRow; i++) {
                QString textX = spreadsheet->item(i,colX)->text();
                if (mapX.contains(textX) == 0) mapX[textX] = numDifferentX++;
                QString textY = spreadsheet->item(i,colY)->text();
                if (mapY.contains(textY) == 0) mapY[textY] = numDifferentY++;
                QPointF dataP;
                dataP.setX(mapX.value(textX));
                dataP.setY(mapY.value(textY));
                dataXY.append(dataP);
            }
        } else {
            QMap<QString, int> mapX;
            int numDifferentX = 0;
            for (int i=0; i<numRow; i++) {
                QString textX = spreadsheet->item(i,colX)->text();
                if (mapX.contains(textX) == 0) mapX[textX] = numDifferentX++;
                QPointF dataP;
                dataP.setX(mapX.value(textX));
                dataP.setY(0.0);
                dataXY.append(dataP);
            }
        }
    } else {
        // If doMap is set to False, then we assume that both vectors contain
        // floating point numbers only. Empty cells are also allowed and their
        // values are not displayed.

        if (colY != colX) {
            // two columns
            bool ok_X, ok_Y;
            for (int i=0; i<numRow; i++) {
                double dataPX = spreadsheet->item(i,colX)->text().toDouble(&ok_X);
                double dataPY = spreadsheet->item(i,colY)->text().toDouble(&ok_Y);
                if ((ok_X==true) && (ok_Y==true)) {
                    QPointF dataP;
                    dataP.setX(dataPX);
                    dataP.setY(dataPY);
                    dataXY.append(dataP);
                }
            }
        } else {
            // single column
            bool ok_X;
            for (int i=0; i<numRow; i++) {
                double dataPX = spreadsheet->item(i,colX)->text().toDouble(&ok_X);
                if (ok_X==true) {
                    QPointF dataP;
                    dataP.setY(0.0);
                    dataP.setX(dataPX);
                    dataXY.append(dataP);
                }
            }
        }
    }
}

void
ResultsPelicun::onSpreadsheetCellClicked(int row, int col)
{
    //qDebug() << "onSPreadSheetCellClicked() :" << row << " " << col;
    mLeft = spreadsheet->wasLeftKeyPressed();

    // create a new series
    chart->removeAllSeries();
    //chart->removeA
    QAbstractAxis *oldAxisX=chart->axisX();
    if (oldAxisX != 0)
        chart->removeAxis(oldAxisX);
    QAbstractAxis *oldAxisY=chart->axisY();
    if (oldAxisY != 0)
        chart->removeAxis(oldAxisY);

    int oldCol = 0;
    if (mLeft == true) {
        oldCol= col2;
        col2 = col;
    } else {
        oldCol= col1;
        col1 = col;
    }    

    int rowCount = spreadsheet->rowCount();
    if (col1 != col2) {
        QScatterSeries *series = new QScatterSeries;

        // adjust marker size and opacity based on the number of samples
        if (rowCount < 10) {
            series->setMarkerSize(15.0);
            series->setColor(QColor(0, 114, 178, 200));
        } else if (rowCount < 100) {
            series->setMarkerSize(11.0);
            series->setColor(QColor(0, 114, 178, 160));
        } else if (rowCount < 1000) {
            series->setMarkerSize(8.0);
            series->setColor(QColor(0, 114, 178, 100));
        } else if (rowCount < 10000) {
            series->setMarkerSize(6.0);
            series->setColor(QColor(0, 114, 178, 70));
        } else if (rowCount < 100000) {
            series->setMarkerSize(5.0);
            series->setColor(QColor(0, 114, 178, 50));
        } else {
            series->setMarkerSize(4.5);
            series->setColor(QColor(0, 114, 178, 30));
        }
        
        series->setBorderColor(QColor(255,255,255,0));

        QList<QPointF> dataXY;
        this->getColDataExt(dataXY, rowCount, col1, col2, false);

        for (int i=0; i<rowCount; i++) {
            spreadsheet->item(i,col1)->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            spreadsheet->item(i,col2)->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            spreadsheet->item(i,oldCol)->setData(Qt::BackgroundRole, QColor(Qt::white));
        }

        series->append(dataXY);
        chart->addSeries(series);

        QValueAxis *axisX = new QValueAxis();
        QValueAxis *axisY = new QValueAxis();

        axisX->setTitleText(resultsToShow.value(theHeadings.at(col1)));
        axisY->setTitleText(resultsToShow.value(theHeadings.at(col2)));

        chart->setAxisX(axisX, series);
        chart->setAxisY(axisY, series);

        chart->zoom(0.95);

    } else {
        QList<QPointF> dataXY;
        this->getColDataExt(dataXY, rowCount, col1, col1, false);
        rowCount = dataXY.length();
        int totalSampleCount = spreadsheet->rowCount();

        int binCount = int(pow(rowCount, 1.0/3.0));
        if (binCount > 20) binCount = 20;
        //qDebug() << "row count: " << rowCount;
        //qDebug() << "bin count: " << binCount;

        // initialize histogram data
        QList<qreal> histogram;
        for (int i=0; i<binCount; i++)
            histogram.append(0);

        QList<qreal> dataValues;

        double min = dataXY[0].x();
        double max = min;
        for (int i=0; i<rowCount; i++) {
            spreadsheet->item(i,col1)->setData(Qt::BackgroundRole, QColor(Qt::lightGray));
            spreadsheet->item(i,oldCol)->setData(Qt::BackgroundRole, QColor(Qt::white));

            double value = dataXY[i].x();
            dataValues.append(value);
            //dataValues[i] = value;

            if (value < min) {
                min = value;
            } else if (value > max) {
                max = value;
            }
        }

        if (mLeft == true) {

            // frequency distribution
            double binSize = (max-min)/binCount;

            for (int i=0; i<rowCount; i++) {
                // compute block belongs to, watch under and overflow due to numerics
                int block = floor((dataValues[i]-min)/binSize);
                if (block < 0) block = 0;
                if (block > binCount-1) block = binCount-1;
                histogram[block] += 1;
            }

            double maxPercent = 0;
            QList<QString> xLabelList;
            for (int i=0; i<binCount; i++) {
                histogram[i] /= totalSampleCount;
                xLabelList.append(QString::number(min+(i+0.5)*binSize));
                if (histogram[i] > maxPercent)
                    maxPercent = histogram[i];
            }            

            QBarSet *set = new QBarSet("");
            set->append(histogram);

            QBarSeries *series = new QBarSeries();
            series->append(set);
            series->setBarWidth(0.9);
            series->setLabelsAngle(0.0);
            // series->setLabelsPrecision(4);

            chart->addSeries(series);
            chart->createDefaultAxes();

            QStringList xLabels = QStringList(xLabelList);
            QBarCategoryAxis *axisX = new QBarCategoryAxis();
            axisX->append(xLabels);
            axisX->setTitleText(resultsToShow.value(theHeadings.at(col1)));
            //axisX->setTickCount(NUM_DIVISIONS+1);
            chart->setAxisX(axisX, series);

            //axisX->setRange(min, max);

            //chart->setAxisX(axisX, series);

            QValueAxis *axisY = new QValueAxis();
            axisY->setRange(0, maxPercent);
            axisY->setTitleText("Frequency %");
            chart->setAxisY(axisY, series);
            chart->zoom(0.95);
    } else {

            QLineSeries *series= new QLineSeries;

            // cumulative distributionn
            std::sort(dataValues.begin(),dataValues.end());

            for (int i=0; i<rowCount; i++) {
                series->append(dataValues[i], 1.0*i/totalSampleCount);
            }

            chart->addSeries(series);
            QValueAxis *axisX = new QValueAxis();
            QValueAxis *axisY = new QValueAxis();

            axisX->setRange(min, max);
            axisY->setRange(0, 1);
            axisY->setTitleText("Cumulative Probability");
            axisX->setTitleText(resultsToShow.value(theHeadings.at(col1)));
            axisX->setTickCount(11);
            axisY->setTickCount(11);
            chart->setAxisX(axisX, series);
            chart->setAxisY(axisY, series);
            chart->zoom(0.995);
        }

    }
}


static QWidget *addLabeledLineEdit(QString theLabelName, QLineEdit **theLineEdit){

    QVBoxLayout *theLayout = new QVBoxLayout();
    QLabel *theLabel = new QLabel();
    theLabel->setText(theLabelName);
    QLineEdit *theEdit = new QLineEdit();
    theLabel->setMaximumWidth(200);
    theLabel->setMinimumWidth(200);
    theEdit->setMaximumWidth(200);
    theEdit->setMinimumWidth(200);
    theLayout->addWidget(theLabel);
    theLayout->addWidget(theEdit);
    theLayout->setSpacing(0);
    theLayout->setContentsMargins(0,0,0,0);

    QWidget *theWidget = new QWidget();
    theWidget->setLayout(theLayout);

    *theLineEdit = theEdit;
    return theWidget;
}

QWidget *
ResultsPelicun::createSummaryHeader() {

    QWidget *item = new QWidget;
    QHBoxLayout *itemLayout = new QHBoxLayout();
    item->setLayout(itemLayout);

    int columnWidth = 160;

    QLabel *nameLabel = new QLabel();
    nameLabel->setText(tr("<b><i>Decision Variable</i></b>"));
    nameLabel->setAlignment(Qt::AlignLeft);
    nameLabel->setFixedWidth(160);
    itemLayout->addWidget(nameLabel);

    QLabel *probLabel = new QLabel();
    probLabel->setText(tr("<b>Probability</b>"));
    probLabel->setAlignment(Qt::AlignRight);
    probLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(probLabel);

    QLabel *meanLabel = new QLabel();
    meanLabel->setText(tr("<b>Mean</b>"));
    meanLabel->setAlignment(Qt::AlignRight);
    meanLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(meanLabel);

    QLabel *stdLabel = new QLabel();
    stdLabel->setText(tr("<b>Standard Dev.</b>"));
    stdLabel->setAlignment(Qt::AlignRight);
    stdLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(stdLabel);

    QLabel *logStdLabel = new QLabel();
    logStdLabel->setText(tr("<b>Log Standard Dev.</b>"));
    logStdLabel->setAlignment(Qt::AlignRight);
    logStdLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(logStdLabel);

    QLabel *minLabel = new QLabel();
    minLabel->setText(tr("<b>Minimum</b>"));
    minLabel->setAlignment(Qt::AlignRight);
    minLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(minLabel);

    QLabel *p10Label = new QLabel();
    p10Label->setText(tr("<b>10<sup>th</sup> Percentile</b>"));
    p10Label->setAlignment(Qt::AlignRight);
    p10Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p10Label);

    QLabel *p50Label = new QLabel();
    p50Label->setText(tr("<b>Median</b>"));
    p50Label->setAlignment(Qt::AlignRight);
    p50Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p50Label);

    QLabel *p90Label = new QLabel();
    p90Label->setText(tr("<b>90<sup>th</sup> Percentile</b>"));
    p90Label->setAlignment(Qt::AlignRight);
    p90Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p90Label);

    QLabel *maxLabel = new QLabel();
    maxLabel->setText(tr("<b>Maximum</b>"));
    maxLabel->setAlignment(Qt::AlignRight);
    maxLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(maxLabel);

    itemLayout->addStretch();

    return item;
}

QWidget *
ResultsPelicun::createSummaryItem2(QString &name, double mean, double stdDev, double logStdDev,
    double min, double p10, double p50, double p90, double max) {

    QStringList probOnly = (QStringList() << "Collapsed?" << "Red Tagged?" <<
                            "Irreparable?" << "Excessive Repair Cost?" <<
                            "Excessive Repair Time?");

    QWidget *item = new QWidget;
    QHBoxLayout *itemLayout = new QHBoxLayout();
    item->setLayout(itemLayout);

    int columnWidth = 160;

    QLabel *nameLabel = new QLabel();
    nameLabel->setText("<i>"+name.replace("/",": ").replace("_"," ")+"</i>");
    nameLabel->setAlignment(Qt::AlignLeft);
    nameLabel->setFixedWidth(160);
    itemLayout->addWidget(nameLabel);
    theNames.append(name);

    QLabel *probLabel = new QLabel();
    QString probText;
    if (probOnly.contains(name)) {
        probLabel->setText(QString::number(mean));
    } else {
        probLabel->setText("-");
    }
    probLabel->setAlignment(Qt::AlignRight);
    probLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(probLabel);

    QLabel *meanLabel = new QLabel();
    if (probOnly.contains(name)) {
        meanLabel->setText("-");
    } else {
        meanLabel->setText(QString::number(mean));
    }
    meanLabel->setAlignment(Qt::AlignRight);
    meanLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(meanLabel);
    theMeans.append(mean);

    QLabel *stdLabel = new QLabel();
    if (probOnly.contains(name)) {
        stdLabel->setText("-");
    } else {
        stdLabel->setText(QString::number(stdDev));
    }
    stdLabel->setAlignment(Qt::AlignRight);
    stdLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(stdLabel);
    theStdDevs.append(stdDev);

    QLabel *logStdLabel = new QLabel();
    if (probOnly.contains(name)) {
        logStdLabel->setText("-");
    } else {
        if (logStdDev == -1){
            logStdLabel->setText("N/A");
        } else {
            logStdLabel->setText(QString::number(logStdDev));
        }
    }
    logStdLabel->setAlignment(Qt::AlignRight);
    logStdLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(logStdLabel);

    QLabel *minLabel = new QLabel();
    if (probOnly.contains(name)) {
        minLabel->setText("-");
    } else {
        minLabel->setText(QString::number(min));
    }
    minLabel->setAlignment(Qt::AlignRight);
    minLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(minLabel);

    QLabel *p10Label = new QLabel();
    if (probOnly.contains(name)) {
        p10Label->setText("-");
    } else {
        p10Label->setText(QString::number(p10));
    }
    p10Label->setAlignment(Qt::AlignRight);
    p10Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p10Label);

    QLabel *p50Label = new QLabel();
    if (probOnly.contains(name)) {
        p50Label->setText("-");
    } else {
        p50Label->setText(QString::number(p50));
    }
    p50Label->setAlignment(Qt::AlignRight);
    p50Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p50Label);

    QLabel *p90Label = new QLabel();
    if (probOnly.contains(name)) {
        p90Label->setText("-");
    } else {
        p90Label->setText(QString::number(p90));
    }
    p90Label->setAlignment(Qt::AlignRight);
    p90Label->setFixedWidth(columnWidth);
    itemLayout->addWidget(p90Label);

    QLabel *maxLabel = new QLabel();
    if (probOnly.contains(name)) {
        maxLabel->setText("-");
    } else {
        maxLabel->setText(QString::number(max));
    }
    maxLabel->setAlignment(Qt::AlignRight);
    maxLabel->setFixedWidth(columnWidth);
    itemLayout->addWidget(maxLabel);

    itemLayout->addStretch();

    return item;
}

QWidget *
ResultsPelicun::createSummaryItem(QString &name, double mean, double stdDev,
                                  int valueType) {

    dataType = valueType;

    QWidget *edp = new QWidget;
    QHBoxLayout *edpLayout = new QHBoxLayout();

    edp->setLayout(edpLayout);

    QLineEdit *nameLineEdit;
    QWidget *nameWidget = addLabeledLineEdit(QString("Name"), &nameLineEdit);
    nameLineEdit->setText(name);
    nameLineEdit->setDisabled(true);
    theNames.append(name);
    edpLayout->addWidget(nameWidget);

    QLineEdit *firstLineEdit;
    QWidget *firstWidget;
    if (valueType == 0)
       firstWidget = addLabeledLineEdit(QString("Mean"), &firstLineEdit);
    else
       firstWidget = addLabeledLineEdit(QString("Min"), &firstLineEdit);


    firstLineEdit->setText(QString::number(mean));
    firstLineEdit->setDisabled(true);
    theMeans.append(mean);
    edpLayout->addWidget(firstWidget);

    QLineEdit *secondLineEdit;
    QWidget *secondWidget ;
    if (valueType == 0)
        secondWidget = addLabeledLineEdit(QString("StdDev"), &secondLineEdit);
    else
     secondWidget = addLabeledLineEdit(QString("Max"), &secondLineEdit);
    secondLineEdit->setText(QString::number(stdDev));
    secondLineEdit->setDisabled(true);
    theStdDevs.append(stdDev);
    edpLayout->addWidget(secondWidget);

    edpLayout->addStretch();

    return edp;
}
