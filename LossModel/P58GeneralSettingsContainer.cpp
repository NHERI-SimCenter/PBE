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

    int maxWidth = 333;

    mainLayout = new QVBoxLayout();
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *mainV1Layout = new QVBoxLayout();
    QVBoxLayout *mainV2Layout = new QVBoxLayout();
    QVBoxLayout *mainV3Layout = new QVBoxLayout();
    //QVBoxLayout *mainV4Layout = new QVBoxLayout();


    // title -------------------------------------------------------------------
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title = new SectionTitle();
    title->setText(tr("General Settings"));
    title->setMinimumWidth(250);

    titleLayout->addWidget(title);
    titleLayout->addStretch();

    // response model ----------------------------------------------------------
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
    EDP_dataLayout->setMargin(0);
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

    QSpacerItem *spacerGroups1 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups1);

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

    // set style
    responseFormLayout->setAlignment(Qt::AlignLeft);
    responseFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    responseFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    responseGroupBox->setLayout(responseFormLayout);


    // damage model -----------------------------------------------------------
    QGroupBox * damageGroupBox = new QGroupBox("Damage Model");
    damageGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * damageFormLayout = new QFormLayout(damageGroupBox);

    // irreparable residual drift
    QLabel *irrepResDriftLabel = new QLabel();
    irrepResDriftLabel->setText(tr("Irreparable Residual Drift:"));
    damageFormLayout->addRow(irrepResDriftLabel);

    // yield drift
    yieldDriftValue = new QLineEdit();
    yieldDriftValue->setToolTip(tr("Interstory drift ratio corresponding to significant yielding in the structure \n"
                                   "(i.e. when the forces in the main components of the lateral load resisting \n"
                                   "system reach the plastic capacity of the components) in the structure."));
    yieldDriftValue->setText("0.01");
    yieldDriftValue->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Yield Drift Ratio"), yieldDriftValue);

    irrepResDriftMedian = new QLineEdit();
    irrepResDriftMedian->setToolTip(tr("Median of the residual drift distribution conditioned on irreparable \n"
                                       "damage in the building.\n"
                                       "If left empty, all non-collapse results are considered repairable."));
    irrepResDriftMedian->setText("");
    irrepResDriftMedian->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Median"), irrepResDriftMedian);

    irrepResDriftStd = new QLineEdit();
    irrepResDriftStd->setToolTip(tr("Logarithmic standard deviation of the residual drift distribution \n"
                                    "conditioned on irreparable damage in the building."));
    irrepResDriftStd->setText("");
    irrepResDriftStd->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Log Standard Dev"), irrepResDriftStd);

    QSpacerItem *spacerGroups4 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups4);

    // collapse
    QLabel *collapseLabel = new QLabel();
    collapseLabel->setText(tr("Collapse Probability:"));
    damageFormLayout->addRow(collapseLabel);

    // approach
    collProbApproach = new QComboBox();
    collProbApproach->setToolTip(tr("Specifies how to define the probability of collapse:\n"
                                    "estimated - use sampled EDPs and the collapse limits below\n"
                                    "prescribed - use a prescribed (user-defined) value"));
    collProbApproach->addItem("estimated",0);
    collProbApproach->addItem("prescribed",1);

    collProbApproach->setCurrentIndex(0);
    damageFormLayout->addRow(tr("    Approach:"), collProbApproach);

    // prescribed value
    colProbValue = new QLineEdit();
    colProbValue->setToolTip(tr("User-defined probability of collapse\n"
                                "Only used when Approach is set to prescribed above"));
    colProbValue->setText("");
    colProbValue->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Prescribed value:"), colProbValue);

    colBasis = new QComboBox();
    colBasis ->setToolTip(tr("Basis of collapse probability estimation:\n"
                             "sampled EDP - re-sampled EDP values\n"
                             "raw EDP - empirical (input) EDP values"));
    colBasis ->addItem("sampled EDP",0);
    colBasis ->addItem("raw EDP",1);

    colBasis ->setCurrentIndex(0);
    damageFormLayout->addRow(tr("    Basis:"), colBasis );

    QSpacerItem *spacerGroups6 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups6);

    // collapse limits
    QLabel *colLimLabel = new QLabel();
    colLimLabel->setText(tr("Collapse Limits:"));
    damageFormLayout->addRow(colLimLabel);

    driftColLim = new QLineEdit();
    driftColLim->setToolTip(tr("Peak interstory drift ratio that corresponds to \n"
                               "the collapse of the building."));
    driftColLim->setText("0.15");
    driftColLim->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Interstory Drift"), driftColLim);

    accColLim = new QLineEdit();
    accColLim->setToolTip(tr("Peak floor acceleration that corresponds to \n"
                             "the collapse of the building."));
    accColLim->setText("");
    accColLim->setAlignment(Qt::AlignRight);
    damageFormLayout->addRow(tr("    Floor Acceleration"), accColLim);

    QSpacerItem *spacerGroups5 = new QSpacerItem(10,10);
    damageFormLayout->addItem(spacerGroups5);

    // set style
    damageFormLayout->setAlignment(Qt::AlignLeft);
    damageFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    damageFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // loss model -------------------------------------------------------------
    QGroupBox * lossGroupBox = new QGroupBox("Loss Model");
    lossGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * lossFormLayout = new QFormLayout(lossGroupBox);

    // replacement cost
    replacementCostValue = new QLineEdit();
    replacementCostValue->setToolTip(tr("The cost to replace the building with a new one.\n"
                                        "Note: monetary units used in the replacement cost\n"
                                        "must be consistent with those used in the consequence\n"
                                        "functions. \n"
                                        "Consequences in FEMA P58 are based on 2011 US dollars"));
    replacementCostValue->setText("");
    replacementCostValue->setAlignment(Qt::AlignRight);
    lossFormLayout->addRow(tr("    Replacement Cost"), replacementCostValue);

    // replacement cost
    replacementTimeValue = new QLineEdit();
    replacementTimeValue->setToolTip(tr("The time it takes to replace the building with a new one.\n"
                                        "Note: the time unit used for replacement time must be \n"
                                        "consistent with those used in the consequence functions.\n"
                                        "Consequence in FEMA P58 are based on days in the first edition\n"
                                        "and worker-days in the second edition. We use the second\n"
                                        "edition here by default."));
    replacementTimeValue->setText("");
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

    // red tag
    needRedTag = new QCheckBox();
    needRedTag->setText("");
    needRedTag->setChecked(true);

    QHBoxLayout *injAndRedLayout = new QHBoxLayout();
    QLabel *redTagLabel = new QLabel();
    redTagLabel->setText(tr("    Red Tag Probability"));
    injAndRedLayout->addWidget(needInjuries);
    injAndRedLayout->setSpacing(5);
    injAndRedLayout->addWidget(redTagLabel);
    injAndRedLayout->addWidget(needRedTag);

    lossFormLayout->addRow(tr("    Injuries"),
                                 injAndRedLayout);

    QSpacerItem *spacerGroups2 = new QSpacerItem(10,10);
    lossFormLayout->addItem(spacerGroups2);

    // inhabitants
    QLabel *inhabitantLabel = new QLabel();
    inhabitantLabel->setText(tr("Inhabitants: "));
    lossFormLayout->addRow(inhabitantLabel);

    // occupancy
    occupancyType = new QComboBox();
    occupancyType->setToolTip(tr("Occupancy defines the function of the building."));
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
    lossFormLayout->addRow(tr("    Occupancy Type"), occupancyType);

    // peak population
    peakPopulation = new QLineEdit();
    peakPopulation->setToolTip(tr("A list of the peak population at each floor of the building.\n"
                                  "If a single number is provided, the population is evenly \n"
                                  "distributed across every floor.\n"));
    peakPopulation->setText("");
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
    populationLayout->setMargin(0);

    lossFormLayout->addRow(tr("    Custom distribution: "),
                               populationLayout);

    // set style
    lossFormLayout->setAlignment(Qt::AlignLeft);
    lossFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    lossFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    // assemble the widgets-----------------------------------------------------
    mainV1Layout->addWidget(responseGroupBox);
    mainV1Layout->addStretch(1);
    mainV1Layout->setSpacing(10);
    mainV1Layout->setMargin(0);

    mainV2Layout->addWidget(damageGroupBox);
    mainV2Layout->addStretch(1);
    mainV2Layout->setSpacing(10);
    mainV2Layout->setMargin(0);

    mainV3Layout->addWidget(lossGroupBox);
    mainV3Layout->addStretch(1);
    mainV3Layout->setSpacing(10);
    mainV3Layout->setMargin(0);

    mainHLayout->addLayout(mainV1Layout, 1);
    mainHLayout->addLayout(mainV2Layout, 1);
    mainHLayout->addLayout(mainV3Layout, 1);
    mainHLayout->addStretch();
    mainHLayout->setSpacing(10);
    mainHLayout->setMargin(0);

    mainLayout->addLayout(titleLayout);
    mainLayout->addLayout(mainHLayout, 0);
    mainLayout->addStretch(1);
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

QString
P58GeneralSettingsContainer::getEDPFile(){
    return EDPFilePath->text();
}

int
P58GeneralSettingsContainer::setEDPFile(QString EDPFile){
    EDPFilePath->setText(EDPFile);
    return 0;
}

void
P58GeneralSettingsContainer::chooseEDPFile(void) {
    QString EDPFile;
    EDPFile=QFileDialog::getOpenFileName(this,tr("Select EDP File"),
        "C://", "All files (*.*)");
    this->setEDPFile(EDPFile);
}

QString
P58GeneralSettingsContainer::getPopulationFile(){
    return populationFilePath->text();
}

int
P58GeneralSettingsContainer::setPopulationFile(QString populationFile){
    populationFilePath->setText(populationFile);
    return 0;
}

void
P58GeneralSettingsContainer::choosePopulationFile(void) {
    QString populationFile;
    populationFile=QFileDialog::getOpenFileName(this,tr("Select Population File"),
        "C://", "All files (*.*)");
    this->setPopulationFile(populationFile);
}

P58GeneralSettingsContainer::~P58GeneralSettingsContainer()
{}

bool P58GeneralSettingsContainer::outputToJSON(QJsonObject &outputObject) {

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

    QJsonObject irrepDrift;    
    irrepDrift["Median"] = irrepResDriftMedian->text();
    irrepDrift["Beta"] = irrepResDriftStd->text();
    irrepDrift["YieldDriftRatio"] = yieldDriftValue->text();
    if ((irrepDrift["Median"] != "") && (irrepDrift["Beta"] != "")) {
        damageModel["IrreparableResidualDrift"] = irrepDrift;
    }

    QJsonObject collProb;
    if (collProbApproach->currentText() == "prescribed") {
        collProb["Value"] = colProbValue->text();
    } else {
        collProb["Value"] = "estimated";
        collProb["BasisOfEstimate"] = colBasis->currentText();
    }
    damageModel["CollapseProbability"] = collProb;

    QJsonObject colLims;
    colLims["PID"] = driftColLim->text();
    colLims["PFA"] = accColLim->text();
    damageModel["CollapseLimits"] = colLims;

    outputObject["DamageModel"] = damageModel;

    // Loss -------------------------------------------------------------------
    QJsonObject lossModel;

    lossModel["ReplacementCost"] = replacementCostValue->text();
    lossModel["ReplacementTime"] = replacementTimeValue->text();

    QJsonObject decVars;
    decVars["ReconstructionCost"] = needRecCost->isChecked();
    decVars["ReconstructionTime"] = needRecTime->isChecked();
    decVars["Injuries"] = needInjuries->isChecked();
    decVars["RedTag"] = needRedTag->isChecked();
    lossModel["DecisionVariables"] = decVars;

    QJsonObject inhabitants;
    inhabitants["OccupancyType"] = occupancyType->currentText();
    inhabitants["PeakPopulation"] = peakPopulation->text();
    if (populationFilePath->text() != "")
        inhabitants["PopulationDataFile"] = populationFilePath->text();
    lossModel["Inhabitants"] = inhabitants;

    outputObject["LossModel"] = lossModel;

    return 0;
}

bool P58GeneralSettingsContainer::inputFromJSON(QJsonObject & inputObject) {

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

    QJsonObject irrepDrift = damageModel["IrreparableResidualDrift"].toObject();
    irrepResDriftMedian->setText(irrepDrift["Median"].toString());
    irrepResDriftStd->setText(irrepDrift["Beta"].toString());
    yieldDriftValue->setText(irrepDrift["YieldDriftRatio"].toString());

    QJsonObject collProb = damageModel["CollapseProbability"].toObject();
    if (collProb["Value"].toString() == "estimated") {
        collProbApproach->setCurrentText("estimated");
        colProbValue->setText("");
        colBasis->setCurrentText(collProb["BasisOfEstimate"].toString());
    } else {
        collProbApproach->setCurrentText("prescribed");
        colProbValue->setText(collProb["Value"].toString());
    }

    QJsonObject colLims = damageModel["CollapseLimits"].toObject();
    driftColLim->setText(colLims["PID"].toString());
    accColLim->setText(colLims["PFA"].toString());

    // Loss -------------------------------------------------------------------
    QJsonObject lossModel = inputObject["LossModel"].toObject();

    replacementCostValue->setText(lossModel["ReplacementCost"].toString());
    replacementTimeValue->setText(lossModel["ReplacementTime"].toString());

    QJsonObject decVars = lossModel["DecisionVariables"].toObject();
    needRecCost->setChecked(decVars["ReconstructionCost"].toBool());
    needRecTime->setChecked(decVars["ReconstructionTime"].toBool());
    needInjuries->setChecked(decVars["Injuries"].toBool());
    needRedTag->setChecked(decVars["RedTag"].toBool());

    QJsonObject inhabitants = lossModel["Inhabitants"].toObject();
    occupancyType->setCurrentText(inhabitants["OccupancyType"].toString());
    peakPopulation->setText(inhabitants["PeakPopulation"].toString());
    if (inhabitants.contains("PopulationDataFile"))
        populationFilePath->setText(inhabitants["PopulationDataFile"].toString());
    else
        populationFilePath->setText(tr(""));

    return 0;
}

