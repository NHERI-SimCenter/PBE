#ifndef COMPONENT_CONTAINER_H
#define COMPONENT_CONTAINER_H

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

// Written: fmckenna, adamzs

#include "GeneralInformationWidget.h"
#include <Utils/ProgramOutputDialog.h>

#include <SimCenterWidget.h>
#include <SimCenterAppWidget.h>
#include "PelicunShared.h"
#include <QWebEngineView>

#include <quazip.h>
#include <quazipfile.h>

//#include <HDF5Handler.h>

class ComponentGroup;
class PopulationGroup;

class QCheckBox;
class QGridLayout;
class QHBoxLayout;
class QVBoxLayout;
class QLineEdit;
class QLabel;
class Component;
class QComboBox;
class QGroupBox;
class QPushButton;
class QStringListModel;
class QSignalMapper;
class SC_TableEdit;

class PelicunComponentContainer : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit PelicunComponentContainer(QWidget *parent = 0);

    ~PelicunComponentContainer();

    bool inputFromJSON(QJsonObject &jsonObject);
    bool outputToJSON(QJsonObject &jsonObject);
    bool copyFiles(QString &dirName);

    int setAdditionalComponentDB(QString additionalComponentDB);

    int saveComponentAssignment(QString filePath);
    int loadComponentAssignment(QString filePath);

    void addCompToOverview(QString compName, QVBoxLayout *compAreaVBox);

    void addComponent();
    void removeComponent();

    void clearCompGroupWidget();
    void retrieveCompGroups();

    //void parseCSVLine(QString &line, QStringList &line_list);

public slots:
   int updateAvailableComponents();

   void addOneComponent(void);
   void addAllComponents(void);
   void removeOneComponent(void);
   void removeAllComponents(void);
   void showSelectedComponent(void);

   //void DBSelectionChanged(const QString &arg1);
   void addComponentCheckChanged(int newState);
   void POPDBSelectionChanged(const QString &arg1);

   void addPopulationGroup(QMap<QString, QString> *PG_data_in=nullptr);
   void removePopulationGroup(QWidget *thePopulationGroup);

   void addComponentGroup(QMap<QString, QString> *CG_data_in=nullptr);
   void removeComponentGroup(QWidget *theComponentGroup);

   void onLoadConfigClicked(void);
   void onSaveConfigClicked(void);

   void chooseAdditionalComponentDB(void);
   void updateComponentVulnerabilityDB();
   void exportComponentVulnerabilityDB(void);

   void storiesOrAreaChanged();
   void setNumStories(int numStories, double dummy);
   void setPlanArea(double dummy, double dummy2, double planArea);
   void setPlanAreaUnit(QString unitName);

   /*
   void storeCompQuantity(void);
   void retrieveCompQuantity(void);

   void storeCompQuantityDist(void);
   void retrieveCompQuantityDist(void);

   void storeCompQuantityCOV(void);
   void retrieveCompQuantityCOV(void);

   void storeWeightD1(void);
   void retrieveWeightD1(void);
   void storeWeightD2(void);
   void retrieveWeightD2(void);

   void refreshCompOverview(void);
   */

private:

    void deleteCompDB();
    void deleteCompConfig();
    QString getDefaultDatabasePath();
    QString generateFragilityInfo(QString comp_DB_path);

    QString   cmpVulnerabilityDB;
    QString   additionalComponentDB;
    QString   cmpVulnerabilityDB_viz;
    QString   additionalComponentDB_viz;

    GeneralInformationWidget *theGI;

    ProgramOutputDialog *outputDialog;

    QGridLayout *gridLayout;

    QSignalMapper *smRemoveCG;
    QSignalMapper *smRemovePG;

    QLineEdit *storiesValue;
    QLineEdit *planAreaValue;
    QLabel    *lblUnitSquared;
    QLineEdit *leAdditionalComponentDB;
    QLineEdit *populationDataBasePath;

    QComboBox *occupancyType;
    QComboBox *databaseCombo;
    QComboBox *databasePOPCombo;

    QCheckBox *addComp;

    QPushButton *btnChooseAdditionalComponentDB;
    QPushButton *btnChoosePOPDB;

    QVBoxLayout *eventLayout;
    QVBoxLayout *loCGList;
    QVBoxLayout *loPGList;
    QVBoxLayout *loCQuantityRemove;
    QVBoxLayout *loPQuantityRemove;

    QComboBox *availableCompCombo;
    QStringListModel *availableCBModel;
    QComboBox *selectedCompCombo;
    QStringListModel *selectedCBModel;

    QLabel *compName;
    QLabel *compDescription;
    QLabel *compEDP;
    QLabel *compUnit;
    QLabel *compInfo;

    QLineEdit *compQuantity;
    QLabel *compUnitSize;
    QLabel *compUnitType;

    QComboBox *compQFamily;
    QLineEdit *compQDispersion;

    QLineEdit *dir1Weights;
    QLineEdit *dir2Weights;

    QGroupBox *compOverviewGroupBox;
    QVBoxLayout *compOverviewLayout;

    // component DL information
    QMap<QString, QMap<QString, QString>* > *compDB;

    // component properties
    QMap<QString, QVector<QMap<QString, QString>* >* > *compConfig;
    QVector<QMap<QString, QString>* > *popConfig;

    QString dbType = "N/A";

    /*
    QMap<QString, QVector<QString>> *compLocationMap;
    QMap<QString, QVector<QString>> *compDirectionMap;
    QMap<QString, QVector<QString>> *compCGMedianMap;
    QMap<QString, QVector<QString>> *compCGUnitMap;
    QMap<QString, QVector<QString>> *compCGDistributionMap;
    QMap<QString, QVector<QString>> *compCGCOVMap;
    */

    QVector<Component *>theComponents;
    QVector<ComponentGroup *>vComponentGroups;
    QVector<PopulationGroup *>vPopulationGroups;
    QVector<QPushButton *>vRemoveButtons;
    QVector<QPushButton *>vRemovePopButtons;

    // Display fragility information
    QWebEngineView *fragilityViz; 
    //QuaZip fragilityZip;

    SC_TableEdit *adamsTable;
};

#endif // COMPONENT_CONTAINER_H
