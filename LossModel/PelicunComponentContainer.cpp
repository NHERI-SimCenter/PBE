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

#include "PelicunComponentContainer.h"
#include "PelicunComponentGroup.h"
#include "PelicunPopulationGroup.h"
#include "SimCenterPreferences.h"
#include "WorkflowAppPBE.h"
#include "GeneralInformationWidget.h"

#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <SectionTitle.h>
#include <QLineEdit>
#include <QRadioButton>
#include <QFileDialog>
#include <QScrollArea>
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <QStringListModel>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QMap>
#include <QCoreApplication>
#include <QSettings>
#include <QSignalMapper>
#include <QWebEngineView>
#include <RunPythonInThread.h>

#include <QScreen>
#include <SC_TableEdit.h>

PelicunComponentContainer::PelicunComponentContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    int maxWidth = 800;
    this->setMaximumWidth(maxWidth);

    theGI = GeneralInformationWidget::getInstance();

    // initialize the compDB Map
    compDB = new QMap<QString, QMap<QString, QString>* >;

    // initialize component property containers
    compConfig = new QMap<QString, QVector<QMap<QString, QString>* >* >;

    // initialize the location of component vulnerability data
    cmpVulnerabilityDB = "";
    cmpVulnerabilityDB_viz = "";
    additionalComponentDB = "";
    additionalComponentDB_viz = "";

    gridLayout = new QGridLayout();

    // general information ----------------------------------------------------
    QGroupBox *generalGroupBox = new QGroupBox("General Information");
    generalGroupBox->setMaximumWidth(320);

    QVBoxLayout *generalFormLayout = new QVBoxLayout(generalGroupBox);
    //QFormLayout *generalFormLayout = new QFormLayout();

    /*
    // adams table
    QHBoxLayout *adamsTableLayout = new QHBoxLayout();
    QStringList headings; headings << "Something" << "Else";
    QStringList data; data << "Row1" << "1.0" << "Row2" << "2.0";
    QStringList adamsStuff; adamsStuff << "Adams Table" << "Add Below Current" << "Delete Current";
    adamsTable = new SC_TableEdit("adamsTable",headings, 2, data,&adamsStuff);
    adamsTableLayout->addWidget(new QLabel("ADAMS TABLE"));
    QPushButton *adamsTableButton = new QPushButton("PRESS TO SEE TABLE");
    adamsTableLayout->addWidget(adamsTableButton);
    connect(adamsTableButton, &QPushButton::clicked, this, [=](){
              adamsTable->setWindowFlag(Qt::Window);
              adamsTable->move(screen()->geometry().center() - frameGeometry().center());	      
	      adamsTable->show();
	    });
    generalFormLayout->addLayout(adamsTableLayout);
    */
    
    
    // stories
    QHBoxLayout *storyLayout = new QHBoxLayout();

    QLabel *lblNumStories = new QLabel();
    lblNumStories->setText("Number of Stories");
    lblNumStories->setMaximumWidth(110);
    lblNumStories->setMinimumWidth(110);
    lblNumStories->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    storyLayout->addWidget(lblNumStories);

    storiesValue = new QLineEdit();
    storiesValue->setToolTip(tr("Number of stories above ground."));
    storiesValue->setText("");
    storiesValue->setMaximumWidth(80);
    storiesValue->setMinimumWidth(80);
    storiesValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    //generalFormLayout->addRow(tr("Number of Stories"), storiesValue);
    storyLayout->addWidget(storiesValue);

    storyLayout->addStretch();

    generalFormLayout->addLayout(storyLayout);

    // plan area
    QHBoxLayout *areaLayout = new QHBoxLayout();

    QLabel *lblPlanArea = new QLabel();
    lblPlanArea->setText("Plan Area");
    lblPlanArea->setMaximumWidth(110);
    lblPlanArea->setMinimumWidth(110);
    lblPlanArea->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    areaLayout->addWidget(lblPlanArea);

    planAreaValue = new QLineEdit();
    planAreaValue->setToolTip(tr("Plan area of the building. \n"
                                 "Unit is based on length unit defined on General Information (GI) panel."));
    planAreaValue->setText("");
    planAreaValue->setMaximumWidth(80);
    planAreaValue->setMinimumWidth(80);
    planAreaValue->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    //generalFormLayout->addRow(tr("Plan Area"), planAreaValue, tr(theGI->getLengthUnit()+"²"));
    areaLayout->addWidget(planAreaValue);

    lblUnitSquared = new QLabel();
    lblUnitSquared->setText(QString(theGI->getLengthUnit())+QString("²"));
    lblUnitSquared->setMaximumWidth(40);
    lblUnitSquared->setMinimumWidth(40);
    lblUnitSquared->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    areaLayout->addWidget(lblUnitSquared);

    areaLayout->addStretch();

    generalFormLayout->addLayout(areaLayout);

    // add signal and slot connections with GI
    double plan, w, d;
    theGI->getBuildingDimensions(w, d, plan);
    planAreaValue->setText(QString::number(plan));
    storiesValue->setText(QString::number(theGI->getNumFloors()));

    connect(storiesValue, SIGNAL(editingFinished()),this,SLOT(storiesOrAreaChanged()));
    connect(planAreaValue, SIGNAL(editingFinished()),this,SLOT(storiesOrAreaChanged()));

    connect(theGI,SIGNAL(numStoriesOrHeightChanged(int, double)),
            this, SLOT(setNumStories(int, double)));
    connect(theGI,SIGNAL(buildingDimensionsChanged(double,double,double)),
            this,SLOT(setPlanArea(double,double,double)));
    connect(theGI,SIGNAL(unitLengthChanged(QString)),
            this,SLOT(setPlanAreaUnit(QString)));

    // occupancy
    QHBoxLayout *occupancyLayout = new QHBoxLayout();

    QLabel *lblOccupancy = new QLabel();
    lblOccupancy->setText("Occupancy Type");
    lblOccupancy->setMaximumWidth(110);
    lblOccupancy->setMinimumWidth(110);
    lblOccupancy->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    occupancyLayout->addWidget(lblOccupancy);

    occupancyType = new QComboBox();
    occupancyType->setToolTip(tr("The type of occupancy defines the function of the building.\n"
                                 "Occupancy classes from Hazus MH2.1 and FEMA P-58 are available."));
    occupancyType->addItem("RES1",0);
    occupancyType->addItem("RES2",1);
    occupancyType->addItem("RES3",2);
    occupancyType->addItem("RES4",3);
    occupancyType->addItem("RES5",4);
    occupancyType->addItem("RES6",5);
    occupancyType->addItem("COM1",6);
    occupancyType->addItem("COM2",7);
    occupancyType->addItem("COM3",8);
    occupancyType->addItem("COM4",9);
    occupancyType->addItem("COM5",10);
    occupancyType->addItem("COM6",11);
    occupancyType->addItem("COM7",12);
    occupancyType->addItem("COM8",13);
    occupancyType->addItem("COM9",14);
    occupancyType->addItem("COM10",15);
    occupancyType->addItem("IND1",16);
    occupancyType->addItem("IND2",17);
    occupancyType->addItem("IND3",18);
    occupancyType->addItem("IND4",19);
    occupancyType->addItem("IND5",20);
    occupancyType->addItem("IND6",21);
    occupancyType->addItem("AGR1",22);
    occupancyType->addItem("REL1",23);
    occupancyType->addItem("GOV1",24);
    occupancyType->addItem("GOV2",25);
    occupancyType->addItem("EDU1",26);
    occupancyType->addItem("EDU2",27);

    occupancyType->insertSeparator(28);

    occupancyType->addItem("Commercial Office"     ,29);
    occupancyType->addItem("Elementary School"     ,30);
    occupancyType->addItem("Middle School"         ,31);
    occupancyType->addItem("High School"           ,32);
    occupancyType->addItem("Healthcare"            ,33);
    occupancyType->addItem("Hospitality"           ,34);
    occupancyType->addItem("Multi-Unit Residential",35);
    occupancyType->addItem("Research Laboratories" ,36);
    occupancyType->addItem("Retail"                ,37);
    occupancyType->addItem("Warehouse"             ,38);


    occupancyType->setItemData( 0, "Hazus - Residential - Single Family Dwelling", Qt::ToolTipRole);
    occupancyType->setItemData( 1, "Hazus - Residential - Mobile Home", Qt::ToolTipRole);
    occupancyType->setItemData( 2, "Hazus - Residential - Multi Family Dwelling", Qt::ToolTipRole);
    occupancyType->setItemData( 3, "Hazus - Residential - Temporary Lodging", Qt::ToolTipRole);
    occupancyType->setItemData( 4, "Hazus - Residential - Institutional Dormitory", Qt::ToolTipRole);
    occupancyType->setItemData( 5, "Hazus - Residential - Nursing Home", Qt::ToolTipRole);
    occupancyType->setItemData( 6, "Hazus - Commercial - Retail Trade", Qt::ToolTipRole);
    occupancyType->setItemData( 7, "Hazus - Commercial - Wholesale Trade", Qt::ToolTipRole);
    occupancyType->setItemData( 8, "Hazus - Commercial - Personal and Repair Services", Qt::ToolTipRole);
    occupancyType->setItemData( 9, "Hazus - Commercial - Professional/Technical Services", Qt::ToolTipRole);
    occupancyType->setItemData(10, "Hazus - Commercial - Banks", Qt::ToolTipRole);
    occupancyType->setItemData(11, "Hazus - Commercial - Hospital", Qt::ToolTipRole);
    occupancyType->setItemData(12, "Hazus - Commercial - Medical Office/Clinic", Qt::ToolTipRole);
    occupancyType->setItemData(13, "Hazus - Commercial - Entertainment & Recreation", Qt::ToolTipRole);
    occupancyType->setItemData(14, "Hazus - Commercial - Theaters", Qt::ToolTipRole);
    occupancyType->setItemData(15, "Hazus - Commercial - Parking", Qt::ToolTipRole);
    occupancyType->setItemData(16, "Hazus - Industry - Heavy ", Qt::ToolTipRole);
    occupancyType->setItemData(17, "Hazus - Industry - Light ", Qt::ToolTipRole);
    occupancyType->setItemData(18, "Hazus - Industry - Food/Drugs/Chemical ", Qt::ToolTipRole);
    occupancyType->setItemData(19, "Hazus - Industry - Metals/Minerals Processing ", Qt::ToolTipRole);
    occupancyType->setItemData(20, "Hazus - Industry - High Technology ", Qt::ToolTipRole);
    occupancyType->setItemData(21, "Hazus - Industry - Construction ", Qt::ToolTipRole);
    occupancyType->setItemData(22, "Hazus - Agriculture", Qt::ToolTipRole);
    occupancyType->setItemData(23, "Hazus - Church/Non-profit", Qt::ToolTipRole);
    occupancyType->setItemData(24, "Hazus - Government - General Services", Qt::ToolTipRole);
    occupancyType->setItemData(25, "Hazus - Government - Emergency Response", Qt::ToolTipRole);
    occupancyType->setItemData(26, "Hazus - Education - Grade Schools", Qt::ToolTipRole);
    occupancyType->setItemData(27, "Hazus - Education - Colleges/Universities", Qt::ToolTipRole);

    occupancyType->setItemData(29, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(30, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(31, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(32, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(33, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(34, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(35, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(36, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(37, "FEMA P-58", Qt::ToolTipRole);
    occupancyType->setItemData(38, "FEMA P-58", Qt::ToolTipRole);

    occupancyType->setCurrentIndex(0);
    //generalFormLayout->addRow(tr("Occupancy Type"), occupancyType);
    occupancyLayout->addWidget(occupancyType);

    occupancyLayout->addStretch();

    generalFormLayout->addLayout(occupancyLayout);

    generalFormLayout->addStretch();

    generalGroupBox->setLayout(generalFormLayout);

    // Databases ----------------------------------------------------

    QGroupBox *DataBasesGroupBox = new QGroupBox("Component Vulnerability");
    //DataBasesGroupBox->setMaximumWidth(maxWidth);
    //QFormLayout *compListFormLayout = new QFormLayout(DataBasesGroupBox);

    QVBoxLayout *loCEns = new QVBoxLayout(DataBasesGroupBox);

    // component data folder
    QHBoxLayout *selectDBLayout = new QHBoxLayout();

    QLabel *lblDBcomp = new QLabel();
    lblDBcomp->setText("Database:");
    //lblDBcomp->setMaximumWidth(100);
    //lblDBcomp->setMinimumWidth(100);
    lblDBcomp->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    selectDBLayout->addWidget(lblDBcomp);

    // database selection
    databaseCombo = new QComboBox();
    databaseCombo->setToolTip(tr("This database provides parameters for component vulnerability models to simulate damages."));
    databaseCombo->addItem("FEMA P-58",0);
    databaseCombo->addItem("Hazus Earthquake - Buildings",1);
    databaseCombo->addItem("Hazus Earthquake - Transportation",2);
    databaseCombo->addItem("None",3);

    databaseCombo->setItemData(0, "Based on the 2nd edition of FEMA P-58", Qt::ToolTipRole);
    databaseCombo->setItemData(1, "Based on the Hazus MH Earthquake Technical Manual v5.1", Qt::ToolTipRole);
    databaseCombo->setItemData(2, "Based on the Hazus MH Earthquake Technical Manual v5.1", Qt::ToolTipRole);
    databaseCombo->setItemData(3, "None of the built-in databases will be used", Qt::ToolTipRole);

    connect(databaseCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateComponentVulnerabilityDB()));

    selectDBLayout->addWidget(databaseCombo, 1);

    // export db
    QPushButton *btnExportDataBase = new QPushButton();
    btnExportDataBase->setMinimumWidth(100);
    btnExportDataBase->setMaximumWidth(100);
    btnExportDataBase->setText(tr("Export DB"));
    connect(btnExportDataBase, SIGNAL(clicked()),this,SLOT(exportComponentVulnerabilityDB()));
    selectDBLayout->addWidget(btnExportDataBase);

    loCEns->addLayout(selectDBLayout);

    //QSpacerItem *spacerGroups1 = new QSpacerItem(5,10);
    //loCEns->addItem(spacerGroups1);

    // additional component checkbox

    QHBoxLayout *addCompLayout = new QHBoxLayout();

    addComp = new QCheckBox();
    addComp->setText("");
    QString addCompTT = QString("Complement or replace parts of the database with additional components.");
    addComp->setToolTip(addCompTT);
    addComp->setChecked(false);
    addComp->setTristate(false);
    addCompLayout->addWidget(addComp);

    QLabel *lblCompCheck = new QLabel();
    lblCompCheck->setText("Use Additional Components");
    lblCompCheck->setToolTip(addCompTT);
    addCompLayout->addWidget(lblCompCheck);

    addCompLayout->addStretch();

    loCEns->addLayout(addCompLayout);

    // database path
    QHBoxLayout *customFolderLayout = new QHBoxLayout();

    leAdditionalComponentDB = new QLineEdit;
    leAdditionalComponentDB->setToolTip(tr("Location of the user-defined component vulnerability data."));
    customFolderLayout->addWidget(leAdditionalComponentDB, 1);
    leAdditionalComponentDB->setVisible(false);
    leAdditionalComponentDB->setReadOnly(true);

    btnChooseAdditionalComponentDB = new QPushButton();
    btnChooseAdditionalComponentDB->setMinimumWidth(100);
    btnChooseAdditionalComponentDB->setMaximumWidth(100);
    btnChooseAdditionalComponentDB->setText(tr("Choose"));
    connect(btnChooseAdditionalComponentDB, SIGNAL(clicked()),this,SLOT(chooseAdditionalComponentDB()));
    //connect(leAdditionalComponentDB, SIGNAL(textChanged(QString)),this,SLOT(updateAvailableComponents()));
    customFolderLayout->addWidget(btnChooseAdditionalComponentDB);
    btnChooseAdditionalComponentDB->setVisible(false);

    connect(addComp, SIGNAL(stateChanged(int)), this, SLOT(addComponentCheckChanged(int)));
    //this->addComponentCheckChanged(addComp->checkState());

    //connect(databaseCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(DBSelectionChanged(QString)));

    loCEns->addLayout(customFolderLayout);

    // add line
    /*
    QFrame *lineDS = new QFrame();
    lineDS->setFrameShape(QFrame::HLine);
    lineDS->setFrameShadow(QFrame::Sunken);
    loCEns->addWidget(lineDS);
    */

    /*
    // population variation data folder
    QHBoxLayout *selectDB_POPLayout = new QHBoxLayout();

    QLabel *lblDBpop = new QLabel();
    lblDBpop->setText("Population Variation:");
    lblDBpop->setMaximumWidth(150);
    lblDBpop->setMinimumWidth(150);
    selectDB_POPLayout->addWidget(lblDBpop);

    // database selection
    databasePOPCombo = new QComboBox();
    databasePOPCombo->setToolTip(tr("This database defines the variation in population over time (e.g., time of day, day of week, month of year) as a function of occupancy type."));
    databasePOPCombo->addItem("FEMA P-58",0);
    databasePOPCombo->addItem("User Defined",1);
    databasePOPCombo->addItem("None",2);

    databasePOPCombo->setItemData(0, "Based on the 2nd edition of FEMA P-58", Qt::ToolTipRole);
    databasePOPCombo->setItemData(1, "Custom database provided by the user", Qt::ToolTipRole);
    databasePOPCombo->setItemData(2, "No variation. The peak population is used for all realizations", Qt::ToolTipRole);

    //connect(databasePOPCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAvailableComponents()));

    selectDB_POPLayout->addWidget(databasePOPCombo, 1);

    QPushButton *btnExportPOPDataBase = new QPushButton();
    btnExportPOPDataBase->setMinimumWidth(100);
    btnExportPOPDataBase->setMaximumWidth(100);
    btnExportPOPDataBase->setText(tr("Export DB"));
    //connect(btnExportPOPDataBase, SIGNAL(clicked()),this,SLOT(exportComponentVulnerabilityDB()));
    selectDB_POPLayout->addWidget(btnExportPOPDataBase);

    loCEns->addLayout(selectDB_POPLayout);

    // database path
    QHBoxLayout *customPOPFolderLayout = new QHBoxLayout();

    populationDataBasePath = new QLineEdit;
    populationDataBasePath->setToolTip(tr("Location of the user-defined population variation data."));
    customPOPFolderLayout->addWidget(populationDataBasePath, 1);
    populationDataBasePath->setVisible(false);
    populationDataBasePath->setEnabled(false);

    btnChoosePOPDB = new QPushButton();
    btnChoosePOPDB->setMinimumWidth(100);
    btnChoosePOPDB->setMaximumWidth(100);
    btnChoosePOPDB->setText(tr("Choose"));
    //connect(btnChoosePOPDB, SIGNAL(clicked()),this,SLOT(chooseAdditionalComponentDB()));
    //connect(populationDataBasePath, SIGNAL(textChanged(QString)),this,SLOT(updateAvailableComponents()));
    customPOPFolderLayout->addWidget(btnChoosePOPDB);
    btnChoosePOPDB->setVisible(false);

    connect(databasePOPCombo, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(POPDBSelectionChanged(QString)));

    loCEns->addLayout(customPOPFolderLayout);
    */

    loCEns->addStretch();

    /*
    // population details  ----------------------------------------------------------

    QGroupBox *popDetailsGroupBox = new QGroupBox("Population Assignment");

    QVBoxLayout *loPDetails = new QVBoxLayout(popDetailsGroupBox);

    // load/save population model
    QHBoxLayout *loadsavePLayout = new QHBoxLayout();

    QPushButton* loadPOPConfig = new QPushButton();
    loadPOPConfig->setMinimumWidth(75);
    loadPOPConfig->setMaximumWidth(75);
    loadPOPConfig->setText(tr("Load"));
    loadPOPConfig->setToolTip(tr("Load population assignment from a CSV file."));
    //connect(loadPOPConfig,SIGNAL(clicked()),this,SLOT(onLoadPOPConfigClicked()));
    loadsavePLayout->addWidget(loadPOPConfig);

    QPushButton* savePOPConfig = new QPushButton();
    savePOPConfig->setMinimumWidth(75);
    savePOPConfig->setMaximumWidth(75);
    savePOPConfig->setText(tr("Save"));
    savePOPConfig->setToolTip(tr("Save population assignment to a CSV file."));
    //connect(savePOPConfig,SIGNAL(clicked()),this,SLOT(onSavePOPConfigClicked()));
    loadsavePLayout->addWidget(savePOPConfig);

    loadsavePLayout->addStretch(1);

    loPDetails->addLayout(loadsavePLayout);

    // add line
    QFrame *linePOPA = new QFrame();
    linePOPA->setFrameShape(QFrame::HLine);
    linePOPA->setFrameShadow(QFrame::Sunken);
    loPDetails->addWidget(linePOPA);

    // Population Group header
    QHBoxLayout *loPGroup_header = new QHBoxLayout();

    QPushButton *addPGroup = new QPushButton();
    addPGroup->setMinimumWidth(20);
    addPGroup->setMaximumWidth(20);
    addPGroup->setMaximumHeight(20);
    addPGroup->setText(tr("+"));
    loPGroup_header->addWidget(addPGroup);
    connect(addPGroup, SIGNAL(pressed()), this, SLOT(addPopulationGroup()));

    QLabel *lblPOP_loc = new QLabel();
    lblPOP_loc->setText(" Location(s)");
    lblPOP_loc->setMaximumWidth(75);
    lblPOP_loc->setMinimumWidth(75);
    loPGroup_header->addWidget(lblPOP_loc);

    QLabel *lblPOP_mu = new QLabel();
    lblPOP_mu->setText(" Quantity");
    lblPOP_mu->setMaximumWidth(75);
    lblPOP_mu->setMinimumWidth(75);
    loPGroup_header->addWidget(lblPOP_mu);

    QLabel *lblPOP_dist = new QLabel();
    lblPOP_dist->setText("Distribution");
    lblPOP_dist->setMaximumWidth(100);
    lblPOP_dist->setMinimumWidth(100);
    loPGroup_header->addWidget(lblPOP_dist);

    QLabel *lblPOP_disp = new QLabel();
    lblPOP_disp->setText(" Dispersion");
    lblPOP_disp->setMaximumWidth(45);
    lblPOP_disp->setMinimumWidth(45);
    loPGroup_header->addWidget(lblPOP_disp);

    QLabel *lblPOP_note = new QLabel();
    lblPOP_note->setText(" Comment");
    loPGroup_header->addWidget(lblPOP_note, 1);

    loPGroup_header->addStretch();
    loPGroup_header->setSpacing(10);
    loPGroup_header->setContentsMargins(0,0,0,0);;

    loPDetails->addLayout(loPGroup_header);

    // Population Group list
    QHBoxLayout *loPQuantityLayout = new QHBoxLayout();

    loPQuantityRemove = new QVBoxLayout();

    loPQuantityRemove->addStretch();

    smRemovePG = new QSignalMapper(this);
    connect(smRemovePG, SIGNAL(mapped(QWidget*)), this, SLOT(removePopulationGroup(QWidget*)));

    loPQuantityLayout->addLayout(loPQuantityRemove);

    QScrollArea *POPQuantityList = new QScrollArea;
    POPQuantityList->setWidgetResizable(true);
    POPQuantityList->setLineWidth(0);
    POPQuantityList->setFrameShape(QFrame::NoFrame);

    QWidget *PGWidget = new QWidget;
    loPGList = new QVBoxLayout();
    loPGList->addStretch();
    loPGList->setContentsMargins(0,0,0,0);;
    PGWidget->setLayout(loPGList);
    POPQuantityList->setWidget(PGWidget);

    loPQuantityLayout->addWidget(POPQuantityList, 1);

    loPDetails->addLayout(loPQuantityLayout, 1);
    */

    // component details  ----------------------------------------------------------

    QGroupBox *compDetailsGroupBox = new QGroupBox("Component Assignment");
    //compDetailsGroupBox->setMaximumWidth(maxWidth);

    QVBoxLayout *loCDetails = new QVBoxLayout(compDetailsGroupBox);
    //QFormLayout *compDetailsFormLayout = new QFormLayout(compDetailsGroupBox);

    // load/save component model
    QHBoxLayout *loadsaveCLayout = new QHBoxLayout();

    QPushButton* loadConfig = new QPushButton();
    loadConfig->setMinimumWidth(75);
    loadConfig->setMaximumWidth(75);
    loadConfig->setText(tr("Load"));
    loadConfig->setToolTip(tr("Load component assignment from a CSV file."));
    connect(loadConfig,SIGNAL(clicked()),this,SLOT(onLoadConfigClicked()));
    loadsaveCLayout->addWidget(loadConfig);

    QPushButton* saveConfig = new QPushButton();
    saveConfig->setMinimumWidth(75);
    saveConfig->setMaximumWidth(75);
    saveConfig->setText(tr("Save"));
    saveConfig->setToolTip(tr("Save component assignment to a CSV file."));
    connect(saveConfig,SIGNAL(clicked()),this,SLOT(onSaveConfigClicked()));
    loadsaveCLayout->addWidget(saveConfig);

    loadsaveCLayout->addStretch(1);

    loCDetails->addLayout(loadsaveCLayout);

    // add line
    QFrame *lineCMPA = new QFrame();
    lineCMPA->setFrameShape(QFrame::HLine);
    lineCMPA->setFrameShadow(QFrame::Sunken);
    loCDetails->addWidget(lineCMPA);

    // available components & load/save assignment
    QHBoxLayout *availableCLayout = new QHBoxLayout();

    QLabel *lblAvailableComp = new QLabel();
    lblAvailableComp->setText("Available in DB:");
    lblAvailableComp->setMaximumWidth(110);
    lblAvailableComp->setMinimumWidth(110);

    availableCLayout->addWidget(lblAvailableComp);

    availableCompCombo = new QComboBox();
    availableCompCombo->setMinimumWidth(150);
    availableCBModel = new QStringListModel();
    availableCompCombo->setModel(availableCBModel);
    availableCompCombo->setToolTip(tr("List of available components. \n"
                                      "Click on the buttons to add the component to the ensemble."));

    availableCLayout->addWidget(availableCompCombo);

    QPushButton *addComponent = new QPushButton();
    addComponent->setMinimumWidth(100);
    addComponent->setMaximumWidth(100);
    addComponent->setText(tr("Add"));
    connect(addComponent,SIGNAL(clicked()),this,SLOT(addOneComponent()));
    availableCLayout->addWidget(addComponent);

    QPushButton *addAllComponents = new QPushButton();
    addAllComponents->setMinimumWidth(100);
    addAllComponents->setMaximumWidth(100);
    addAllComponents->setText(tr("Add All"));
    connect(addAllComponents,SIGNAL(clicked()),this,SLOT(addAllComponents()));
    availableCLayout->addWidget(addAllComponents);

    availableCLayout->addStretch();

    loCDetails->addLayout(availableCLayout);
    //compListFormLayout->addRow(tr("Available Components: "),availableCLayout);

    QSpacerItem *spacerGroups3 = new QSpacerItem(10,10);
    loCDetails->addItem(spacerGroups3);

    // selected components
    QHBoxLayout *selectedCLayout = new QHBoxLayout();

    QLabel *lblSelectedComp = new QLabel();
    lblSelectedComp->setText("Assigned:");
    lblSelectedComp->setMaximumWidth(110);
    lblSelectedComp->setMinimumWidth(110);
    selectedCLayout->addWidget(lblSelectedComp);

    selectedCompCombo = new QComboBox();
    selectedCompCombo->setMinimumWidth(150);
    selectedCBModel = new QStringListModel();
    selectedCompCombo->setModel(selectedCBModel);
    selectedCompCombo->setToolTip(tr("List of selected components. \n"
                                     "Click on the buttons to remove the component from the ensemble."));
    connect(selectedCompCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(showSelectedComponent()));
    selectedCLayout->addWidget(selectedCompCombo);

    QPushButton *removeComponent = new QPushButton();
    removeComponent->setMinimumWidth(100);
    removeComponent->setMaximumWidth(100);
    removeComponent->setText(tr("Remove"));
    connect(removeComponent,SIGNAL(clicked()),this,SLOT(removeOneComponent()));
    selectedCLayout->addWidget(removeComponent);

    QPushButton *removeAllComponents = new QPushButton();
    removeAllComponents->setMinimumWidth(100);
    removeAllComponents->setMaximumWidth(100);
    removeAllComponents->setText(tr("Remove All"));
    connect(removeAllComponents,SIGNAL(clicked()),this, SLOT(removeAllComponents()));
    selectedCLayout->addWidget(removeAllComponents);

    selectedCLayout->addStretch();

    loCDetails->addLayout(selectedCLayout);
    //compListFormLayout->addRow(tr("Selected Components: "), selectedCLayout);

    int CD_lbl_width = 110;

    /*
    // name
    QHBoxLayout *loCDName = new QHBoxLayout();

    QLabel *lblCompName = new QLabel();
    lblCompName->setText("Name:");
    lblCompName->setMaximumWidth(CD_lbl_width);
    lblCompName->setMinimumWidth(CD_lbl_width);
    lblCompName->setAlignment(Qt::AlignTop);
    loCDName->addWidget(lblCompName);

    compName = new QLabel();
    compName->setWordWrap(true);
    compName->setText("");
    loCDName->addWidget(compName, 1);

    //loCDName->addStretch();
    loCDetails->addLayout(loCDName);
    //compDetailsFormLayout->addRow(tr("Name: "), compName);
    */

    // fragility info

    fragilityViz = new QWebEngineView();
    fragilityViz->setMinimumHeight(240);
    fragilityViz->setMaximumHeight(240);
    fragilityViz->page()->setBackgroundColor(Qt::transparent);
    //fragilityViz->page()->setBackgroundColor(Qt::red);
    fragilityViz->setHtml("");
    fragilityViz->setVisible(false);
    // sy - **NOTE** QWebEngineView display is VERY SLOW when the app is built in debug mode 
    // Max size of figure is limited to 2MB
    loCDetails->addWidget(fragilityViz, 0);

    // description
    QHBoxLayout *loCDDesc = new QHBoxLayout();

    QLabel *lblCompDesc = new QLabel();
    lblCompDesc->setText("Description:");
    lblCompDesc->setMaximumWidth(CD_lbl_width);
    lblCompDesc->setMinimumWidth(CD_lbl_width);
    lblCompDesc->setAlignment(Qt::AlignTop);
    loCDDesc->addWidget(lblCompDesc);

    compDescription = new QLabel();
    compDescription->setWordWrap(true);
    compDescription->setText("");
    loCDDesc->addWidget(compDescription, 1);

    //loCDDesc->addStretch();
    loCDetails->addLayout(loCDDesc);
    //compDetailsFormLayout->addRow(tr("Description: "), compDescription);

    // EDP type
    QHBoxLayout *loCDEDP = new QHBoxLayout();

    QLabel *lblEDP_Type = new QLabel();
    lblEDP_Type->setText("Demand type:");
    lblEDP_Type->setMaximumWidth(CD_lbl_width);
    lblEDP_Type->setMinimumWidth(CD_lbl_width);
    loCDEDP->addWidget(lblEDP_Type);

    compEDP = new QLabel();
    compEDP->setWordWrap(true);
    compEDP->setText("");
    loCDEDP->addWidget(compEDP, 1);

    //loCDEDP->addStretch();
    loCDetails->addLayout(loCDEDP);
    //compDetailsFormLayout->addRow(tr("EDP type: "), compEDP);

    // Standard unit
    QHBoxLayout *loCDUnit = new QHBoxLayout();

    QLabel *lblStdUnit = new QLabel();
    lblStdUnit->setText("Block size:");
    lblStdUnit->setMaximumWidth(CD_lbl_width);
    lblStdUnit->setMinimumWidth(CD_lbl_width);
    loCDUnit->addWidget(lblStdUnit);

    compUnit = new QLabel();
    compUnit->setWordWrap(true);
    compUnit->setText("");
    loCDUnit->addWidget(compUnit, 1);

    //loCDUnit->addStretch();
    loCDetails->addLayout(loCDUnit);

    // Additional info
    QHBoxLayout *loCDInfo = new QHBoxLayout();

    QLabel *lblInfo = new QLabel();
    lblInfo->setText("Additional info:");
    lblInfo->setMaximumWidth(CD_lbl_width);
    lblInfo->setMinimumWidth(CD_lbl_width);
    loCDInfo->addWidget(lblInfo);

    compInfo = new QLabel();
    compInfo->setWordWrap(true);
    compInfo->setText("");
    loCDInfo->addWidget(compInfo, 1);

    loCDetails->addLayout(loCDInfo);

    // Quantity title & buttons
    QHBoxLayout *loCQuantity_title = new QHBoxLayout();

    /*
    QPushButton *removeCGroup = new QPushButton();
    removeCGroup->setMinimumWidth(150);
    removeCGroup->setMaximumWidth(150);
    removeCGroup->setText(tr("Remove Components"));
    loCQuantity_title->addWidget(removeCGroup);
    connect(removeCGroup, SIGNAL(pressed()), this, SLOT(removeComponentGroup()));
    */

    loCQuantity_title->addStretch();

    loCDetails->addLayout(loCQuantity_title);

    // Component Group header
    QHBoxLayout *loCGroup_header = new QHBoxLayout();

    QPushButton *addCGroup = new QPushButton();
    addCGroup->setMinimumWidth(20);
    addCGroup->setMaximumWidth(20);
    addCGroup->setMaximumHeight(20);
    addCGroup->setText(tr("+"));
    loCGroup_header->addWidget(addCGroup);
    connect(addCGroup, SIGNAL(pressed()), this, SLOT(addComponentGroup()));

    QLabel *lblQNT_unit = new QLabel();
    lblQNT_unit->setText(" Unit");
    lblQNT_unit->setMaximumWidth(45);
    lblQNT_unit->setMinimumWidth(45);
    loCGroup_header->addWidget(lblQNT_unit);

    QLabel *lblQNT_loc = new QLabel();
    lblQNT_loc->setText(" Location(s)");
    lblQNT_loc->setMaximumWidth(75);
    lblQNT_loc->setMinimumWidth(75);
    loCGroup_header->addWidget(lblQNT_loc);

    QLabel *lblQNT_dir = new QLabel();
    lblQNT_dir->setText(" Direction(s)");
    lblQNT_dir->setMaximumWidth(75);
    lblQNT_dir->setMinimumWidth(75);
    loCGroup_header->addWidget(lblQNT_dir);

    QLabel *lblQNT_mu = new QLabel();
    lblQNT_mu->setText(" Quantity");
    lblQNT_mu->setMaximumWidth(75);
    lblQNT_mu->setMinimumWidth(75);
    loCGroup_header->addWidget(lblQNT_mu);

    QLabel *lblQNT_block = new QLabel();
    lblQNT_block->setText(" Blocks");
    lblQNT_block->setMaximumWidth(75);
    lblQNT_block->setMinimumWidth(75);
    loCGroup_header->addWidget(lblQNT_block);

    QLabel *lblQNT_dist = new QLabel();
    lblQNT_dist->setText("Distribution");
    lblQNT_dist->setMaximumWidth(100);
    lblQNT_dist->setMinimumWidth(100);
    loCGroup_header->addWidget(lblQNT_dist);

    QLabel *lblQNT_disp = new QLabel();
    lblQNT_disp->setText(" Disp.");
    lblQNT_disp->setMaximumWidth(45);
    lblQNT_disp->setMinimumWidth(45);
    loCGroup_header->addWidget(lblQNT_disp);

    QLabel *lblQNT_note = new QLabel();
    lblQNT_note->setText(" Comment");
    loCGroup_header->addWidget(lblQNT_note, 1);

    loCGroup_header->addStretch();
    loCGroup_header->setSpacing(10);
    loCGroup_header->setContentsMargins(0,0,0,0);;

    loCDetails->addLayout(loCGroup_header);

    // Component Group list
    QHBoxLayout *loCQuantityLayout = new QHBoxLayout();

    loCQuantityRemove = new QVBoxLayout();
    loCQuantityRemove->setAlignment(Qt::AlignTop);

    loCQuantityRemove->addSpacing(2);
    loCQuantityRemove->setSpacing(5);
    loCQuantityRemove->setContentsMargins(0,0,0,0);;

    smRemoveCG = new QSignalMapper(this);
    connect(smRemoveCG, SIGNAL(mapped(QWidget*)), this,
            SLOT(removeComponentGroup(QWidget*)));

    loCQuantityLayout->addLayout(loCQuantityRemove);

    QScrollArea *saQuantityList = new QScrollArea;
    saQuantityList->setWidgetResizable(true);
    saQuantityList->setLineWidth(0);
    saQuantityList->setFrameShape(QFrame::NoFrame);

    QWidget *CGWidget = new QWidget;
    loCGList = new QVBoxLayout();
    loCGList->setAlignment(Qt::AlignTop);
    loCGList->setSpacing(5);    
    loCGList->setContentsMargins(0,0,0,0);;
    CGWidget->setLayout(loCGList);
    saQuantityList->setWidget(CGWidget);

    loCQuantityLayout->addWidget(saQuantityList, 1);

    loCDetails->addLayout(loCQuantityLayout, 1);

    // ----------------------------------------------------

    gridLayout->addWidget(generalGroupBox,0,0);
    gridLayout->addWidget(DataBasesGroupBox,0,1);
    //gridLayout->addWidget(popDetailsGroupBox, 1, 0, 1, 2);
    gridLayout->addWidget(compDetailsGroupBox, 2, 0, 1, 2);
    gridLayout->setRowStretch(2, 1);

    this->setLayout(gridLayout);

    this-> updateComponentVulnerabilityDB();
}

QString
PelicunComponentContainer::getDefaultDatabasePath()
{
    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    QString python = preferences->getPython();
    QString workDir = preferences->getLocalWorkDir();
    QString appDir = preferences->getAppDir();

    QProcess proc;
    QStringList params;

    params << appDir + "/applications/performDL/pelicun3/DL_visuals.py" << "query" << "default_db";

    proc.start(python, params);
    proc.waitForFinished(-1);

    QByteArray stdOut = proc.readAllStandardOutput();

    //this->statusMessage(stdOut);
    this->errorMessage(proc.readAllStandardError());

    QString databasePath(stdOut);

    return databasePath.trimmed();
}

QString
PelicunComponentContainer::generateFragilityInfo(QString comp_DB_path)
{
    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    QString python = preferences->getPython();
    QString workDir = preferences->getLocalWorkDir();
    QString appDir = preferences->getAppDir();

    QString comp_DB_name = comp_DB_path.mid(comp_DB_path.lastIndexOf("/"));
    comp_DB_name.chop(4);

    QString output_path = workDir + "/resources/fragility_viz/" + comp_DB_name + "/";

    //this->statusMessage(python);
    //this->statusMessage(workDir);
    //this->statusMessage(output_path);

    QString vizScript = appDir + QDir::separator() + "applications" + QDir::separator()
    + "performDL" + QDir::separator() + "pelicun3" + QDir::separator() + "DL_visuals.py";

    QStringList args; 
    args << QString("fragility") << QString(comp_DB_path)
         << QString("--output_path") << QString(output_path);

    RunPythonInThread *vizThread = new RunPythonInThread(vizScript, args, workDir);
    //connect(vizThread, &RunPythonInThread::processFinished, this, &PelicunComponentContainer::vizFilesCreated);
    vizThread->runProcess();

    //QProcess proc;
    //QStringList params;

    //params << appDir + "/applications/performDL/pelicun3/" + "DL_visuals.py" << "fragility" << comp_DB_path << "--output_path" << output_path;

    //proc.start(python, params);
    //proc.waitForFinished(-1);

    //this->statusMessage(proc.readAllStandardOutput());
    //this->errorMessage(proc.readAllStandardError());

    return output_path;
}

/*
void
PelicunComponentContainer::DBSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        leAdditionalComponentDB->setVisible(true);
        btnChooseAdditionalComponentDB->setVisible(true);
    } else {
        leAdditionalComponentDB->setVisible(false);
        btnChooseAdditionalComponentDB->setVisible(false);
    }
}
*/

void
PelicunComponentContainer::addComponentCheckChanged(int newState)
{
    if (newState == 2) {
        leAdditionalComponentDB->setVisible(true);
        btnChooseAdditionalComponentDB->setVisible(true);
    } else {
        leAdditionalComponentDB->setVisible(false);
        btnChooseAdditionalComponentDB->setVisible(false);
    }

    updateComponentVulnerabilityDB();
}

void
PelicunComponentContainer::POPDBSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        populationDataBasePath->setVisible(true);
        btnChoosePOPDB->setVisible(true);
    } else {
        populationDataBasePath->setVisible(false);
        btnChoosePOPDB->setVisible(false);
    }
}

void
PelicunComponentContainer::clearCompGroupWidget(){

    int CGcount = vComponentGroups.size();
    for (int i = CGcount-1; i >= 0; i--) {
      ComponentGroup *theComponentGroup = vComponentGroups.at(i);
      //remove the widget from the UI
      theComponentGroup->close();
      loCGList->removeWidget(theComponentGroup);
      //remove the CG from the UI vector
      vComponentGroups.remove(i);
      //delete the CG UI object
      theComponentGroup->setParent(nullptr);
      delete theComponentGroup;

      // get the corresponding remove button
      QPushButton *theRemoveButton = vRemoveButtons.at(i);
      // remove the button from the UI
      theRemoveButton->close();
      loCQuantityRemove->removeWidget(theRemoveButton);
      // remove the button from the UI vector
      vRemoveButtons.remove(i);
      // delete the button object
      theRemoveButton->setParent(nullptr);
      delete theRemoveButton;
    }
}

void
PelicunComponentContainer::retrieveCompGroups(){

    if (compConfig->contains(selectedCompCombo->currentText())) {
        // get the vector of CG data from the compConfig dict
        QVector<QMap<QString, QString>* > *vCG_data =
                compConfig->value(selectedCompCombo->currentText(),
                                  nullptr);
                                  //new QVector<QMap<QString, QString>* >);

        if (vCG_data != nullptr) {
            // create the CG UI elements for the existing data
            for (int i=0; i<vCG_data->count(); i++){

                addComponentGroup(vCG_data->at(i));
            }
        }
    }
}

void
PelicunComponentContainer::showSelectedComponent(){

    if ((selectedCompCombo->count() > 0) && 
        (selectedCompCombo->currentText() != "") && 
        (compDB->contains(selectedCompCombo->currentText()))){

        // Start with the metadata

        QString compID = selectedCompCombo->currentText();

        QMap<QString, QString>* C_info = compDB->value(compID);

        //compName->setText(C_info->value("ID"));
        compDescription->setText(C_info->value("Description"));
        compEDP->setText(C_info->value("Demand-Type"));
        compUnit->setText(C_info->value("BlockSize"));

        QString infoString = "";
        if (C_info->value("Demand-Directional") == "1") {
            infoString += "Directional";
        } else {
            infoString += "Non-directional";
        }

        if (C_info->value("RoundToInt") == "True") {
            infoString += "; round up quantities to integers";
        }

        if (C_info->value("Incomplete") == "1") {
            infoString += ";\t INCOMPLETE DATA!";
        }

        compInfo->setText(infoString);

        this->clearCompGroupWidget();
        this->retrieveCompGroups();
        
        fragilityViz->hide();
        fragilityViz->setVisible(false); // don't worry, we'll set it to true later, if needed

        // Then load the vulnerability model visualization

        QString vizDatabase;
        for (int db_i=1; db_i>=0; db_i--)
        {
            if (db_i==0){
                vizDatabase = cmpVulnerabilityDB_viz;
            } else if (db_i==1) {
                vizDatabase = additionalComponentDB_viz;
            }

            if (vizDatabase==""){
                continue;
            }

            //this->statusMessage("Loading figure from "+vizDatabase);

            QString htmlString;
            if (vizDatabase.endsWith("zip")) {

                QuaZip componentDBZip(vizDatabase);        
            
                if (!componentDBZip.open(QuaZip::mdUnzip)) {
                    this->errorMessage(QString(
                        "Error while trying to open figure zip file"));
                }

                if (!componentDBZip.setCurrentFile(selectedCompCombo->currentText()+".html")) {
                    this->statusMessage(QString("Cannot find figure for component ") + 
                        selectedCompCombo->currentText() + " in " + vizDatabase);
                    continue; // hoping that the component is in the other DB
                }

                QuaZipFile inFile(&componentDBZip);
                if (!inFile.open(QIODevice::ReadOnly)) {
                    this->errorMessage(QString(
                        "Error while trying to open figure for component " + 
                        selectedCompCombo->currentText()));
                } else {
                    QTextStream ts(&inFile);
                    htmlString = ts.readAll();
                    inFile.close();
                }
                
                componentDBZip.close();

            } else {

                // we assume that in every other case the viz DB points to a directory

                QFile inFile(vizDatabase + selectedCompCombo->currentText() + ".html");

                if (!inFile.exists()){
                    continue;
                }

                if (!inFile.open(QIODevice::ReadOnly)) {
                    this->errorMessage(QString(
                        "Error while trying to open figure for component " + 
                        selectedCompCombo->currentText()));
                } else {
                    QTextStream ts(&inFile);
                    htmlString = ts.readAll();
                    inFile.close();
                }
            }

            fragilityViz->setHtml(htmlString,
                QUrl::fromUserInput("/Users/"));                    
            // Zoom factor has a bug in Qt, below is a workaround
            QObject::connect(
                fragilityViz, &QWebEngineView::loadFinished,
                [=](bool arg) {
                    fragilityViz->setZoomFactor(0.75);
                    fragilityViz->show();
                    fragilityViz->setVisible(true);
                });

            break; // so that we do not check the following databases for the same component
        }

    } else {

        // Initialize the description fields and hide them if the combo is empty.
        //compName->setText("");
        compDescription->setText("");
        compEDP->setText("");
        compUnit->setText("");
        compInfo->setText("");
        this->clearCompGroupWidget();

        fragilityViz->hide();
        fragilityViz->setVisible(false);
    }

    
}

void
PelicunComponentContainer::deleteCompDB(){

    qDeleteAll(compDB->begin(), compDB->end());
    compDB->clear();
    delete compDB;
}

void
PelicunComponentContainer::storiesOrAreaChanged(){
    double w, d, area;
    theGI->getBuildingDimensions(w, d, area);

    double h;
    h = theGI->getHeight();

    theGI->setNumStoriesAndHeight(storiesValue->text().toInt(), h);

    theGI->setBuildingDimensions(w, d, planAreaValue->text().toDouble());
}

void
PelicunComponentContainer::setNumStories(int numStories, double dummy){

    this->storiesValue->setText(QString::number(numStories));
}

void
PelicunComponentContainer::setPlanArea(double dummy, double dummy2, double planArea){

    this->planAreaValue->setText(QString::number(planArea));
}

void
PelicunComponentContainer::setPlanAreaUnit(QString unitName){
    lblUnitSquared->setText(unitName+QString("²"));
    lblUnitSquared->update();
}

int PelicunComponentContainer::updateAvailableComponents(){

    this->statusMessage("Updating list of available components...");

    availableCompCombo->clear();

    QString componentDataBase;

    QStringList compIDs;

    QVector<QString> qvComp;

    //initialize the component map
    deleteCompDB();
    compDB = new QMap<QString, QMap<QString, QString>* >;

    for (int db_i=0; db_i<2; db_i++)
    {
        if (db_i==0){
            componentDataBase = cmpVulnerabilityDB;
        } else if (db_i==1) {
            componentDataBase = additionalComponentDB;
        }

        if (componentDataBase==""){
            continue;
        }

        //QString componentDataBase = this->updateComponentVulnerabilityDB();
        //QString componentDataBase = cmpVulnerabilityDB;

        if (componentDataBase.endsWith("csv")) {

            //this->statusMessage("Parsing component vulnerability data file...");

            QFile csvFile(componentDataBase);

            componentDataBase.chop(4);
            QFile jsonFile(componentDataBase+QString(".json"));

            if (csvFile.open(QIODevice::ReadOnly))
            {                
                
                QStringList header;

                // open the JSON file to get the metadata
                QJsonObject metaData;
                bool hasMeta = false;
                if (jsonFile.open(QFile::ReadOnly | QFile::Text)){
                    QString val = jsonFile.readAll();
                    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
                    metaData = doc.object();
                    jsonFile.close();
                    hasMeta = true;
                }

                // start the CSV stream
                QTextStream stream(&csvFile);

                int counter = 0;
                while (!stream.atEnd()) {
                    counter ++;

                    QString line = stream.readLine();
                    QStringList line_list;

                    //this->parseCSVLine(line, line_list);
                    parseCSVLine(line, line_list, this);

                    QString compName = line_list[0];

                    if (compName == "ID") {
                        //this->parseCSVLine(line, header);
                        parseCSVLine(line, header, this);
                        continue;
                    } 

                    QMap<QString, QString>* C_info;
                    if (compIDs.contains(compName)){
                        // Get the existing C_info dict from the compDB
                        C_info = compDB->value(compName);
                    }
                    else {
                        compIDs << compName;
                        // Create a new C_info dict and add it to the compDB dict
                        C_info = new QMap<QString, QString>;
                        compDB->insert(compName, C_info);
                    }

                    // Then fill the C_info with the info from the DL DB
                    for (int i=0; i<line_list.size(); i++){
                        if (i<header.size()) {
                            C_info -> insert(header[i], line_list[i]);
                        }
                    }

                    // and add info from metaData
                    if (hasMeta){
                        if (metaData.contains(compName)) {
                            QJsonObject compMetaData = metaData[compName].toObject();

                            // Description
                            if (compMetaData.contains("Description")){

                                QString description = compMetaData["Description"].toString();

                                if (compMetaData.contains("Comments")){
                                    description += "\n" + compMetaData["Comments"].toString();
                                }

                                C_info -> insert("Description", description);
                            } else {
                                C_info -> insert("Description", "");
                            }

                            // Block size
                            if (compMetaData.contains("SuggestedComponentBlockSize")){
                                QString blockSize = compMetaData["SuggestedComponentBlockSize"].toString();
                                C_info -> insert("BlockSize", blockSize);
                            } else {
                                C_info -> insert("BlockSize", QString("N/A"));
                            }

                            // Round to Integer
                            if (compMetaData.contains("RoundUpToIntegerQuantity")){
                                QString roundToInt = compMetaData["RoundUpToIntegerQuantity"].toString();
                                C_info -> insert("RoundToInt", roundToInt);
                            } else {
                                C_info -> insert("RoundToInt", QString("N/A"));
                            }
                        }
                    }

                }                

                csvFile.close();

            } else {
	      QString errMsg(QString("Cannot open CSV file: ") + componentDataBase);
	      this->errorMessage(errMsg);
	      return 1;
            }

            //this->statusMessage("Successfully parsed CSV file.");

        } else {
            this->errorMessage("Component data file is not in CSV format.");
            return 1;
        }
    }

    compIDs.sort();
    availableCompCombo->addItems(compIDs);

    return 0;
}

void
PelicunComponentContainer::updateComponentVulnerabilityDB(){

    bool cmpDataChanged = false;

    QString databasePath = this->getDefaultDatabasePath();

    // check the component vulnerability database set in the combo box
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString cmpVulnerabilityDB_tmp;

    if (databaseCombo->currentText() == "FEMA P-58") {
        cmpVulnerabilityDB_tmp = databasePath +        
        "/resources/SimCenterDBDL/damage_DB_FEMA_P58_2nd.csv";

    } else if (databaseCombo->currentText() == "Hazus Earthquake - Buildings") {
        cmpVulnerabilityDB_tmp = databasePath +
        "/resources/SimCenterDBDL/damage_DB_Hazus_EQ_bldg.csv";

    } else if (databaseCombo->currentText() == "Hazus Earthquake - Transportation") {
        cmpVulnerabilityDB_tmp = databasePath +
        "/resources/SimCenterDBDL/damage_DB_Hazus_EQ_trnsp.csv";        

    } else {

        cmpVulnerabilityDB_tmp = "";

    }

    // check if the component vulnerability database has changed
    if (cmpVulnerabilityDB != cmpVulnerabilityDB_tmp)
    {
        cmpDataChanged = true;
        cmpVulnerabilityDB = cmpVulnerabilityDB_tmp;

        if (cmpVulnerabilityDB != "")
        {
            this->statusMessage("Updating component list with " + 
                                QString(databaseCombo->currentText()) +
                                " data from "+ cmpVulnerabilityDB);

            cmpVulnerabilityDB_viz = generateFragilityInfo(cmpVulnerabilityDB);

            // load the visualization path too (assume that we have a zip file for every bundled DB)
            /*
            cmpVulnerabilityDB_viz = cmpVulnerabilityDB;
            cmpVulnerabilityDB_viz.chop(4);
            cmpVulnerabilityDB_viz = cmpVulnerabilityDB_viz + QString(".zip");
            */

        } else {
            this->statusMessage("Removing built-in component data from the list.");

            cmpVulnerabilityDB_viz = cmpVulnerabilityDB;
        }
    }

    // check if there is additional component data prescribed in the line edit
    QString additionalComponentDB_tmp;
    if (addComp->checkState() == 2)
    {
        additionalComponentDB_tmp = leAdditionalComponentDB->text();
    } else {
        additionalComponentDB_tmp = "";
    }

    // check if this additional data is new
    if (additionalComponentDB != additionalComponentDB_tmp)
    {
        cmpDataChanged = true;
        additionalComponentDB = additionalComponentDB_tmp;

        if (additionalComponentDB != "")
        {
            this->statusMessage("Updating component list with data from" + additionalComponentDB);    
            additionalComponentDB_viz = generateFragilityInfo(additionalComponentDB);
        } else {
            this->statusMessage("Removing additional component data from the list.");
            additionalComponentDB_viz = "";
        } 
    }

    if (cmpDataChanged == true)
        this->updateAvailableComponents();

    //return cmpVulnerabilityDB;
}

int
PelicunComponentContainer::setAdditionalComponentDB(QString additionalComponentDB_path){

    leAdditionalComponentDB->setText(additionalComponentDB_path);
    if (addComp->checkState() != 2){
        addComp->setChecked(true);    
    }
    return 0;
}

void
PelicunComponentContainer::chooseAdditionalComponentDB(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString additionalComponentDB_path =
        QFileDialog::getOpenFileName(this, tr("Select Database CSV File"), appDir);
        //QFileDialog::getExistingDirectory(this,tr("Select Database File"), appDir);

    this->setAdditionalComponentDB(additionalComponentDB_path);

    this->updateComponentVulnerabilityDB();
}

void
PelicunComponentContainer::exportComponentVulnerabilityDB(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString destinationFolder;
    destinationFolder =
        QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"),
                                          appDir);
    QDir destDir(destinationFolder);

    this->statusMessage(QString("Exporting component vulnerability database..."));

    // copy the db file(s) to the desired location
    //QFileInfo fi = leAdditionalComponentDB->text();
    QFileInfo fi = QFileInfo(cmpVulnerabilityDB);
    //QFileInfo fi = this->updateComponentVulnerabilityDB();

    // get the filenames
    QString csvFileName = fi.fileName();
    QString jsonFileName = fi.fileName();
    jsonFileName.replace(".csv", ".json");

    // get the source file paths
    QDir DBDir(fi.absolutePath());
    QString csvFile = DBDir.absoluteFilePath(csvFileName);
    QString jsonFile = DBDir.absoluteFilePath(jsonFileName);

    // get the destination file paths
    QString csvFile_dest = destDir.absoluteFilePath(csvFileName);
    QString jsonFile_dest = destDir.absoluteFilePath(jsonFileName);

    // check if the destination csv file exists and remove it if needed
    if (QFile::exists(csvFile_dest)){
        QFile::remove(csvFile_dest);
    }
    // copy the csv file
    QFile::copy(csvFile, csvFile_dest);

    // check if the destination json file exists and remove it if needed
    if (QFile::exists(jsonFile_dest)){
        QFile::remove(jsonFile_dest);
    }
    // check if the source json file exists
    if (QFile::exists(jsonFile)){
        // and copy it
        QFile::copy(jsonFile, jsonFile_dest);
    }

    /*
    scriptDir.cd("applications");
    scriptDir.cd("performDL");
    scriptDir.cd("pelicun");
    QString exportScript = scriptDir.absoluteFilePath("export_DB.py");
    scriptDir.cd("pelicun");
    scriptDir.cd("resources");
    QString dbFile = scriptDir.absoluteFilePath("FEMA_P58_2nd_ed.hdf");

    QProcess *proc = new QProcess();
    QString python = QString("python");
    QSettings settingsPy("SimCenter", "Common"); //These names will need to be constants to be shared
    QVariant  pythonLocationVariant = settingsPy.value("pythonExePath");
    if (pythonLocationVariant.isValid()) {
      python = pythonLocationVariant.toString();
    }

#ifdef Q_OS_WIN
    python = QString("\"") + python + QString("\"");
    QStringList args{exportScript, "--DL_DB_path",dbFile,"--target_dir",destinationFolder};

    qDebug() << "PYTHON COMMAND: " << python << args;

    proc->execute(python, args);

#else
    // note the above not working under linux because basrc not being called so no env variables!!

    QString command = QString("source $HOME/.bash_profile; \"") + python + QString("\" \"") +
        exportScript + QString("\"--DL_DB_path\"") + dbFile + QString("\"--target_dir\"") +
        destinationFolder;

    qDebug() << "PYTHON COMMAND: " << command;
    proc->execute("bash", QStringList() << "-c" <<  command);

#endif

    proc->waitForStarted();
    */

    this->statusMessage(QString("Successfully exported damage and loss database..."));
}

/*
void
PelicunComponentContainer::parseCSVLine(QString &line, QStringList &line_list)
{
    // parse the line considering "" and , and \n
    bool in_commented_block = false;
    bool save_element = false;
    int c_0 = 0;

    for (int c=0; c<line.length(); c++) {
        if (line[c] == "\"") {
            if (in_commented_block) {
                save_element = true;
                in_commented_block = false;
            } else {
                in_commented_block = true;
                c_0 = c+1;
            }
        } else if (line[c] == ",") {
            if (c_0 == c){
                line_list.append("");
                c_0++;
            } else if (in_commented_block == false) {
                save_element = true;
            }
        } else if (c == line.length()-1) {
            save_element = true;
            c++;
        }

        if (save_element) {
            QString element = line.mid(c_0, c-c_0);

            c_0 = c+1;
            line_list.append(element);
            save_element = false;

            if (c_0 < line.length()-1) {
                if (line[c_0-1] == "\"") {
                    if (line[c_0] != ",") {
                        this->statusMessage("Error while parsing CSV file at the following line: " + line);
                    } else {
                        c_0 ++;
                        c++;
                    }
                }
            }
        }
    }
}
*/

void
PelicunComponentContainer::deleteCompConfig(){

    // get an iterator for the main map
    QMap<QString, QVector<QMap<QString, QString>* >* >::iterator m;
    for (m=compConfig->begin(); m!=compConfig->end(); ++m){

        // for each vector in the map, get an iterator
        QVector<QMap<QString, QString>* > *vi = *m;
        QVector<QMap<QString, QString>* >::iterator i;
        for(i=vi->begin(); i != vi->end(); ++i){

            // free each vector element
            delete *i;
        }

        // then remove the vector elements
        vi->clear();

        // and free the vector itself
        delete *m;
    }

    // then remove the vectors from the map
    compConfig->clear();

    // and free the map
    delete compConfig;
}

void
PelicunComponentContainer::onLoadConfigClicked(void) {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Load Configuration"), "",
                                                    tr("All Files (*)"));

    if (this->loadComponentAssignment(fileName) != 0) {

        this->statusMessage("ERROR while loading component assignment from CSV file");
    }


}

int
PelicunComponentContainer::loadComponentAssignment(QString filePath) {

    int result = 1;

    QFile file(filePath);

    if (filePath.endsWith(".csv") == false) {

        this->statusMessage("ERROR: non-CSV Component Assignment file: " + filePath);

        return 1;
    }

    if (file.open(QIODevice::ReadOnly))
    {
        // clear the selectedCompCombo
        this->removeAllComponents();

        // remove the existing compConfig and initialize a new one
        deleteCompConfig();
        compConfig = new QMap<QString, QVector<QMap<QString, QString>* >* >;

        QTextStream stream(&file);

        //bool hasComment = false;
        bool hasDistribution = false;

        int counter = 0;
        while (!stream.atEnd()) {
            counter ++;

            QString line = stream.readLine();
            QStringList line_list;

            //this->parseCSVLine(line, line_list);
            parseCSVLine(line, line_list, this);

            QString compName = line_list[0];
            if (compName == "ID") {
	      // qDebug() << line_list;

                //if (line_list[line_list.count()-1] == "Comment") {
                //    hasComment = true;
                //}

                if (QString::compare(line_list[6], "Family", 
                    Qt::CaseInsensitive) == 0){
                    hasDistribution = true;
                }

                continue;
            }

            // first make sure that the comp ID is in the compConfig dict
            // and a corresponding CG_data vector is also stored there
            QVector<QMap<QString, QString>* > *vCG_data;
            if (compConfig->contains(compName)) {
                // get the vector of CG data from the compConfig dict
                vCG_data = compConfig->value(compName, nullptr);
            } else {
                vCG_data = new QVector<QMap<QString, QString>* >;
                compConfig->insert(compName, vCG_data);
            }

            // create a new CG_data dict and add it to the vector
            QMap<QString, QString> *CG_data = new QMap<QString, QString>;
            vCG_data->append(CG_data);

            // qDebug() << line_list;

            if (line_list.count() >= 6) {
                // fill the CG_data dict with the info from the given row in the file
                CG_data -> insert("unit",         line_list[1]);
                CG_data -> insert("location",     line_list[2]);
                CG_data -> insert("direction",    line_list[3]);
                CG_data -> insert("median",       line_list[4]);
                CG_data -> insert("blocks",       line_list[5]);

                if (hasDistribution == true) {
                    if (line_list.count() >= 8) {
                        CG_data -> insert("family",     line_list[6]);
                        CG_data -> insert("dispersion", line_list[7]);
                    } else {
                        CG_data -> insert("family",     "");
                        CG_data -> insert("dispersion", "");
                    }

                    if (line_list.count() >= 9) {
                        CG_data -> insert("comment", line_list[8]);
                    }
                } else {
                    CG_data -> insert("family",     "");
                    CG_data -> insert("dispersion", "");
                    CG_data -> insert("comment", line_list[6]);
                }
            } else {
                this->statusMessage("Error while parsing line " + QString::number(counter) + " in the config file");
            }
        }

        file.close();

        // add the component IDs to the selectedCompCombo
        for (auto compName: compConfig->keys()){

            if (availableCompCombo->findText(compName) != -1) {
                selectedCompCombo->addItem(compName);
            } else {
	      
                this->statusMessage("Component " + QString(compName) + "is not in the DL data library!");
            }
        }

        result = 0;

    } else {

        this->statusMessage("ERROR: Could not open Component Assignment file: " + QString(filePath));

    }

    return result;
}

int
PelicunComponentContainer::saveComponentAssignment(QString filePath) {

    int result = 1;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        //create the header
        stream << "\"ID\"" << ",";
        stream << "\"Units\"" << ",";
        stream << "\"Location\"" << ",";
        stream << "\"Direction\"" << ",";
        stream << "\"Theta_0\"" << ",";
        stream << "\"Blocks\"" << ",";
        stream << "\"Family\"" << ",";
        stream << "\"Theta_1\"" << ",";
        stream << "\"Comment\"";
        stream << "\n";

        // save the component characteristics
        const QStringList compList = selectedCBModel->stringList();

        for (const auto& compName: compList)
        {
            // get the vector of CG data from the compConfig dict
            QVector<QMap<QString, QString>* > *vCG_data =
                    compConfig->value(compName, nullptr);

            if (vCG_data != nullptr) {
                for (int i=0; i<vCG_data->count(); i++){
                    QMap<QString, QString> *CG_data = vCG_data->at(i);

                    stream << "\"" << compName << "\",";
                    stream << "\"" << CG_data -> value("unit", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("location", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("direction", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("median", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("blocks", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("family", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("dispersion", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("comment", tr("")) << "\"";
                    stream << "\n";
                }
            }
        }

        result = 0;
    }

    return result;
}

void
PelicunComponentContainer::onSaveConfigClicked(void) {

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Configuration"), "",
                                                    tr("All Files (*)"));

    if (this->saveComponentAssignment(fileName) != 0) {

        this->statusMessage("ERROR while saving component assignment to CSV file");
    }
}


PelicunComponentContainer::~PelicunComponentContainer()
{
    fragilityViz -> deleteLater();
}

void PelicunComponentContainer::addOneComponent()
{
    this->addComponent();
}

void PelicunComponentContainer::removeOneComponent()
{
    this->removeComponent();
}

void PelicunComponentContainer::addComponent()
{
    if (selectedCompCombo->findText(availableCompCombo->currentText()) == -1) {
        selectedCompCombo->addItem(availableCompCombo->currentText());
    }
}

void PelicunComponentContainer::addAllComponents(void)
{
    const QStringList compList = availableCBModel->stringList();

    for (const auto& compName : compList)
    {
        availableCompCombo->setCurrentText(compName);
        this->addComponent();
    }
}

void PelicunComponentContainer::removeComponent()
{
    selectedCompCombo->removeItem(selectedCompCombo->currentIndex());
}

void PelicunComponentContainer::removeAllComponents(void)
{
    selectedCompCombo->clear();
}

void PelicunComponentContainer::addComponentGroup(QMap<QString, QString> *CG_data_in)
{
    if (selectedCompCombo->count() > 0) {

       // add a new CG_data dict in the vector
       QMap<QString, QString> *CG_data;
       if (CG_data_in == nullptr) {

           // get the vector of CG data from the compConfig dict
           QVector<QMap<QString, QString>* > *vCG_data =
                   compConfig->value(selectedCompCombo->currentText(),
                                     nullptr);

           // create a new CG_data dict and add it to the vector
           CG_data = new QMap<QString, QString>;

           if (vCG_data != nullptr) {
                vCG_data->append(CG_data);

           } else {
               QVector<QMap<QString, QString>* > *new_vCG_data =
                                          new QVector<QMap<QString, QString>* >;

               new_vCG_data->append(CG_data);

               // save the vector of CG data to the compConfig dict
               compConfig->insert(selectedCompCombo->currentText(),
                                  new_vCG_data);
           }
       } else {
           CG_data = CG_data_in;
       }

       // create the new UI object and assign CG_data to it
       ComponentGroup *theComponentGroup = new ComponentGroup(nullptr,
                                                              CG_data);

       // add the ComponentGroup to the vector of CGs
       vComponentGroups.append(theComponentGroup);

       theComponentGroup->setMinimumHeight(20);
       theComponentGroup->setMaximumHeight(20);

       // add the ComponentGroup to the UI
       loCGList->insertWidget(-1, theComponentGroup);

       // add a remove button to the UI
       QPushButton *removeCGroup = new QPushButton();
       removeCGroup->setMinimumWidth(20);
       removeCGroup->setMaximumWidth(20);
       removeCGroup->setMaximumHeight(20);
       removeCGroup->setText(tr("-"));
       loCQuantityRemove->insertWidget(-1, removeCGroup);

       smRemoveCG->setMapping(removeCGroup, theComponentGroup);
       connect(removeCGroup, SIGNAL(clicked()), smRemoveCG, SLOT(map()));

       //connect(smRemoveCG,
       //       static_cast<void(QSignalMapper::*)(QWidget *)>(&QSignalMapper::mapped),
       //        this, SLOT(removeComponentGroup(QWidget)));
       //connect(removeCGroup, SIGNAL(pressed()), this,
       //        SLOT(removeComponentGroup(theComponentGroup)));

       vRemoveButtons.append(removeCGroup);
    }

}

void PelicunComponentContainer::removeComponentGroup(QWidget *theComponentGroup)
{

    if (selectedCompCombo->count() > 0) {

        // get the vector of CG data from the compConfig dict
        QVector<QMap<QString, QString>* > *vCG_data =
                compConfig->value(selectedCompCombo->currentText(),
                                  nullptr);
                                  //new QVector<QMap<QString, QString>* >);

        if (vCG_data != nullptr) {
            // remove the requested component group
            int CGcount = vComponentGroups.size();
            for (int i = CGcount-1; i >= 0; i--) {
              ComponentGroup *theComponentGroup_i = vComponentGroups.at(i);
              if (theComponentGroup_i == theComponentGroup){
                  //remove the widget from the UI
                  theComponentGroup_i->close();
                  loCGList->removeWidget(theComponentGroup);
                  //remove the CG_data from the database
                  vCG_data->remove(i);
                  //remove the CG from the UI vector
                  vComponentGroups.remove(i);
                  //delete the CG UI object
                  theComponentGroup->setParent(nullptr);
                  delete theComponentGroup;
                  //remove the PushButton from the button list
                  QPushButton *theRemoveButton = vRemoveButtons.at(i);
                  vRemoveButtons.remove(i);
                  theRemoveButton->setParent(nullptr);
                  delete theRemoveButton;
              }
            }
        }
    }
}

void PelicunComponentContainer::addPopulationGroup(QMap<QString, QString> *PG_data_in)
{

    // add a new PG_data dict in the vector
    QMap<QString, QString> *PG_data;
    if (PG_data_in == nullptr) {

        // create a new PG_data dict and add it to the vector
        PG_data = new QMap<QString, QString>;

        if (popConfig == nullptr) {
            popConfig = new QVector<QMap<QString, QString>* >;
        }

        popConfig->append(PG_data);

    } else {
       PG_data = PG_data_in;
    }

    // create the new UI object and assign PG_data to it
    PopulationGroup *thePopulationGroup = new PopulationGroup(nullptr, PG_data);

    // add the PopulationGroup to the vector of PGs
    vPopulationGroups.append(thePopulationGroup);

    //thePopulationGroup->setMinimumHeight(20);
    //thePopulationGroup->setMaximumHeight(20);

    // add the PopulationGroup to the UI
    loPGList->insertWidget(-1, thePopulationGroup);

    // add a remove button to the UI
    QPushButton *removePGroup = new QPushButton();
    removePGroup->setMinimumWidth(20);
    removePGroup->setMaximumWidth(20);
    removePGroup->setMaximumHeight(20);
    removePGroup->setText(tr("-"));
    loPQuantityRemove->insertWidget(-1, removePGroup);

    smRemovePG->setMapping(removePGroup, thePopulationGroup);
    connect(removePGroup, SIGNAL(clicked()), smRemovePG, SLOT(map()));

    vRemovePopButtons.append(removePGroup);

}

void PelicunComponentContainer::removePopulationGroup(QWidget *thePopulationGroup)
{

    if (popConfig != nullptr) {
        // remove the requested component group
        int PGcount = vPopulationGroups.size();
        for (int i = PGcount-1; i >= 0; i--) {
          PopulationGroup *thePopulationGroup_i = vPopulationGroups.at(i);
          if (thePopulationGroup_i == thePopulationGroup){
              //remove the widget from the UI
              loPGList->removeWidget(thePopulationGroup);
              thePopulationGroup_i->close();
              //remove the CG_data from the database
              popConfig->remove(i);
              //remove the CG from the UI vector
              vPopulationGroups.remove(i);
              //delete the CG UI object
              thePopulationGroup->setParent(nullptr);
              delete thePopulationGroup;
              //remove the PushButton from the button list
              QPushButton *theRemoveButton = vRemovePopButtons.at(i);
              vRemovePopButtons.remove(i);
              theRemoveButton->setParent(nullptr);
              delete theRemoveButton;
          }
        }
    }
}

bool
PelicunComponentContainer::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    QJsonObject assetData;

    assetData["NumberOfStories"] = storiesValue->text();
    assetData["PlanArea"] = planAreaValue->text();
    assetData["OccupancyType"] = occupancyType->currentText();
    assetData["ComponentDatabase"] = databaseCombo->currentText();

    if ((addComp->checkState() == 2) && (additionalComponentDB != "")){
        assetData["ComponentDatabasePath"] = additionalComponentDB;
    }

    WorkflowAppPBE *theInputApp = WorkflowAppPBE::getInstance(nullptr);

    this->statusMessage("Output file path (for cmp): " + QString(theInputApp->outputFilePath));

    QFileInfo fileInfo(theInputApp->outputFilePath);
    QString cmpFileName = QString("CMP_QNT.csv");
    QString cmpFilePath = fileInfo.dir().absoluteFilePath(cmpFileName);

    saveComponentAssignment(cmpFilePath);

    assetData["ComponentAssignmentFile"] = cmpFilePath;

    jsonObject["Asset"] = assetData;

    // result = false;

    if (result == false) {
      this->errorMessage("PelicunComponentContainer::outputToJSON failed\n");
    }
    
    return result;
}

bool
PelicunComponentContainer::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;

    QJsonObject assetData = jsonObject["Asset"].toObject();

    if (assetData.contains("NumberOfStories")) {
        storiesValue->setText(assetData["NumberOfStories"].toString());
    }
    if (assetData.contains("PlanArea")) {
        planAreaValue->setText(assetData["PlanArea"].toString());
    }
    if (assetData.contains("OccupancyType")) {
        occupancyType->setCurrentText(assetData["OccupancyType"].toString());
    }
    if (assetData.contains("ComponentDatabase")) {

        // ---
        // this is kept for backward compatibility - drop after PBE 4.0
        QString in_componentDB = assetData["ComponentDatabase"].toString();

        if (in_componentDB == "User Defined") {
            in_componentDB = "None";
        }

        if (in_componentDB == "Hazus Earthquake") {
            in_componentDB = "Hazus Earthquake - Buildings";
        }
        // ---

        databaseCombo->setCurrentText(in_componentDB);
    }
    if (assetData.contains("ComponentDatabasePath")){
        this->setAdditionalComponentDB(assetData["ComponentDatabasePath"].toString());
    } else {
        addComp->setChecked(false);
    }
    this->updateComponentVulnerabilityDB();

    if (assetData.contains("ComponentAssignmentFile")) {
        QString cmpFilePath = assetData["ComponentAssignmentFile"].toString();

        this->loadComponentAssignment(cmpFilePath);
    }

    /*
    // first, load the DL data folder
    QString pathString;
    pathString = jsonObject["ComponentDataFolder"].toString();
    leAdditionalComponentDB->setText(pathString);

    // clear the selectedCompCombo
    this->removeAllComponents();

    // remove the existing compConfig and initialize a new one
    deleteCompConfig();
    compConfig = new QMap<QString, QVector<QMap<QString, QString>* >* >;

    QJsonObject compData;
    if (jsonObject.contains("Components"))
      compData = jsonObject["Components"].toObject();

    foreach (const QString& compID, compData.keys()) {

        // first make sure that the comp ID is in the compConfig dict
        // and a corresponding CG_data vector is also stored there
        QVector<QMap<QString, QString>* > *vCG_data;
        if (compConfig->contains(compID)) {
          // get the vector of CG data from the compConfig dict
          vCG_data = compConfig->value(compID, nullptr);
        } else {
          vCG_data = new QVector<QMap<QString, QString>* >;
          compConfig->insert(compID, vCG_data);
        }

        QJsonArray CGDataList = compData[compID].toArray();
        foreach (const QJsonValue& compValue, CGDataList) {
            QJsonObject compInfo = compValue.toObject();

            // create a new CG_data dict and add it to the vector
            QMap<QString, QString> *CG_data = new QMap<QString, QString>;
            vCG_data->append(CG_data);

            CG_data -> insert("location",     compInfo["location"].toString());
            CG_data -> insert("direction",    compInfo["direction"].toString());
            CG_data -> insert("median",       compInfo["median_quantity"].toString());
            CG_data -> insert("unit",         compInfo["unit"].toString());
            CG_data -> insert("distribution", compInfo["distribution"].toString());
            if (compInfo.contains("dispersion"))
                CG_data -> insert("dispersion",          compInfo["dispersion"].toString());
            else
                CG_data -> insert("dispersion", tr(""));
        }
    }

    // add the component IDs to the selectedCompCombo
    for (auto compName: compConfig->keys()){
        if (availableCompCombo->findText(compName) != -1) {
            selectedCompCombo->addItem(compName);
        } else {
            qDebug() << "Component " << compName << "is not in the DL data folder!";
        }
    }
    */

    result = true;

    return result;
}

bool
PelicunComponentContainer::copyFiles(QString &dirName) {
    return true;
}

