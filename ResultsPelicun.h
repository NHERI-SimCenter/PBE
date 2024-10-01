#ifndef RESULTS_PELICUN_H
#define RESULTS_PELICUN_H

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

#include <SimCenterWidget.h>

#include <QtCharts/QChart>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
using namespace QtCharts;
#endif

class QTextEdit;
class QVBoxLayout;
class QTabWidget;
class MyTableWidget;
using namespace QtCharts;
class ResultsPelicun : public SimCenterWidget
{
    Q_OBJECT
public:
    explicit ResultsPelicun(QWidget *parent = 0);
    ~ResultsPelicun();

    bool outputToJSON(QJsonObject &jsonObject);
    bool inputFromJSON(QJsonObject &jsonObject);

    int processResults(QString &inputFile, QString &resultsDir);
    int processREDiResults(QString &inputFileName, QString &resultsDirName);

signals:

public slots:
   void clear(void);
   void onSpreadsheetCellClicked(int, int);

private:
   void getColData(QVector<double> &data, int numRow, int col);
   void getColDataExt(QList<QPointF> &dataXY, int numRow, int colX,
                      int colY, bool doMap);
   QWidget *createSummaryItem(QString &name, double mean, double stdDev, int valueType);
   QWidget *createSummaryItem2(QString &name, double mean, double stdDev, double logStdDev,
        double min, double p10, double p50, double p90, double max);
   QWidget *createSummaryHeader();

   int runPelicunAfterHPC(QString &resultsDirName, QString &runDirName,
                          QString &appDirName);

   QTabWidget *tabWidget;
   QTextEdit  *dakotaText;
   MyTableWidget *spreadsheet;
   QChart *chart;

   int col1, col2;
   bool mLeft;
   QStringList theHeadings;
   QMap<QString, QString> resultsToShow;

   QVector<QString>theNames;
   QVector<double>theMeans;
   QVector<double>theStdDevs;
   int dataType; // min/max or mean/stdDev
   QVBoxLayout *summaryLayout = nullptr;

};

#endif // RESULTS_PELICUN_H
