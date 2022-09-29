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

#include "PelicunPreferencesContainer.h"

PelicunPreferencesContainer::PelicunPreferencesContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    int maxWidth = 333;

    mainLayout = new QVBoxLayout();

    mainLayout->addStretch(1);
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

QString
PelicunPreferencesContainer::getEDPFile(){
    return EDPFilePath->text();
}

int
PelicunPreferencesContainer::setEDPFile(QString EDPFile){
    EDPFilePath->setText(EDPFile);
    return 0;
}

void
PelicunPreferencesContainer::chooseEDPFile(void) {
    QString EDPFile;
    EDPFile=QFileDialog::getOpenFileName(this,tr("Select EDP File"),
        "C://", "All files (*.*)");
    this->setEDPFile(EDPFile);
}

QString
PelicunPreferencesContainer::getPopulationFile(){
    return populationFilePath->text();
}

int
PelicunPreferencesContainer::setPopulationFile(QString populationFile){
    populationFilePath->setText(populationFile);
    return 0;
}

void
PelicunPreferencesContainer::choosePopulationFile(void) {
    QString populationFile;
    populationFile=QFileDialog::getOpenFileName(this,tr("Select Population File"),
        "C://", "All files (*.*)");
    this->setPopulationFile(populationFile);
}

PelicunPreferencesContainer::~PelicunPreferencesContainer()
{}

bool PelicunPreferencesContainer::outputToJSON(QJsonObject &outputObject) {

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

bool PelicunPreferencesContainer::inputFromJSON(QJsonObject & inputObject) {

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

