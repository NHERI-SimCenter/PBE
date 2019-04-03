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
#include <QJsonObject>
#include <sectiontitle.h>

#include "P58GeneralSettingsContainer.h"

P58GeneralSettingsContainer::P58GeneralSettingsContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

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

    QSpacerItem *spacerTop1 = new QSpacerItem(10,30);
    // QSpacerItem *spacerGroupHeader = new QSpacerItem(20,5);
    // QSpacerItem *spacerGroupMembers = new QSpacerItem(20,1);

    // building response -------------------------------------------------------
    QGroupBox * responseGroupBox = new QGroupBox("Building Response");
    QFormLayout * responseFormLayout = new QFormLayout();

    // add elements

    // yield drift
    yieldDriftValue = new QLineEdit();
    yieldDriftValue->setToolTip(tr("Interstory drift ratio corresponding to significant yielding (i.e. when the forces in the main components of the lateral load resisting system reach the plastic capacity of the components) in the structure."));
    yieldDriftValue->setText("0.01");
    yieldDriftValue->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Yield Drift Ratio"), yieldDriftValue);

    QSpacerItem *spacerGroups1 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups1);

     // detection limits
    QLabel *detLimLabel = new QLabel();
    detLimLabel->setText(tr("Detection Limits:"));
    responseFormLayout->addRow(detLimLabel);

    driftDetLim = new QLineEdit();
    driftDetLim->setToolTip(tr("Maximum interstory drift ratio that the response estimation method can provide reliably."));    
    driftDetLim->setText("0.15");
    driftDetLim->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Interstory Drift"), driftDetLim);

    accDetLim = new QLineEdit();
    accDetLim->setToolTip(tr("Maximum floor acceleration that the response estimation method can provide reliably."));    
    accDetLim->setText("");
    accDetLim->setAlignment(Qt::AlignRight);
    responseFormLayout->addRow(tr("    Floor Acceleration"), accDetLim);

    QSpacerItem *spacerGroups2 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups2);

    // set style
    responseFormLayout->setAlignment(Qt::AlignLeft);
    responseFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    responseFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    responseGroupBox->setLayout(responseFormLayout);

    // building damage -------------------------------------------------------
    QGroupBox * damageGroupBox = new QGroupBox("Building Damage and Loss");
    QFormLayout * damageFormLayout = new QFormLayout(damageGroupBox);
    
    // add elements

    // replacement cost
    replacementCostValue = new QLineEdit();
    replacementCostValue->setToolTip(tr("Some tooltip"));
    replacementCostValue->setText("");
    replacementCostValue->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Replacement Cost"), replacementCostValue);

    // replacement cost
    replacementTimeValue = new QLineEdit();
    replacementTimeValue->setToolTip(tr("Some tooltip"));
    replacementTimeValue->setText("");
    replacementTimeValue->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Replacement Time"), replacementTimeValue);

     QSpacerItem *spacerGroups3 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups3);

    // irrepairable residual drift
    QLabel *irrepResDriftLabel = new QLabel();
    irrepResDriftLabel->setText(tr("Irrepairable Residual Drift:"));
    damageFormLayout->addRow(irrepResDriftLabel);

    irrepResDriftMedian = new QLineEdit();
    irrepResDriftMedian->setToolTip(tr("Median of the residual drift distribution conditioned on irrepairable damage in the building."));    
    irrepResDriftMedian->setText("0.15");
    irrepResDriftMedian->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Median"), irrepResDriftMedian);
    
    irrepResDriftStd = new QLineEdit();
    irrepResDriftStd->setToolTip(tr("Logarithmic standard deviation of the residual drift distribution conditioned on irrepairable damage in the building."));    
    irrepResDriftStd->setText("0.3");
    irrepResDriftStd->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Log Standard Dev"), irrepResDriftStd);
    
     QSpacerItem *spacerGroups4 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups4);

    // collapse limits
    QLabel *colLimLabel = new QLabel();
    colLimLabel->setText(tr("Collapse Limits:"));
    damageFormLayout->addRow(colLimLabel);

    driftColLim = new QLineEdit();
    driftColLim->setToolTip(tr("Peak interstory drift ratio that corresponds to the collapse of the building."));    
    driftColLim->setText("0.15");
    driftColLim->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Interstory Drift"), driftColLim);
    
    accColLim = new QLineEdit();
    accColLim->setToolTip(tr("Peak floor acceleration that corresponds to the collapse of the building."));    
    accColLim->setText("");
    accColLim->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Floor Acceleration"), accColLim);
    
     QSpacerItem *spacerGroups5 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups5);

    // set style
    damageFormLayout->setAlignment(Qt::AlignLeft);
    damageFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    damageFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);


    // uncertainty quantification ----------------------------------------------
    QGroupBox * uqGroupBox = new QGroupBox("Uncertainty Quantification");
    QFormLayout * uqFormLayout = new QFormLayout(uqGroupBox);
    
    // add elements

    // realizations
    realizationsValue = new QLineEdit();
    realizationsValue->setToolTip(tr("Number of simulated building-performance outcomes."));
    realizationsValue->setText("100000");
    realizationsValue->setAlignment(Qt::AlignRight);
    uqFormLayout->addRow(tr("    Realizations"), realizationsValue);

     QSpacerItem *spacerGroups7 = new QSpacerItem(10,10);
    uqFormLayout->addItem(spacerGroups7);

    // additional uncertainty
    QLabel *addedUncertaintyLabel = new QLabel();
    addedUncertaintyLabel->setText(tr("Additional Uncertainty:"));
    addedUncertaintyLabel->setToolTip(tr("Increase in logarithmic standard deviation of EDPs to consider additional sources of uncertainty."));
    uqFormLayout->addRow(addedUncertaintyLabel);

    addedUncertaintyGM = new QLineEdit();
    addedUncertaintyGM->setToolTip(tr("Uncertainty in the shape and amplitude of the target spectrum for scenario-based assessment."));    
    addedUncertaintyGM->setText("0.1");
    addedUncertaintyGM->setAlignment(Qt::AlignRight);
    uqFormLayout->addRow(tr("    Ground Motion"), addedUncertaintyGM);
    
    addedUncertaintyModel = new QLineEdit();
    addedUncertaintyModel->setToolTip(tr("Uncertainty resulting from inaccuracies in component modeling, damping and mass assumptions."));    
    addedUncertaintyModel->setText("0.1");
    addedUncertaintyModel->setAlignment(Qt::AlignRight);
    uqFormLayout->addRow(tr("    Modeling"), addedUncertaintyModel);
    
     QSpacerItem *spacerGroups8 = new QSpacerItem(10,10);
    uqFormLayout->addItem(spacerGroups8);

    // set style
    uqFormLayout->setAlignment(Qt::AlignLeft);
    uqFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    uqFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // loss model dependencies -------------------------------------------------
    QGroupBox * dependencyGroupBox = new QGroupBox("Loss Model Dependencies");
    QFormLayout * dependencyFormLayout = new QFormLayout(dependencyGroupBox);
    
    // add elements

    // perfect correlation label
    QLabel *perfectCorrelationLabel = new QLabel();
    perfectCorrelationLabel->setText(tr("Perfect Correlation in ..."));
    perfectCorrelationLabel->setToolTip(tr("Specify at which organizational level shall the following random variables be correlated. If no correlation is desired, choose Independent."));
    dependencyFormLayout->addRow(perfectCorrelationLabel);

    // quantities
    quantityDep = new QComboBox();
    quantityDep->setToolTip(tr("Some tooltip"));
    quantityDep->addItem("btw. Fragility Groups",0);
    quantityDep->addItem("btw. Performance Groups",1);
    quantityDep->addItem("btw. Floors",2);
    quantityDep->addItem("btw. Directions",3);
    quantityDep->addItem("Independent",4);
    quantityDep->setCurrentIndex(4);
    dependencyFormLayout->addRow(tr("    Component Quantities"), quantityDep);

     QSpacerItem *spacerGroups9 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups9);

    // fragilities
    fragilityDep = new QComboBox();
    fragilityDep->setToolTip(tr("Some tooltip"));
    fragilityDep->addItem("btw. Performance Groups",0);
    fragilityDep->addItem("btw. Floors",1);
    fragilityDep->addItem("btw. Directions",2);
    fragilityDep->addItem("btw. Damage States",3);
    fragilityDep->addItem("Independent",4);
    fragilityDep->addItem("per ATC recommendation",5);
    fragilityDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("    Component Fragilities"), fragilityDep);

     QSpacerItem *spacerGroups10 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups10);

    // reconstruction cost
    costDep = new QComboBox();
    costDep->setToolTip(tr("Some tooltip"));
    costDep->addItem("btw. Fragility Groups",0);
    costDep->addItem("btw. Performance Groups",1);
    costDep->addItem("btw. Floors",2);
    costDep->addItem("btw. Directions",3);
    costDep->addItem("btw. Damage States", 4);
    costDep->addItem("Independent",5);
    costDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("    Reconstruction Costs"), costDep);

    // reconstruction time
    timeDep = new QComboBox();
    timeDep->setToolTip(tr("Some tooltip"));
    timeDep->addItem("btw. Fragility Groups",0);
    timeDep->addItem("btw. Performance Groups",1);
    timeDep->addItem("btw. Floors",2);
    timeDep->addItem("btw. Directions",3);
    timeDep->addItem("btw. Damage States", 4);
    timeDep->addItem("Independent",5);
    timeDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("    Reconstruction Times"), timeDep);

    // rec. cost and time
    costAndTimeDep = new QCheckBox();
    costAndTimeDep->setText("");
    costAndTimeDep->setToolTip(tr("Some tooltip"));
    costAndTimeDep->setChecked(false);
    dependencyFormLayout->addRow(tr("    btw. Rec. Cost and Time"),
                                 costAndTimeDep);

     QSpacerItem *spacerGroups11 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups11);

    // injuries
    injuryDep = new QComboBox();
    injuryDep->setToolTip(tr("Some tooltip"));
    injuryDep->addItem("btw. Fragility Groups",0);
    injuryDep->addItem("btw. Performance Groups",1);
    injuryDep->addItem("btw. Floors",2);
    injuryDep->addItem("btw. Directions",3);
    injuryDep->addItem("btw. Damage States", 4);
    injuryDep->addItem("Independent",5);
    injuryDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("    Injuries"), injuryDep);

    // injuries and fatalities
    injSeverityDep = new QCheckBox();
    injSeverityDep->setText("");
    injSeverityDep->setToolTip(tr("Some tooltip"));
    injSeverityDep->setChecked(false);
    dependencyFormLayout->addRow(tr("    btw. Injuries and Fatalities"),
                                 injSeverityDep);

     QSpacerItem *spacerGroups12 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups12);

    // red tag
    redTagDep = new QComboBox();
    redTagDep->setToolTip(tr("Some tooltip"));
    redTagDep->addItem("btw. Fragility Groups",0);
    redTagDep->addItem("btw. Performance Groups",1);
    redTagDep->addItem("btw. Floors",2);
    redTagDep->addItem("btw. Directions",3);
    redTagDep->addItem("btw. Damage States", 4);
    redTagDep->addItem("Independent",5);
    redTagDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("    Red Tag Probabilities"), redTagDep);

     QSpacerItem *spacerGroups13 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups13);

    // decision variables ------------------------------------------------------
    QGroupBox * decisionGroupBox = new QGroupBox("Decision Variables");
    QFormLayout * decisionFormLayout = new QFormLayout(decisionGroupBox);
    
    // add elements

    // estimate label
    QLabel *decisionVarLabel = new QLabel();
    decisionVarLabel->setText(tr("Estimate ..."));
    decisionVarLabel->setToolTip(tr("Some tooltip"));
    decisionFormLayout->addRow(decisionVarLabel);

    // reconstruction cost
    needRecCost = new QCheckBox();
    needRecCost->setText("");
    needRecCost->setToolTip(tr("Some tooltip"));
    needRecCost->setChecked(true);
    decisionFormLayout->addRow(tr("    Reconstruction Cost"),
                                 needRecCost);

    // reconstruction time
    needRecTime = new QCheckBox();
    needRecTime->setText("");
    needRecTime->setToolTip(tr("Some tooltip"));
    needRecTime->setChecked(true);
    decisionFormLayout->addRow(tr("    Reconstruction Time"),
                                 needRecTime);

    // injuries
    needInjuries = new QCheckBox();
    needInjuries->setText("");
    needInjuries->setToolTip(tr("Some tooltip"));
    needInjuries->setChecked(true);
    decisionFormLayout->addRow(tr("    Injuries"),
                                 needInjuries);

    // red tag
    needRedTag = new QCheckBox();
    needRedTag->setText("");
    needRedTag->setToolTip(tr("Some tooltip"));
    needRedTag->setChecked(true);
    decisionFormLayout->addRow(tr("    Red Tag Probability"),
                                 needRedTag);

    // inhabitants -------------------------------------------------------------
    QGroupBox * inhabitantsGroupBox = new QGroupBox("Inhabitants");
    QFormLayout * inhabitantsFormLayout = new QFormLayout(inhabitantsGroupBox);
    
    // add elements

    // occupancy
    occupancyType = new QComboBox();
    occupancyType->setToolTip(tr("Some tooltip"));
    occupancyType->addItem("Commercial Office",0);
    occupancyType->addItem("Elementary School",1);
    occupancyType->addItem("Middle School",2);
    occupancyType->addItem("High School",3);
    occupancyType->addItem("Healthcare",4);
    occupancyType->addItem("Hospitality",5);
    occupancyType->addItem("Multi-Unit Residential",6);
    occupancyType->addItem("Research Laboratories",7);
    occupancyType->addItem("Retail",8);
    occupancyType->addItem("Warehouse",9);
    occupancyType->setCurrentIndex(7);
    inhabitantsFormLayout->addRow(tr("    Occupancy Type"), occupancyType);

     QSpacerItem *spacerGroups6 = new QSpacerItem(10,10);
    inhabitantsFormLayout->addItem(spacerGroups6);

    // peak population
    peakPopulation = new QLineEdit();
    peakPopulation->setToolTip(tr("A list of the peak population at each floor of the building."));    
    peakPopulation->setText("");
    peakPopulation->setAlignment(Qt::AlignRight);
    inhabitantsFormLayout->addRow(tr("    Peak Population"), peakPopulation);

    // data sources ------------------------------------------------------------
    QGroupBox * resourcesGroupBox = new QGroupBox("Custom Data Sources");
    QFormLayout * resourcesFormLayout = new QFormLayout(resourcesGroupBox);

    QHBoxLayout *fragilityLayout = new QHBoxLayout();

    fragilityFolderPath = new QLineEdit;
    QPushButton *chooseFragility = new QPushButton();
    chooseFragility->setText(tr("Choose"));
    connect(chooseFragility, SIGNAL(clicked()),this,SLOT(chooseFragilityFolder()));
    fragilityLayout->addWidget(fragilityFolderPath);
    fragilityLayout->addWidget(chooseFragility);
    fragilityLayout->setSpacing(1);
    fragilityLayout->setMargin(0);

    resourcesFormLayout->addRow(tr("component damage and loss: "),
                                fragilityLayout);

    QSpacerItem *spacerGroups14 = new QSpacerItem(10,10);
    resourcesFormLayout->addItem(spacerGroups14);

    QHBoxLayout *populationLayout = new QHBoxLayout();

    populationFilePath = new QLineEdit;
    QPushButton *choosePopulation = new QPushButton();
    choosePopulation->setText(tr("Choose"));
    connect(choosePopulation, SIGNAL(clicked()),this,SLOT(choosePopulationFile()));
    populationLayout->addWidget(populationFilePath);
    populationLayout->addWidget(choosePopulation);
    populationLayout->setSpacing(1);
    populationLayout->setMargin(0);

    resourcesFormLayout->addRow(tr("population distribution: "),
                                populationLayout);

    // assemble the widgets-----------------------------------------------------


    mainV1Layout->addWidget(uqGroupBox);
    mainV1Layout->addWidget(decisionGroupBox);
    mainV1Layout->addWidget(inhabitantsGroupBox);
    mainV1Layout->addStretch(1);
    mainV1Layout->setSpacing(10);
    mainV1Layout->setMargin(0);

    mainV2Layout->addWidget(responseGroupBox);
    mainV2Layout->addWidget(damageGroupBox);
    mainV2Layout->addStretch(1);
    mainV2Layout->setSpacing(10);
    mainV2Layout->setMargin(0);

    mainV3Layout->addWidget(dependencyGroupBox);
    mainV3Layout->addWidget(resourcesGroupBox);
    mainV3Layout->addStretch(1);
    mainV3Layout->setSpacing(10);
    mainV3Layout->setMargin(0);


    mainHLayout->addLayout(mainV1Layout, 0);
    mainHLayout->addLayout(mainV2Layout, 0);
    mainHLayout->addLayout(mainV3Layout, 0);
    mainHLayout->addStretch(1);
    mainHLayout->setSpacing(10);
    mainHLayout->setMargin(0);

    mainLayout->addLayout(titleLayout);
    mainLayout->addItem(spacerTop1);
    mainLayout->addLayout(mainHLayout, 0);
    mainLayout->addStretch(1);
    mainLayout->setSpacing(10);
    //mainLayout->setMargin(0);

   // mainLayout->addWidget(responseGroupBox);
    this->setLayout(mainLayout);
}

QString
P58GeneralSettingsContainer::getFragilityFolder(){
    return fragilityFolderPath->text();
}

QString
P58GeneralSettingsContainer::getPopulationFile(){
    return populationFilePath->text();
}

int
P58GeneralSettingsContainer::setFragilityFolder(QString fragilityFolder){
    fragilityFolderPath->setText(fragilityFolder);
    return 0;
}

void
P58GeneralSettingsContainer::chooseFragilityFolder(void) {
    QString fragilityFolder;
    fragilityFolder=QFileDialog::getExistingDirectory(this,tr("Select Folder"),
        "C://");
    int ok = this->setFragilityFolder(fragilityFolder);
}

int
P58GeneralSettingsContainer::setPopulationFile(QString populationFile){
    populationFilePath->setText(populationFile);
    return 0;
}

void
P58GeneralSettingsContainer::choosePopulationFile(void) {
    QString populationFile;
    populationFile=QFileDialog::getOpenFileName(this,tr("Select File"),
        "C://", "All files (*.*)");
    int ok = this->setPopulationFile(populationFile);
}

P58GeneralSettingsContainer::~P58GeneralSettingsContainer()
{}

bool P58GeneralSettingsContainer::outputToJSON(QJsonObject &outputObject) {

    QJsonObject UQ;
    QJsonObject decVars;
    QJsonObject response;
    QJsonObject damage;
    QJsonObject dependencies;
    QJsonObject inhabitants;
    QJsonObject dataSources;
    
    // UQ ---------------------------------------------------------------------

    UQ["Realizations"] = realizationsValue->text();
    
    QJsonObject addUncertainty;

    addUncertainty["GroundMotion"] = addedUncertaintyGM->text();
    addUncertainty["Modeling"] = addedUncertaintyModel->text();

    UQ["AdditionalUncertainty"] =  addUncertainty;

    outputObject["UncertaintyQuantification"] = UQ;

    // decision vars ----------------------------------------------------------

    decVars["ReconstructionCost"] = needRecCost->isChecked();
    decVars["ReconstructionTime"] = needRecTime->isChecked();
    decVars["Injuries"] = needInjuries->isChecked();
    decVars["RedTag"] = needRedTag->isChecked();

    outputObject["DecisionVariables"] = decVars;

    // building response ------------------------------------------------------

    response["YieldDriftRatio"] = yieldDriftValue->text();

    QJsonObject detLims;

    detLims["PID"] = driftDetLim->text();
    detLims["PFA"] = accDetLim->text();

    response["DetectionLimits"] = detLims;

    outputObject["BuildingResponse"] = response;

    // building damage --------------------------------------------------------

    damage["ReplacementCost"] = replacementCostValue->text();
    damage["ReplacementTime"] = replacementTimeValue->text();

    QJsonObject irrepDrift;

    irrepDrift["Median"] = irrepResDriftMedian->text();
    irrepDrift["Sig"] = irrepResDriftStd->text();

    damage["IrrepairableResidualDrift"] = irrepDrift;

    QJsonObject colLims;

    colLims["PID"] = driftColLim->text();
    colLims["PFA"] = accColLim->text();

    damage["CollapseLimits"] = colLims;

    outputObject["BuildingDamage"] = damage;

    // loss model dependencies ------------------------------------------------

    dependencies["Quantities"] = quantityDep->currentText();
    dependencies["Fragilities"] = fragilityDep->currentText();
    dependencies["ReconstructionCosts"] = costDep->currentText();
    dependencies["ReconstructionTimes"] = timeDep->currentText();
    dependencies["CostAndTime"] = costAndTimeDep->isChecked();
    dependencies["Injuries"] = injuryDep->currentText();
    dependencies["InjurySeverities"] = injSeverityDep->isChecked();
    dependencies["RedTagProbabilities"] = redTagDep->currentText();

    outputObject["LossModelDependencies"] = dependencies;

    // inhabitants -------------------------------------------------------------

    inhabitants["OccupancyType"] = occupancyType->currentText();
    inhabitants["PeakPopulation"] = peakPopulation->text();

    outputObject["Inhabitants"] = inhabitants;

    // data sources ------------------------------------------------------------

    dataSources["ComponentDataFolder"] = fragilityFolderPath->text();
    dataSources["PopulationDataFile"] = populationFilePath->text();

    outputObject["DataSources"] = dataSources;

    return 0;
}

bool P58GeneralSettingsContainer::inputFromJSON(QJsonObject & inputObject) {

    // UQ ---------------------------------------------------------------------

    QJsonObject UQ = inputObject["UncertaintyQuantification"].toObject();
 
    realizationsValue->setText(UQ["Realizations"].toString());
    
    QJsonObject addUncertainty;
    addUncertainty = UQ["AdditionalUncertainty"].toObject();

    addedUncertaintyGM->setText(addUncertainty["GroundMotion"].toString());
    addedUncertaintyModel->setText(addUncertainty["Modeling"].toString()); 

    // decision vars ----------------------------------------------------------

    QJsonObject decVars = inputObject["DecisionVariables"].toObject();

    needRecCost->setChecked(decVars["ReconstructionCost"].toBool());
    needRecTime->setChecked(decVars["ReconstructionTime"].toBool());
    needInjuries->setChecked(decVars["Injuries"].toBool());
    needRedTag->setChecked(decVars["RedTag"].toBool());

    // building response ------------------------------------------------------

    QJsonObject response = inputObject["BuildingResponse"].toObject();

    yieldDriftValue->setText(response["YieldDriftRatio"].toString());

    QJsonObject detLims;
    detLims = response["DetectionLimits"].toObject();

    driftDetLim->setText(detLims["PID"].toString());
    accDetLim->setText(detLims["PFA"].toString());

    // building damage --------------------------------------------------------

    QJsonObject damage = inputObject["BuildingDamage"].toObject();

    replacementCostValue->setText(damage["ReplacementCost"].toString());
    replacementTimeValue->setText(damage["ReplacementTime"].toString());

    QJsonObject irrepDrift;
    irrepDrift = damage["IrrepairableResidualDrift"].toObject();

    irrepResDriftMedian->setText(irrepDrift["Median"].toString());
    irrepResDriftStd->setText(irrepDrift["Sig"].toString());

    QJsonObject colLims;
    colLims = damage["CollapseLimits"].toObject();

    driftColLim->setText(colLims["PID"].toString());
    accColLim->setText(colLims["PFA"].toString());

    // loss model dependencies ------------------------------------------------

    QJsonObject dependencies = inputObject["LossModelDependencies"].toObject();

    quantityDep->setCurrentText(dependencies["Quantities"].toString());
    fragilityDep->setCurrentText(dependencies["Fragilities"].toString());
    costDep->setCurrentText(dependencies["ReconstructionCosts"].toString());
    timeDep->setCurrentText(dependencies["ReconstructionTimes"].toString());
    costAndTimeDep->setChecked(dependencies["CostAndTime"].toBool());
    injuryDep->setCurrentText(dependencies["Injuries"].toString());
    injSeverityDep->setChecked(dependencies["InjurySeverities"].toBool());
    redTagDep->setCurrentText(dependencies["RedTagProbabilities"].toString());

    // inhabitants ------------------------------------------------------------

    QJsonObject inhabitants = inputObject["Inhabitants"].toObject();

    occupancyType->setCurrentText(inhabitants["OccupancyType"].toString());
    peakPopulation->setText(inhabitants["PeakPopulation"].toString());

    // data sources -----------------------------------------------------------

    QJsonObject dataSources = inputObject["DataSources"].toObject();

    fragilityFolderPath->setText(dataSources["ComponentDataFolder"].toString());
    populationFilePath->setText(dataSources["PopulationDataFile"].toString());

    return 0;

}

