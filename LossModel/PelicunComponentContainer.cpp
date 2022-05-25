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

#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <sectiontitle.h>
#include <QLineEdit>
#include <QPushButton>
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

PelicunComponentContainer::PelicunComponentContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    int maxWidth = 800;
    this->setMaximumWidth(maxWidth);

    // initialize the compDB Map
    compDB = new QMap<QString, QMap<QString, QString>* >;

    // initialize component property containers
    compConfig = new QMap<QString, QVector<QMap<QString, QString>* >* >;

    gridLayout = new QGridLayout();

    // general information ----------------------------------------------------
    QGroupBox *generalGroupBox = new QGroupBox("General Information");
    generalGroupBox->setMaximumWidth(300);

    QFormLayout *generalFormLayout = new QFormLayout();

    // stories
    storiesValue = new QLineEdit();
    storiesValue->setToolTip(tr("Number of stories above ground."));
    storiesValue->setText("");
    storiesValue->setAlignment(Qt::AlignRight);
    generalFormLayout->addRow(tr("Number of Stories"), storiesValue);

    // plan area
    planAreaValue = new QLineEdit();
    planAreaValue->setToolTip(tr("Plan area of the building. \n"
                                 "Unit is based on length unit defined on General Information (GI) panel."));
    planAreaValue->setText("");
    planAreaValue->setAlignment(Qt::AlignRight);
    generalFormLayout->addRow(tr("Plan Area"), planAreaValue);

    // occupancy
    occupancyType = new QComboBox();
    occupancyType->setToolTip(tr("The type of occupancy defines the function of the building.\n"
                                 "Occupancy classes from Hazus MH2.1 and FEMA P58 are available."));
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

    occupancyType->setItemData(29, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(30, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(31, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(32, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(33, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(34, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(35, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(36, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(37, "FEMA P58", Qt::ToolTipRole);
    occupancyType->setItemData(38, "FEMA P58", Qt::ToolTipRole);

    occupancyType->setCurrentIndex(0);
    generalFormLayout->addRow(tr("Occupancy Type"), occupancyType);

    generalGroupBox->setLayout(generalFormLayout);

    // Databases ----------------------------------------------------

    QGroupBox *DataBasesGroupBox = new QGroupBox("Databases");
    //DataBasesGroupBox->setMaximumWidth(maxWidth);
    //QFormLayout *compListFormLayout = new QFormLayout(DataBasesGroupBox);

    QVBoxLayout *loCEns = new QVBoxLayout(DataBasesGroupBox);

    // component data folder
    QHBoxLayout *selectDBLayout = new QHBoxLayout();

    QLabel *lblDBcomp = new QLabel();
    lblDBcomp->setText("Component Vulnerability:");
    lblDBcomp->setMaximumWidth(150);
    lblDBcomp->setMinimumWidth(150);
    selectDBLayout->addWidget(lblDBcomp);

    // database selection
    databaseCombo = new QComboBox();
    databaseCombo->setToolTip(tr("This database defines the components available for the analysis."));
    databaseCombo->addItem("FEMA P58",0);
    databaseCombo->addItem("Hazus Earthquake",1);
    databaseCombo->addItem("User Defined",2);

    databaseCombo->setItemData(0, "Based on the 2nd edition of FEMA P58", Qt::ToolTipRole);
    databaseCombo->setItemData(1, "Based on the Hazus MH 2.1 Earthquake Technical Manual", Qt::ToolTipRole);
    databaseCombo->setItemData(2, "Custom database provided by the user", Qt::ToolTipRole);

    connect(databaseCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAvailableComponents()));

    selectDBLayout->addWidget(databaseCombo, 1);

    QPushButton *btnExportDataBase = new QPushButton();
    btnExportDataBase->setMinimumWidth(100);
    btnExportDataBase->setMaximumWidth(100);
    btnExportDataBase->setText(tr("Export DB"));
    connect(btnExportDataBase, SIGNAL(clicked()),this,SLOT(exportFragilityDataBase()));
    selectDBLayout->addWidget(btnExportDataBase);

    loCEns->addLayout(selectDBLayout);

    //QSpacerItem *spacerGroups1 = new QSpacerItem(5,10);
    //loCEns->addItem(spacerGroups1);

    // database path
    QHBoxLayout *customFolderLayout = new QHBoxLayout();

    fragilityDataBasePath = new QLineEdit;
    fragilityDataBasePath->setToolTip(tr("Location of the user-defined component vulnerability data."));
    customFolderLayout->addWidget(fragilityDataBasePath, 1);
    fragilityDataBasePath->setVisible(false);
    fragilityDataBasePath->setEnabled(false);

    btnChooseFragility = new QPushButton();
    btnChooseFragility->setMinimumWidth(100);
    btnChooseFragility->setMaximumWidth(100);
    btnChooseFragility->setText(tr("Choose"));
    connect(btnChooseFragility, SIGNAL(clicked()),this,SLOT(chooseFragilityDataBase()));
    //connect(fragilityDataBasePath, SIGNAL(textChanged(QString)),this,SLOT(updateAvailableComponents()));
    customFolderLayout->addWidget(btnChooseFragility);
    btnChooseFragility->setVisible(false);

    connect(databaseCombo, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(DBSelectionChanged(QString)));

    loCEns->addLayout(customFolderLayout);

    // add line
    QFrame *lineDS = new QFrame();
    lineDS->setFrameShape(QFrame::HLine);
    lineDS->setFrameShadow(QFrame::Sunken);
    loCEns->addWidget(lineDS);

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
    databasePOPCombo->addItem("FEMA P58",0);
    databasePOPCombo->addItem("User Defined",1);
    databasePOPCombo->addItem("None",2);

    databasePOPCombo->setItemData(0, "Based on the 2nd edition of FEMA P58", Qt::ToolTipRole);
    databasePOPCombo->setItemData(1, "Custom database provided by the user", Qt::ToolTipRole);
    databasePOPCombo->setItemData(2, "No variation. The peak population is used for all realizations", Qt::ToolTipRole);

    //connect(databasePOPCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateAvailableComponents()));

    selectDB_POPLayout->addWidget(databasePOPCombo, 1);

    QPushButton *btnExportPOPDataBase = new QPushButton();
    btnExportPOPDataBase->setMinimumWidth(100);
    btnExportPOPDataBase->setMaximumWidth(100);
    btnExportPOPDataBase->setText(tr("Export DB"));
    //connect(btnExportPOPDataBase, SIGNAL(clicked()),this,SLOT(exportFragilityDataBase()));
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
    //connect(btnChoosePOPDB, SIGNAL(clicked()),this,SLOT(chooseFragilityDataBase()));
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

    QLabel *lblPOP_cov = new QLabel();
    lblPOP_cov->setText(" COV");
    lblPOP_cov->setMaximumWidth(45);
    lblPOP_cov->setMinimumWidth(45);
    loPGroup_header->addWidget(lblPOP_cov);

    QLabel *lblPOP_note = new QLabel();
    lblPOP_note->setText(" Comment");
    loPGroup_header->addWidget(lblPOP_note, 1);

    loPGroup_header->addStretch();
    loPGroup_header->setSpacing(10);
    loPGroup_header->setMargin(0);

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
    loPGList->setMargin(0);
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

    this-> updateAvailableComponents();

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

    QLabel *lblQNT_cov = new QLabel();
    lblQNT_cov->setText(" COV");
    lblQNT_cov->setMaximumWidth(45);
    lblQNT_cov->setMinimumWidth(45);
    loCGroup_header->addWidget(lblQNT_cov);

    QLabel *lblQNT_note = new QLabel();
    lblQNT_note->setText(" Comment");
    loCGroup_header->addWidget(lblQNT_note, 1);

    loCGroup_header->addStretch();
    loCGroup_header->setSpacing(10);
    loCGroup_header->setMargin(0);

    loCDetails->addLayout(loCGroup_header);

    // Component Group list
    QHBoxLayout *loCQuantityLayout = new QHBoxLayout();

    loCQuantityRemove = new QVBoxLayout();

    loCQuantityRemove->addStretch();

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
    loCGList->addStretch();
    loCGList->setMargin(0);
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
}

void
PelicunComponentContainer::DBSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        fragilityDataBasePath->setVisible(true);
        btnChooseFragility->setVisible(true);
    } else {
        fragilityDataBasePath->setVisible(false);
        btnChooseFragility->setVisible(false);
    }
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

    if (selectedCompCombo->count() > 0) {

        if (selectedCompCombo->currentText() != "") {

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

            /*
            if (dbType == "JSON") {

                QString compFileName = selectedCompCombo->currentText() + ".json";
                QDir fragDir(this->getFragilityDataBase());
                QFile compFile(fragDir.absoluteFilePath(compFileName));
                compFile.open(QFile::ReadOnly | QFile::Text);

                QString val;
                val = compFile.readAll();
                QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
                QJsonObject compData = doc.object();
                compFile.close();

                compName->setText(compData["Name"].toString());

                QJsonObject compGI = compData["GeneralInformation"].toObject();
                compDescription->setText(compGI["Description"].toString());

                QJsonObject compEDPVar = compData["EDP"].toObject();
                compEDP->setText(compEDPVar["Type"].toString());

                QJsonArray compQData = compData["QuantityUnit"].toArray();
                //compUnit->setText(QString("%1").arg(compQData[0].toDouble()));
                compUnit->setText(QString("%1").arg(compQData[0].toDouble())+QString(" ")+
                                  compQData[1].toString());

                QString infoString = "";
                if (compData["Directional"] == true) {
                    infoString += "Directional, ";
                } else {
                    infoString += "Non-directional, ";
                }
                if (compData["Correlated"] == true) {
                    infoString += "identical behavior among Component Groups.";
                } else {
                    infoString += "independent behavior among Component Groups.";
                }

                if (compGI["Incomplete"] == true) infoString += "  INCOMPLETE DATA!";
                compInfo->setText(infoString);
            }
            else if (dbType == "HDF5") {

                QString compID = selectedCompCombo->currentText();
                QMap<QString, QString>* C_info = compDB->value(compID);

                compName->setText(C_info->value("Name"));
                compDescription->setText(C_info->value("Description"));
                compEDP->setText(C_info->value("EDPType"));
                compUnit->setText(C_info->value("QuantityUnit"));

                QString infoString = "";
                if (C_info->value("Directional") == "1") {
                    infoString += "Directional, ";
                } else {
                    infoString += "Non-directional, ";
                }
                if (C_info->value("Correlated") == "1") {
                    infoString += "identical behavior among Component Groups.";
                } else {
                    infoString += "independent behavior among Component Groups.";
                }

                if (C_info->value("Incomplete") == "1") infoString += "  INCOMPLETE DATA!";
                compInfo->setText(infoString);
            }
            */

            this->clearCompGroupWidget();
            this->retrieveCompGroups();
        }
    } else {
        //compName->setText("");
        compDescription->setText("");
        compEDP->setText("");
        compUnit->setText("");
        compInfo->setText("");
        this->clearCompGroupWidget();
    }
}

void
PelicunComponentContainer::deleteCompDB(){

    qDeleteAll(compDB->begin(), compDB->end());
    compDB->clear();
    delete compDB;
}

int PelicunComponentContainer::updateAvailableComponents(){

    availableCompCombo->clear();

    QString fragilityDataBase = this->getFragilityDataBase();

    QStringList compIDs;

    QVector<QString> qvComp;

    if (fragilityDataBase.endsWith("csv")) {

        qDebug() << "CSV fragility data file recognized.";

        QFile csvFile(fragilityDataBase);

        fragilityDataBase.chop(4);
        QFile jsonFile(fragilityDataBase+QString(".json"));

        if (csvFile.open(QIODevice::ReadOnly))
        {

            //initialize the component map
            deleteCompDB();
            compDB = new QMap<QString, QMap<QString, QString>* >;
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

                this->parseCSVLine(line, line_list);

                QString compName = line_list[0];

                if (compName == "ID") {
                    this->parseCSVLine(line, header);
                    continue;
                } else {
                    compIDs << compName;
                }

                // Create a new C_info dict and add it to the compDB dict

                QMap<QString, QString> *C_info = new QMap<QString, QString>;
                compDB->insert(compName, C_info);

                // Then fill it with the info from the DL DB
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



                //C_info -> insert("Name",         qvName[i]);
                //C_info -> insert("Description",  qvDescription[i]);
                //C_info -> insert("EDPType",      qvEDPType[i]);
                //C_info -> insert("QuantityUnit",
                //    QString("%1").arg(qvQuantityCount[i])+QString(" ")+qvQuantityUnit[i]);
                //C_info -> insert("Directional",  QString::number(qvDirectional[i]));
                //C_info -> insert("Correlated",   QString::number(qvCorrelated[i]));
                //C_info -> insert("Incomplete",   QString::number(qvIncomplete[i]));
            }

            availableCompCombo->addItems(compIDs);

            csvFile.close();

        } else {
            // send a warning message
            return 1;
        }


    } else {
        // send a warning message
        return 1;
    }

    return 0;
}

/*
int
PelicunComponentContainer::updateAvailableComponents_old(){

    availableCompCombo->clear();

    QString fragilityDataBase = this->getFragilityDataBase();

    QStringList compIDs;

    if (fragilityDataBase.endsWith("hdf")) {

        qDebug() << "HDF file recognized";
        dbType = "HDF5";

        // we are dealing with an HDF5 file...

        HDF5Data *DLDB = new HDF5Data(fragilityDataBase);

        QVector<QString> qvComp;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("index"), &qvComp);

        // when using an HDF5 file, all component info is loaded at once here
        QVector<QString> qvName;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("Name"), &qvName);

        QVector<QString> qvDescription;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard/GeneralInformation")),
                          QString("Description"), &qvDescription);

        QVector<QString> qvEDPType;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard/EDP")),
                          QString("Type"), &qvEDPType);

        QVector<int> qvQuantityCount;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("QuantityUnit#0"), &qvQuantityCount);

        QVector<QString> qvQuantityUnit;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("QuantityUnit#1"), &qvQuantityUnit);

        QVector<int> qvDirectional;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("Directional"), &qvDirectional);

        QVector<int> qvCorrelated;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard")),
                          QString("Correlated"), &qvCorrelated);

        QVector<int> qvIncomplete;
        DLDB->getDFEntry(DLDB->getMember(QString("/data_standard/GeneralInformation")),
                          QString("Incomplete"), &qvIncomplete);

        //initialize the component map
        deleteCompDB();
        compDB = new QMap<QString, QMap<QString, QString>* >;

        for (int i =0; i < qvComp.size(); i++){

            QString compName = qvComp[i];
            compIDs << compName;

            // Create a new C_info dict and add it to the compDB dict

            QMap<QString, QString> *C_info = new QMap<QString, QString>;
            compDB->insert(compName, C_info);

            // Then fill it with the info from the DL DB

            C_info -> insert("Name",         qvName[i]);
            C_info -> insert("Description",  qvDescription[i]);
            C_info -> insert("EDPType",      qvEDPType[i]);
            C_info -> insert("QuantityUnit",
                QString("%1").arg(qvQuantityCount[i])+QString(" ")+qvQuantityUnit[i]);
            C_info -> insert("Directional",  QString::number(qvDirectional[i]));
            C_info -> insert("Correlated",   QString::number(qvCorrelated[i]));
            C_info -> insert("Incomplete",   QString::number(qvIncomplete[i]));
        }

        delete DLDB;

    } else {

        dbType = "JSON";

        // we are dealing with json files...
        QDir directory(fragilityDataBase);

        compIDs = directory.entryList(QStringList() << "*.json" << "*.JSON", QDir::Files);

        for (int i=0; i<compIDs.length(); i++){
            compIDs[i] = compIDs[i].remove(compIDs[i].size()-5, 5);
        }
    }

    availableCompCombo->addItems(compIDs);

    return 0;
}
*/

QString
PelicunComponentContainer::getFragilityDataBase(){

    QString fragilityDataBase;
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    if (databaseCombo->currentText() == "FEMA P58") {

        fragilityDataBase = appDir +
        "/applications/performDL/pelicun3/pelicunPBE/resources/fragility_DB_FEMA_P58_2nd.csv";

    } else if (databaseCombo->currentText() == "Hazus Earthquake") {

        fragilityDataBase = appDir +
        "/applications/performDL/pelicun3/pelicunPBE/resources/fragility_DB_HAZUS_EQ.csv";

    } else {

        fragilityDataBase = fragilityDataBasePath->text();
    }

    qDebug() << "updating frag folder with " << fragilityDataBase;

    // point to the frag DB in the line edit even if it is the default one
    fragilityDataBasePath->setText(fragilityDataBase);

    return fragilityDataBase;

// This does not seem to be necessary any longer
//#ifdef Q_OS_WIN
//        fragilityDataBase = appDir +
//        "/applications/performDL/pelicun3/pelicunPBE/resources/fragility_DB_FEMA_P58_2nd.csv";
//#else
//        fragilityDataBase = appDir +
//        "/applications/performDL/pelicun3/pelicunPBE/resources/fragility_DB_FEMA_P58_2nd.csv";
//#endif
//        //"/applications/performDL/pelicun/pelicunPBE/resources/DL json/";
}

int
PelicunComponentContainer::setFragilityDataBase(QString fragilityDataBase){

    fragilityDataBasePath->setText(fragilityDataBase);
    return 0;
}

void
PelicunComponentContainer::chooseFragilityDataBase(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString fragilityDataBase;
    fragilityDataBase =
        QFileDialog::getOpenFileName(this, tr("Select Database CSV File"), appDir);
        //QFileDialog::getExistingDirectory(this,tr("Select Database File"), appDir);

    this->setFragilityDataBase(fragilityDataBase);

    this->updateAvailableComponents();
}

void
PelicunComponentContainer::exportFragilityDataBase(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString destinationFolder;
    destinationFolder =
        QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"),
                                          appDir);
    QDir destDir(destinationFolder);

    qDebug() << QString("Exporting damage and loss database...");

    // copy the db file(s) to the desired location
    QFileInfo fi = fragilityDataBasePath->text();

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
    scriptDir.cd("pelicunPBE");
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

    qDebug() << QString("Successfully exported default damage and loss database...");
}

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
        }
    }
}

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

    QFile file(fileName);

    if (file.open(QIODevice::ReadOnly))
    {
        // clear the selectedCompCombo
        this->removeAllComponents();

        // remove the existing compConfig and initialize a new one
        deleteCompConfig();
        compConfig = new QMap<QString, QVector<QMap<QString, QString>* >* >;

        QTextStream stream(&file);

        int counter = 0;
        while (!stream.atEnd()) {
            counter ++;

            QString line = stream.readLine();
            QStringList line_list;

            this->parseCSVLine(line, line_list);

            QString compName = line_list[0];
            if (compName == "ID") {
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

            if (line_list.count() >= 6) {
                // fill the CG_data dict with the info from the given row in the file
                CG_data -> insert("location",     line_list[1]);
                CG_data -> insert("direction",    line_list[2]);
                CG_data -> insert("median",       line_list[3]);
                CG_data -> insert("unit",         line_list[4]);
                CG_data -> insert("distribution", line_list[5]);
                if (line_list.count() >= 7) {
                    CG_data -> insert("cov",      line_list[6]);
                } else {
                    CG_data -> insert("cov",      "");
                }
            } else {
                qDebug() << "Error while parsing line " << counter << " in the config file";
            }
        }

        file.close();

        // add the component IDs to the selectedCompCombo
        for (auto compName: compConfig->keys()){

            if (availableCompCombo->findText(compName) != -1) {
                selectedCompCombo->addItem(compName);
            } else {
                qDebug() << "Component " << compName << "is not in the DL data folder!";
            }
        }

    }
}

void
PelicunComponentContainer::onSaveConfigClicked(void) {

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Configuration"), "",
                                                    tr("All Files (*)"));

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);

        //create the header
        stream << "\"ID\"" << ",";
        stream << "\"Location(s)\"" << ",";
        stream << "\"Direction(s)\"" << ",";
        stream << "\"Median Quantity\"" << ",";
        stream << "\"Unit\"" << ",";
        stream << "\"Distribution\"" << ",";
        stream << "\"COV\"";
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
                    stream << "\"" << CG_data -> value("location", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("direction", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("median", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("unit", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("distribution", tr("")) << "\",";
                    stream << "\"" << CG_data -> value("cov", tr("")) << "\"";
                    stream << "\n";
                }
            }
        }
    }
}


PelicunComponentContainer::~PelicunComponentContainer()
{

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
       loCGList->insertWidget(loCGList->count()-1, theComponentGroup);

       // add a remove button to the UI
       QPushButton *removeCGroup = new QPushButton();
       removeCGroup->setMinimumWidth(20);
       removeCGroup->setMaximumWidth(20);
       removeCGroup->setMaximumHeight(20);
       removeCGroup->setText(tr("-"));
       loCQuantityRemove->insertWidget(loCQuantityRemove->count()-1,
                                       removeCGroup);

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

    thePopulationGroup->setMinimumHeight(20);
    thePopulationGroup->setMaximumHeight(20);

    // add the PopulationGroup to the UI
    loPGList->insertWidget(loPGList->count()-1, thePopulationGroup);

    // add a remove button to the UI
    QPushButton *removePGroup = new QPushButton();
    removePGroup->setMinimumWidth(20);
    removePGroup->setMaximumWidth(20);
    removePGroup->setMaximumHeight(20);
    removePGroup->setText(tr("-"));
    loPQuantityRemove->insertWidget(loPQuantityRemove->count()-1,
                                   removePGroup);

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
              thePopulationGroup_i->close();
              loPGList->removeWidget(thePopulationGroup);
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

    // first, save the DL data folder
    QString pathString;
    pathString = fragilityDataBasePath->text();
    if (pathString != ""){
        jsonObject["ComponentDataFolder"] = pathString;
    }

    // then, for each component save the component information
    QJsonObject compData;
    if (jsonObject.contains("Components"))
        compData = jsonObject["Components"].toObject();

    const QStringList compList = selectedCBModel-> stringList();
    foreach (const QString compID, compList){

        // get the vector of CG data from the compConfig dict
        QVector<QMap<QString, QString>* > *vCG_data =
                compConfig->value(compID, nullptr);

        if (vCG_data != nullptr) {
            QJsonArray compArray;
            for (int i=0; i<vCG_data->count(); i++){
                QMap<QString, QString> *CG_data = vCG_data->at(i);
                QJsonObject CGObj;

                CGObj["location"] = CG_data -> value("location", tr(""));
                CGObj["direction"] = CG_data -> value("direction", tr(""));
                CGObj["median_quantity"] = CG_data -> value("median", tr(""));
                CGObj["unit"] = CG_data -> value("unit", tr(""));
                CGObj["distribution"] = CG_data -> value("distribution", tr(""));
                CGObj["cov"] = CG_data -> value("cov", tr(""));

                compArray.append(CGObj);
            }

            compData[compID] = compArray;
        }
    }
    jsonObject["Components"]=compData;

    return result;
}

bool
PelicunComponentContainer::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = true;

    // first, load the DL data folder
    QString pathString;
    pathString = jsonObject["ComponentDataFolder"].toString();
    fragilityDataBasePath->setText(pathString);

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
            if (compInfo.contains("cov"))
                CG_data -> insert("cov",          compInfo["cov"].toString());
            else
                CG_data -> insert("cov", tr(""));
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

    return result;
}

bool
PelicunComponentContainer::copyFiles(QString &dirName) {
    return true;
}

