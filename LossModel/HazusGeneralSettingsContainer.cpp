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

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <SectionTitle.h>
#include <QProcess>
#include <QCoreApplication>
#include <QSettings>

#include "HazusGeneralSettingsContainer.h"
#include "SimCenterPreferences.h"

HazusGeneralSettingsContainer::HazusGeneralSettingsContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    int maxWidth = 333;

    mainLayout = new QVBoxLayout();
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *mainV1Layout = new QVBoxLayout();
    QVBoxLayout *mainV2Layout = new QVBoxLayout();
    QVBoxLayout *mainV3Layout = new QVBoxLayout();

    // title -------------------------------------------------------------------
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title = new SectionTitle();
    title->setText(tr("General Settings"));
    title->setMinimumWidth(250);

    titleLayout->addWidget(title);
    titleLayout->addStretch();    

    // response model -------------------------------------------------------
    QGroupBox * responseGroupBox = new QGroupBox("Response Model");
    responseGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * responseFormLayout = new QFormLayout();

    QLabel *describeEDPLabel = new QLabel();
    describeEDPLabel->setText(tr("response description:"));
    responseFormLayout->addRow(describeEDPLabel);

    // EDP data
    QHBoxLayout *EDP_dataLayout = new QHBoxLayout();
    EDPFilePath = new QLineEdit;
    QPushButton *chooseEDP_data = new QPushButton();
    chooseEDP_data->setText(tr("Choose"));
    connect(chooseEDP_data, SIGNAL(clicked()),this,SLOT(chooseEDPFile()));
    EDP_dataLayout->addWidget(EDPFilePath);
    EDP_dataLayout->addWidget(chooseEDP_data);
    EDP_dataLayout->setSpacing(1);
    EDP_dataLayout->setContentsMargins(0,0,0,0);
    responseFormLayout->addRow(tr("    EDP data: "), EDP_dataLayout);

    // EDP distribution
    EDP_Distribution = new QComboBox();
    EDP_Distribution->setToolTip(tr("Approach used to re-sample EDPs.\n"
                                    "empirical - use raw data as is\n"
                                    "lognormal - use multivariate lognormal distribution\n"
                                    "truncated lognormal - truncate the multivar. lognorm. at the detection limits"));
    EDP_Distribution->addItem("empirical",0);
    EDP_Distribution->addItem("lognormal",1);
    EDP_Distribution->addItem("truncated lognormal",2);

    EDP_Distribution->setCurrentIndex(1);
    responseFormLayout->addRow(tr("    EDP distribution:"), EDP_Distribution);

    // basis of EDP fitting
    EDP_Fitting = new QComboBox();
    EDP_Fitting ->setToolTip(tr("Basis of EDP fitting (only used when the EDP distribution is not empirical)\n"
                                "all results - use all samples\n"
                                "non-collapse results - use only the samples that have all EDP values below the corresponding collapse limits"));
    EDP_Fitting ->addItem("all results",0);
    EDP_Fitting ->addItem("non-collapse results",1);

    EDP_Fitting ->setCurrentIndex(0);
    responseFormLayout->addRow(tr("    Basis:"), EDP_Fitting );

    // realizations
    realizationsValue = new QLineEdit();
    realizationsValue->setToolTip(tr("Number of simulated decision-variable samples."));
    realizationsValue->setText("2000");
    realizationsValue->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Realizations"), realizationsValue);

    QSpacerItem *spacerGroups7 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups7);

    // additional uncertainty
    QLabel *addedUncertaintyLabel = new QLabel();
    addedUncertaintyLabel->setText(tr("Additional Uncertainty:"));
    addedUncertaintyLabel->setToolTip(tr("Increase in logarithmic standard deviation of EDPs \n"
                                         "to consider additional sources of uncertainty."));
    responseFormLayout->addRow(addedUncertaintyLabel);

    addedUncertaintyGM = new QLineEdit();
    addedUncertaintyGM->setToolTip(tr("Uncertainty in the shape and amplitude of the target spectrum \n"
                                      "for intensity- and scenario-based assessment."));
    addedUncertaintyGM->setText("");
    addedUncertaintyGM->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Ground Motion"), addedUncertaintyGM);

    addedUncertaintyModel = new QLineEdit();
    addedUncertaintyModel->setToolTip(tr("Uncertainty resulting from inaccuracies in component modeling,\n"
                                         "damping and mass assumptions."));
    addedUncertaintyModel->setText("");
    addedUncertaintyModel->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Model"), addedUncertaintyModel);

     QSpacerItem *spacerGroups8 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups8);

    // detection limits
    QLabel *detLimLabel = new QLabel();
    detLimLabel->setText(tr("Detection Limits:"));
    responseFormLayout->addRow(detLimLabel);

    driftDetLim = new QLineEdit();
    driftDetLim->setToolTip(tr("Maximum interstory drift ratio that the \n"
                               "response estimation method can provide reliably."));
    driftDetLim->setText("");
    driftDetLim->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Interstory Drift"), driftDetLim);

    accDetLim = new QLineEdit();
    accDetLim->setToolTip(tr("Maximum floor acceleration that the \n"
                             "response estimation method can provide reliably."));
    accDetLim->setText("");
    accDetLim->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Floor Acceleration"), accDetLim);

    //QSpacerItem *spacerGroups2 = new QSpacerItem(10,10);
    //responseFormLayout->addItem(spacerGroups2);

    // set style
    responseFormLayout->setAlignment(Qt::AlignLeft);
    responseFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    responseFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    responseGroupBox->setLayout(responseFormLayout);

    // damage model -----------------------------------------------------------
    QGroupBox * damageGroupBox = new QGroupBox("Damage Model");
    damageGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * damageFormLayout = new QFormLayout(damageGroupBox);

    // building design information
    QLabel *buildingDesignLabel = new QLabel();
    buildingDesignLabel->setText(tr("Design Information:"));
    damageFormLayout->addRow(buildingDesignLabel);

    // structure type
    structureType = new QComboBox();
    structureType->setToolTip(tr("Model Building Type as per Table 5.1 in HAZUS MH 2.1"));
    structureType->addItem("W1", 0);
    structureType->addItem("W2", 1);
    structureType->addItem("S1L", 2);
    structureType->addItem("S1M", 3);
    structureType->addItem("S1H", 4);
    structureType->addItem("S2L", 5);
    structureType->addItem("S2M", 6);
    structureType->addItem("S2H", 7);
    structureType->addItem("S3", 8);
    structureType->addItem("S4L", 9);
    structureType->addItem("S4M",10);
    structureType->addItem("S4H",11);
    structureType->addItem("S5L",12);
    structureType->addItem("S5M",13);
    structureType->addItem("S5H",14);
    structureType->addItem("C1L",15);
    structureType->addItem("C1M",16);
    structureType->addItem("C1H",17);
    structureType->addItem("C2L",18);
    structureType->addItem("C2M",19);
    structureType->addItem("C2H",20);
    structureType->addItem("C3L",21);
    structureType->addItem("C3M",22);
    structureType->addItem("C3H",23);
    structureType->addItem("PC1",24);
    structureType->addItem("PC2L",25);
    structureType->addItem("PC2M",26);
    structureType->addItem("PC2H",27);
    structureType->addItem("RM1L",28);
    structureType->addItem("RM1M",29);
    structureType->addItem("RM2L",30);
    structureType->addItem("RM2M",31);
    structureType->addItem("RM2H",32);
    structureType->addItem("URML",33);
    structureType->addItem("URMM",34);
    structureType->addItem("MH",35);

    structureType->setItemData( 0, "Wood - Low Rise 1-2 stories", Qt::ToolTipRole);
    structureType->setItemData( 1, "Wood - Mid-Rise 2+ stories", Qt::ToolTipRole);
    structureType->setItemData( 2, "Steel Moment Frame - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData( 3, "Steel Moment Frame - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData( 4, "Steel Moment Frame - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData( 5, "Steel Braced Frame - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData( 6, "Steel Braced Frame - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData( 7, "Steel Braced Frame - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData( 8, "Steel Light Frame", Qt::ToolTipRole);
    structureType->setItemData( 9, "Steel Frame + Concrete Shear Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(10, "Steel Frame + Concrete Shear Walls - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(11, "Steel Frame + Concrete Shear Walls - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(12, "Steel Frame + Masonry Infill Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(13, "Steel Frame + Masonry Infill Walls - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(14, "Steel Frame + Masonry Infill Walls - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(15, "Concrete Moment Frame - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(16, "Concrete Moment Frame - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(17, "Concrete Moment Frame - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(18, "Concrete Shear Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(19, "Concrete Shear Walls - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(20, "Concrete Shear Walls - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(21, "Concrete Frame + Masonry Infill Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(22, "Concrete Frame + Masonry Infill Walls - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(23, "Concrete Frame + Masonry Infill Walls - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(24, "Precast Concrete Tilt-Up Walls", Qt::ToolTipRole);
    structureType->setItemData(25, "Precast Concrete Frames + Concrete Shear Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(26, "Precast Concrete Frames + Concrete Shear Walls - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(27, "Precast Concrete Frames + Concrete Shear Walls - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(28, "Reinforced Masonry Bearing Walls + Wood or Metal Deck Diaphragms - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(29, "Reinforced Masonry Bearing Walls + Wood or Metal Deck Diaphragms - Mid-Rise 4+ stories", Qt::ToolTipRole);
    structureType->setItemData(30, "Reinforced Masonry Bearing Walls + Precast Concrete Diaphragms - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(31, "Reinforced Masonry Bearing Walls + Precast Concrete Diaphragms - Mid-Rise 4-7 stories", Qt::ToolTipRole);
    structureType->setItemData(32, "Reinforced Masonry Bearing Walls + Precast Concrete Diaphragms - High-Rise 8+ stories", Qt::ToolTipRole);
    structureType->setItemData(33, "Unreinforced Masonry Bearing Walls - Low-Rise 1-3 stories", Qt::ToolTipRole);
    structureType->setItemData(34, "Unreinforced Masonry Bearing Walls - Mid-Rise 3+ stories", Qt::ToolTipRole);
    structureType->setItemData(35, "Mobile Homes", Qt::ToolTipRole);

    structureType->setCurrentIndex(0);
    damageFormLayout->addRow(tr("    Structure Type"), structureType);

    // seismic design level
    designLevel = new QComboBox();
    designLevel->setToolTip(tr("Seismic Design level as per HAZUS MH 2.1"));
    designLevel->addItem("High-Code", 0);
    designLevel->addItem("Moderate-Code", 1);
    designLevel->addItem("Low-Code", 2);
    designLevel->addItem("Pre-Code", 3);

    designLevel->setCurrentIndex(3);
    damageFormLayout->addRow(tr("    Design Level"), designLevel);

    QSpacerItem *spacerGroups5 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups5);


    // loss model -------------------------------------------------------------
    QGroupBox * lossGroupBox = new QGroupBox("Loss Model");
    lossGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * lossFormLayout = new QFormLayout(lossGroupBox);

    // replacement cost
    replacementCostValue = new QLineEdit();
    replacementCostValue->setToolTip(tr("The cost to replace the building with a new one.\n"
                                        "Note: following the HAZUS MH Technical Manual, \n"
                                        "repair costs are estimated as a fraction of \n"
                                        "the replacement cost provided here."));
    replacementCostValue->setText("1");
    replacementCostValue->setAlignment(Qt::AlignRight);
    lossFormLayout->addRow(tr("    Replacement Cost"), replacementCostValue);

    // replacement cost
    replacementTimeValue = new QLineEdit();
    replacementTimeValue->setToolTip(tr("The time it takes to replace the building with a new one.\n"
                                        "Note: the time unit used for replacement time must be \n"
                                        "consistent with those used in the consequence functions.\n"
                                        "Repair times in HAZUS MH are measured in days."));
    replacementTimeValue->setText("1");
    replacementTimeValue->setAlignment(Qt::AlignRight);
    lossFormLayout->addRow(tr("    Replacement Time"), replacementTimeValue);

    QSpacerItem *spacerGroups3 = new QSpacerItem(10,10);
    lossFormLayout->addItem(spacerGroups3);

    // decision variables
    QLabel *decisionVarLabel = new QLabel();
    decisionVarLabel->setText(tr("Decision variables of interest: "));
    decisionVarLabel->setToolTip(tr("Select the decision variables that shall be calculated."));
    lossFormLayout->addRow(decisionVarLabel);

    // reconstruction cost
    needRecCost = new QCheckBox();
    needRecCost->setText("");
    needRecCost->setChecked(true);

    // reconstruction time
    needRecTime = new QCheckBox();
    needRecTime->setText("");
    needRecTime->setChecked(true);

    QHBoxLayout *costAndTimeLayout = new QHBoxLayout();
    QLabel *recTimeLabel = new QLabel();
    recTimeLabel->setText(tr("    Repair Time"));
    costAndTimeLayout->addWidget(needRecCost);
    costAndTimeLayout->setSpacing(5);
    costAndTimeLayout->addWidget(recTimeLabel);
    costAndTimeLayout->addWidget(needRecTime);

    lossFormLayout->addRow(tr("    Repair Cost"),
                                 costAndTimeLayout);

    // injuries
    needInjuries = new QCheckBox();
    needInjuries->setText("");
    needInjuries->setChecked(true);

    lossFormLayout->addRow(tr("    Injuries"),
                                 needInjuries);

    QSpacerItem *spacerGroups6 = new QSpacerItem(10,10);
    lossFormLayout->addItem(spacerGroups6);

    // inhabitants
    QLabel *inhabitantLabel = new QLabel();
    inhabitantLabel->setText(tr("Inhabitants: "));
    lossFormLayout->addRow(inhabitantLabel);

    // occupancy
    occupancyType = new QComboBox();
    occupancyType->setToolTip(tr("Building Occupancy Class as per Table 3.2 in HAZUS MH2.1"));
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

    occupancyType->setItemData( 0, "Residential - Single Family Dwelling", Qt::ToolTipRole);
    occupancyType->setItemData( 1, "Residential - Mobile Home", Qt::ToolTipRole);
    occupancyType->setItemData( 2, "Residential - Multi Family Dwelling", Qt::ToolTipRole);
    occupancyType->setItemData( 3, "Residential - Temporary Lodging", Qt::ToolTipRole);
    occupancyType->setItemData( 4, "Residential - Institutional Dormitory", Qt::ToolTipRole);
    occupancyType->setItemData( 5, "Residential - Nursing Home", Qt::ToolTipRole);
    occupancyType->setItemData( 6, "Commercial - Retail Trade", Qt::ToolTipRole);
    occupancyType->setItemData( 7, "Commercial - Wholesale Trade", Qt::ToolTipRole);
    occupancyType->setItemData( 8, "Commercial - Personal and Repair Services", Qt::ToolTipRole);
    occupancyType->setItemData( 9, "Commercial - Professional/Technical Services", Qt::ToolTipRole);
    occupancyType->setItemData(10, "Commercial - Banks", Qt::ToolTipRole);
    occupancyType->setItemData(11, "Commercial - Hospital", Qt::ToolTipRole);
    occupancyType->setItemData(12, "Commercial - Medical Office/Clinic", Qt::ToolTipRole);
    occupancyType->setItemData(13, "Commercial - Entertainment & Recreation", Qt::ToolTipRole);
    occupancyType->setItemData(14, "Commercial - Theaters", Qt::ToolTipRole);
    occupancyType->setItemData(15, "Commercial - Parking", Qt::ToolTipRole);
    occupancyType->setItemData(16, "Industry - Heavy ", Qt::ToolTipRole);
    occupancyType->setItemData(17, "Industry - Light ", Qt::ToolTipRole);
    occupancyType->setItemData(18, "Industry - Food/Drugs/Chemical ", Qt::ToolTipRole);
    occupancyType->setItemData(19, "Industry - Metals/Minerals Processing ", Qt::ToolTipRole);
    occupancyType->setItemData(20, "Industry - High Technology ", Qt::ToolTipRole);
    occupancyType->setItemData(21, "Industry - Construction ", Qt::ToolTipRole);
    occupancyType->setItemData(22, "Agriculture", Qt::ToolTipRole);
    occupancyType->setItemData(23, "Church/Non-profit", Qt::ToolTipRole);
    occupancyType->setItemData(24, "Government - General Services", Qt::ToolTipRole);
    occupancyType->setItemData(25, "Government - Emergency Response", Qt::ToolTipRole);
    occupancyType->setItemData(26, "Education - Grade Schools", Qt::ToolTipRole);
    occupancyType->setItemData(27, "Education - Colleges/Universities", Qt::ToolTipRole);

    occupancyType->setCurrentIndex(0);
    lossFormLayout->addRow(tr("    Occupancy Type"), occupancyType);

    // peak population
    peakPopulation = new QLineEdit();
    peakPopulation->setToolTip(tr("Peak population in the building."));
    peakPopulation->setText("1");
    peakPopulation->setAlignment(Qt::AlignRight);
    lossFormLayout->addRow(tr("    Peak Population"), peakPopulation);

    // population distribution
    QHBoxLayout *populationLayout = new QHBoxLayout();
    populationFilePath = new QLineEdit;
    QPushButton *choosePopulation = new QPushButton();
    choosePopulation->setText(tr("Choose"));
    connect(choosePopulation, SIGNAL(clicked()),this,SLOT(choosePopulationFile()));
    populationLayout->addWidget(populationFilePath);
    populationLayout->addWidget(choosePopulation);
    populationLayout->setSpacing(1);
    populationLayout->setContentsMargins(0,0,0,0);

    lossFormLayout->addRow(tr("    Custom distribution: "),
                                populationLayout);

    QSpacerItem *spacerGroups14 = new QSpacerItem(10,10);
    lossFormLayout->addItem(spacerGroups14);

    // component database
    QLabel *componentLabel = new QLabel();
    componentLabel->setText(tr("Components: "));
    lossFormLayout->addRow(componentLabel);

    QHBoxLayout *fragilityLayout = new QHBoxLayout();
    fragilityFolderPath = new QLineEdit;
    QPushButton *chooseFragility = new QPushButton();
    chooseFragility->setText(tr("Choose"));
    connect(chooseFragility, SIGNAL(clicked()),this,SLOT(chooseFragilityDataBase()));
    fragilityLayout->addWidget(fragilityFolderPath);
    fragilityLayout->addWidget(chooseFragility);
    fragilityLayout->setSpacing(1);
    fragilityLayout->setContentsMargins(0,0,0,0);

    lossFormLayout->addRow(tr("Custom DL data: "),
                                fragilityLayout);

    QPushButton *btnExportDataBase = new QPushButton();
    btnExportDataBase->setMinimumWidth(150);
    //btnExportDataBase->setMaximumWidth(150);
    btnExportDataBase->setText(tr("Export Default DB"));
    connect(btnExportDataBase, SIGNAL(clicked()),this,SLOT(exportFragilityDataBase()));

    lossFormLayout->addRow(tr(" "), btnExportDataBase);

    // set style
    lossFormLayout->setAlignment(Qt::AlignLeft);
    lossFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    lossFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // assemble the widgets-----------------------------------------------------
    mainV1Layout->addWidget(responseGroupBox);
    mainV1Layout->addStretch(1);
    mainV1Layout->setSpacing(10);
    mainV1Layout->setContentsMargins(0,0,0,0);

    mainV2Layout->addWidget(damageGroupBox);
    mainV2Layout->addStretch(1);
    mainV2Layout->setSpacing(10);
    mainV2Layout->setContentsMargins(0,0,0,0);

    mainV3Layout->addWidget(lossGroupBox);
    mainV3Layout->addStretch(1);
    mainV3Layout->setSpacing(10);
    mainV3Layout->setContentsMargins(0,0,0,0);

    mainHLayout->addLayout(mainV1Layout, 1);
    mainHLayout->addLayout(mainV2Layout, 1);
    mainHLayout->addLayout(mainV3Layout, 1);
    mainHLayout->addStretch();
    mainHLayout->setSpacing(10);
    mainHLayout->setContentsMargins(0,0,0,0);

    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(mainHLayout, 0);
    mainLayout->addStretch(1);
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

QString
HazusGeneralSettingsContainer::getEDPFile(){
    return EDPFilePath->text();
}

int
HazusGeneralSettingsContainer::setEDPFile(QString EDPFile){
    EDPFilePath->setText(EDPFile);
    return 0;
}

void
HazusGeneralSettingsContainer::chooseEDPFile(void) {
    QString EDPFile;
    EDPFile=QFileDialog::getOpenFileName(this,tr("Select EDP File"),
        "C://", "All files (*.*)");
    this->setEDPFile(EDPFile);
}

QString
HazusGeneralSettingsContainer::getFragilityDataBase(){
    return fragilityFolderPath->text();
}

QString
HazusGeneralSettingsContainer::getPopulationFile(){
    return populationFilePath->text();
}

int
HazusGeneralSettingsContainer::setFragilityDataBase(QString fragilityFolder){
    fragilityFolderPath->setText(fragilityFolder);
    return 0;
}

void
HazusGeneralSettingsContainer::chooseFragilityDataBase(void) {
    QString fragilityFolder;
    fragilityFolder=QFileDialog::getExistingDirectory(this,tr("Select Folder"),
        "C://");
    this->setFragilityDataBase(fragilityFolder);
}

void
HazusGeneralSettingsContainer::exportFragilityDataBase(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString destinationFolder;
    destinationFolder = 
        QFileDialog::getExistingDirectory(this, tr("Select Destination Folder"),
                                          appDir);

    qDebug() << QString("Exporting default damage and loss database...");

    // run the export script    
    QDir scriptDir(appDir);
    scriptDir.cd("applications");
    scriptDir.cd("performDL");
    scriptDir.cd("pelicun");
    QString exportScript = scriptDir.absoluteFilePath("export_DB.py");
    scriptDir.cd("pelicunPBE");
    scriptDir.cd("resources");
    QString dbFile = scriptDir.absoluteFilePath("HAZUS_MH_2.1_EQ.hdf");

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

    qDebug() << QString("Successfully exported default damage and loss database...");
}

int
HazusGeneralSettingsContainer::setPopulationFile(QString populationFile){
    populationFilePath->setText(populationFile);
    return 0;
}

void
HazusGeneralSettingsContainer::choosePopulationFile(void) {
    QString populationFile;
    populationFile=QFileDialog::getOpenFileName(this,tr("Select File"),
        "C://", "All files (*.*)");
    this->setPopulationFile(populationFile);
}

HazusGeneralSettingsContainer::~HazusGeneralSettingsContainer()
{}

bool HazusGeneralSettingsContainer::outputToJSON(QJsonObject &outputObject) {

    // Response ---------------------------------------------------------------
    QJsonObject responseModel;

    QJsonObject responseDescription;
    responseDescription["EDP_Distribution"] = EDP_Distribution->currentText();
    responseDescription["BasisOfEDP_Distribution"] = EDP_Fitting->currentText();
    responseDescription["Realizations"] = realizationsValue->text();
    responseDescription["CoupledAssessment"] = false;
    if (EDPFilePath->text() != "")
        responseDescription["EDPDataFile"] = EDPFilePath->text();
    responseModel["ResponseDescription"] = responseDescription;

    QJsonObject detLims;
    detLims["PID"] = driftDetLim->text();
    detLims["PFA"] = accDetLim->text();
    responseModel["DetectionLimits"] = detLims;

    QJsonObject addUncertainty;
    addUncertainty["GroundMotion"] = addedUncertaintyGM->text();
    addUncertainty["Modeling"] = addedUncertaintyModel->text();
    responseModel["AdditionalUncertainty"] =  addUncertainty;

    outputObject["ResponseModel"] = responseModel;

    // Damage -----------------------------------------------------------------
    QJsonObject damageModel;

    damageModel["StructureType"] = structureType->currentText();
    damageModel["DesignLevel"] = designLevel->currentText();
    QString design_code;
    if (damageModel["DesignLevel"] == "High-Code") {
        design_code = "HC";
    } else if (damageModel["DesignLevel"] == "Moderate-Code") {
        design_code = "MC";
    } else if (damageModel["DesignLevel"] == "Low-Code") {
        design_code = "LC";
    } else if (damageModel["DesignLevel"] == "Pre-Code") {
        design_code = "PC";
    }

    outputObject["DamageModel"] = damageModel;

    // Loss -------------------------------------------------------------------
    QJsonObject lossModel;

    lossModel["ReplacementCost"] = replacementCostValue->text();
    lossModel["ReplacementTime"] = replacementTimeValue->text();

    QJsonObject decVars;
    decVars["ReconstructionCost"] = needRecCost->isChecked();
    decVars["ReconstructionTime"] = needRecTime->isChecked();
    decVars["Injuries"] = needInjuries->isChecked();
    lossModel["DecisionVariables"] = decVars;

    QJsonObject inhabitants;
    inhabitants["OccupancyType"] = occupancyType->currentText();
    inhabitants["PeakPopulation"] = peakPopulation->text();
    if (populationFilePath->text() != "")
        inhabitants["PopulationDataFile"] = populationFilePath->text();
    lossModel["Inhabitants"] = inhabitants;

    outputObject["LossModel"] = lossModel;

    // Component Data Folder --------------------------------------------------
    QString pathString = fragilityFolderPath->text();
    if (pathString != "")
        outputObject["ComponentDataFolder"] = pathString;

    // Components -------------------------------------------------------------
    QJsonObject compData;

    // set up the three components
    QString comp_S = "S-"+damageModel["StructureType"].toString()+"-"+design_code+"-"+inhabitants["OccupancyType"].toString();
    QJsonArray compArray_S;
    QJsonObject CGObj_S;
    CGObj_S["location"] = "1";
    CGObj_S["direction"] = "1";
    CGObj_S["median_quantity"] = "1.0";
    CGObj_S["unit"] = "ea";
    CGObj_S["distribution"] = "N/A";
    compArray_S.append(CGObj_S);
    compData[comp_S] = compArray_S;

    QString comp_NSA = "NSA-"+design_code+"-"+inhabitants["OccupancyType"].toString();
    QJsonArray compArray_NSA;
    QJsonObject CGObj_NSA;
    CGObj_NSA["location"] = "roof";
    CGObj_NSA["direction"] = "1";
    CGObj_NSA["median_quantity"] = "1.0";
    CGObj_NSA["unit"] = "ea";
    CGObj_NSA["distribution"] = "N/A";
    compArray_NSA.append(CGObj_NSA);
    compData[comp_NSA] = compArray_NSA;

    QString comp_NSD = "NSD-"+inhabitants["OccupancyType"].toString();
    QJsonArray compArray_NSD;
    QJsonObject CGObj_NSD;
    CGObj_NSD["location"] = "1";
    CGObj_NSD["direction"] = "1";
    CGObj_NSD["median_quantity"] = "1.0";
    CGObj_NSD["unit"] = "ea";
    CGObj_NSD["distribution"] = "N/A";
    compArray_NSD.append(CGObj_NSD);
    compData[comp_NSD] = compArray_NSD;

    outputObject["Components"] = compData;

    return 0;
}

bool HazusGeneralSettingsContainer::inputFromJSON(QJsonObject & inputObject) {

    // Response ---------------------------------------------------------------
    QJsonObject responseModel = inputObject["ResponseModel"].toObject();

    QJsonObject responseDescription = responseModel["ResponseDescription"].toObject();
    if (responseDescription.contains("EDPDataFile"))
        EDPFilePath->setText(responseDescription["EDPDataFile"].toString());
    else
        EDPFilePath->setText(tr(""));
    if (responseDescription.contains("EDP_Distribution"))
        EDP_Distribution->setCurrentText(responseDescription["EDP_Distribution"].toString());
    if (responseDescription.contains("BasisOfEDP_Distribution"))
        EDP_Fitting->setCurrentText(responseDescription["BasisOfEDP_Distribution"].toString());
    realizationsValue->setText(responseDescription["Realizations"].toString());

    QJsonObject detLims = responseModel["DetectionLimits"].toObject();
    driftDetLim->setText(detLims["PID"].toString());
    accDetLim->setText(detLims["PFA"].toString());

    QJsonObject addUncertainty = responseModel["AdditionalUncertainty"].toObject();
    addedUncertaintyGM->setText(addUncertainty["GroundMotion"].toString());
    addedUncertaintyModel->setText(addUncertainty["Modeling"].toString());

    // Damage -----------------------------------------------------------------
    QJsonObject damageModel = inputObject["DamageModel"].toObject();

    structureType->setCurrentText(damageModel["StructureType"].toString());
    designLevel->setCurrentText(damageModel["DesignLevel"].toString());

    // Loss -------------------------------------------------------------------
    QJsonObject lossModel = inputObject["LossModel"].toObject();

    replacementCostValue->setText(lossModel["ReplacementCost"].toString());
    replacementTimeValue->setText(lossModel["ReplacementTime"].toString());

    QJsonObject decVars = lossModel["DecisionVariables"].toObject();
    needRecCost->setChecked(decVars["ReconstructionCost"].toBool());
    needRecTime->setChecked(decVars["ReconstructionTime"].toBool());
    needInjuries->setChecked(decVars["Injuries"].toBool());

    QJsonObject inhabitants = lossModel["Inhabitants"].toObject();
    occupancyType->setCurrentText(inhabitants["OccupancyType"].toString());
    peakPopulation->setText(inhabitants["PeakPopulation"].toString());
    if (inhabitants.contains("PopulationDataFile"))
        populationFilePath->setText(inhabitants["PopulationDataFile"].toString());
    else
        populationFilePath->setText(tr(""));

    // Component Data Folder --------------------------------------------------

    if (inputObject.contains("ComponentDataFolder"))
        fragilityFolderPath->setText(inputObject["ComponentDataFolder"].toString());
    else
        fragilityFolderPath->setText(tr(""));

    return 0;
}
