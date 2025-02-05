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

// Written: adamzs

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
#include <SectionTitle.h>

#include "PelicunOutputsContainer.h"

PelicunOutputsContainer::PelicunOutputsContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    int maxWidth = 400;
    this->setMaximumWidth(maxWidth);

    gridLayout = new QGridLayout();

    // General Output Settings -------------------------------------------------

    QGroupBox *OutputGB = new QGroupBox("");
    OutputGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loGenV = new QVBoxLayout(OutputGB);

    // Output Format

    QHBoxLayout *outputFormatLayout = new QHBoxLayout();
    outputFormatLayout->setContentsMargins(0,0,0,0);;

    QLabel *lblOutputFormat = new QLabel();
    lblOutputFormat->setText("Format:");
    outputFormatLayout->addWidget(lblOutputFormat);

    outputCSV = new QCheckBox();
    outputCSV->setText("CSV");
    outputCSV->setTristate(false);
    outputCSV->setToolTip("Save output data in CSV format.");
    outputCSV->setChecked(true);
    outputFormatLayout->addWidget(outputCSV);

    outputJSON = new QCheckBox();
    outputJSON->setText("JSON");
    outputJSON->setTristate(false);
    outputJSON->setToolTip("Save output data in JSON format.");
    outputJSON->setChecked(false);
    outputFormatLayout->addWidget(outputJSON);

    outputFormatLayout->addStretch();
    outputFormatLayout->setSpacing(10);

    loGenV->addLayout(outputFormatLayout);

    // add line
    QFrame *lineGV = new QFrame();
    lineGV->setFrameShape(QFrame::HLine);
    lineGV->setFrameShadow(QFrame::Sunken);
    loGenV->addWidget(lineGV);

    // Asset outputs

    QLabel *lblAssetOutputs = new QLabel();
    lblAssetOutputs->setText("Asset:");
    loGenV->addWidget(lblAssetOutputs);

    assetOutSample = new QCheckBox();
    assetOutSample->setText("Sample");
    assetOutSample->setTristate(false);
    assetOutSample->setToolTip("Save component quantities for every "
                                "realization.");
    assetOutSample->setChecked(true);
    loGenV->addWidget(assetOutSample);

    assetOutStats = new QCheckBox();
    assetOutStats->setText("Statistics");
    assetOutStats->setTristate(false);
    assetOutStats->setToolTip("Save statistics that summarize component "
                              "quantities across all realizations.");
    assetOutStats->setChecked(true);
    loGenV->addWidget(assetOutStats);    
    
    loGenV->addStretch();

    // add line
    QFrame *lineGV2 = new QFrame();
    lineGV2->setFrameShape(QFrame::HLine);
    lineGV2->setFrameShadow(QFrame::Sunken);
    loGenV->addWidget(lineGV2);

    // Demand outputs

    QLabel *lblDemandOutputs = new QLabel();
    lblDemandOutputs->setText("Demands:");
    loGenV->addWidget(lblDemandOutputs);

    demandOutSample = new QCheckBox();
    demandOutSample->setText("Sample");
    demandOutSample->setTristate(false);
    demandOutSample->setToolTip("Save demands in every realization.");
    demandOutSample->setChecked(true);
    loGenV->addWidget(demandOutSample);

    demandOutStats = new QCheckBox();
    demandOutStats->setText("Statistics");
    demandOutStats->setTristate(false);
    demandOutStats->setToolTip("Save statistics that summarize demands across "
                               "all realizations.");
    demandOutStats->setChecked(true);
    loGenV->addWidget(demandOutStats);  

    // add line
    QFrame *lineGV3 = new QFrame();
    lineGV3->setFrameShape(QFrame::HLine);
    lineGV3->setFrameShadow(QFrame::Sunken);
    loGenV->addWidget(lineGV3);

    // Damage outputs

    QLabel *lblDamageOutputs = new QLabel();
    lblDamageOutputs->setText("Damage:");
    loGenV->addWidget(lblDamageOutputs);

    damageOutSample = new QCheckBox();
    damageOutSample->setText("Sample");
    damageOutSample->setTristate(false);
    damageOutSample->setToolTip("Save the quantity of components in each "
                                "damage state in each performance group in "
                                "every realization.");
    damageOutSample->setChecked(true);
    loGenV->addWidget(damageOutSample);

    damageOutStats = new QCheckBox();
    damageOutStats->setText("Statistics");
    damageOutStats->setTristate(false);
    damageOutStats->setToolTip("Save statistics that summarize the quantity of "
                               "components in each damage state in each "
                               "performance group across all realizations.");
    damageOutStats->setChecked(true);
    loGenV->addWidget(damageOutStats);

    damageOutGrpSample = new QCheckBox();
    damageOutGrpSample->setText("Grouped Sample");
    damageOutGrpSample->setTristate(false);
    damageOutGrpSample->setToolTip("Aggregate damage across performance groups "
                                   "to save the quantity of damage in each "
                                   "damage state of each component in every "
                                   "realization.");
    damageOutGrpSample->setChecked(true);
    loGenV->addWidget(damageOutGrpSample);

    damageOutGrpStats = new QCheckBox();
    damageOutGrpStats->setText("Grouped Statistics");
    damageOutGrpStats->setTristate(false);
    damageOutGrpStats->setToolTip("Aggregate damage across performance groups"
                                  "and save statistics that summarize the "
                                  "quantity of damage in each damage state of "
                                  "each component across all realizations.");
    damageOutGrpStats->setChecked(true);
    loGenV->addWidget(damageOutGrpStats);   

    // add line
    QFrame *lineGV4 = new QFrame();
    lineGV4->setFrameShape(QFrame::HLine);
    lineGV4->setFrameShadow(QFrame::Sunken);
    loGenV->addWidget(lineGV4);

    // Repair Consequence outputs

    QLabel *lblRepairOutputs = new QLabel();
    lblRepairOutputs->setText("Repair Consequences:");
    loGenV->addWidget(lblRepairOutputs);

    repairOutSample = new QCheckBox();
    repairOutSample->setText("Sample");
    repairOutSample->setTristate(false);
    repairOutSample->setToolTip("Save the repair consequence in each "
                                "damage state in each performance group in "
                                "every realization.");
    repairOutSample->setChecked(true);
    loGenV->addWidget(repairOutSample);

    repairOutStats = new QCheckBox();
    repairOutStats->setText("Statistics");
    repairOutStats->setTristate(false);
    repairOutStats->setToolTip("Save statistics that summarize the repair "
                               "consequence in each damage state in each "
                               "performance group across all realizations.");
    repairOutStats->setChecked(true);
    loGenV->addWidget(repairOutStats);

    repairOutGrpSample = new QCheckBox();
    repairOutGrpSample->setText("Grouped Sample");
    repairOutGrpSample->setTristate(false);
    repairOutGrpSample->setToolTip("Aggregate repair consequences across "
                                   "performance groups and damage states and "
                                   "save the total consequence for each "
                                   "component in every realization.");
    repairOutGrpSample->setChecked(true);
    loGenV->addWidget(repairOutGrpSample);

    repairOutGrpStats = new QCheckBox();
    repairOutGrpStats->setText("Grouped Statistics");
    repairOutGrpStats->setTristate(false);
    repairOutGrpStats->setToolTip("Aggregate repair consequences across "
                                  "performance groups and save statistics that "
                                  "summarize the total consequence for eacah "
                                  "component across all realizations.");
    repairOutGrpStats->setChecked(true);
    loGenV->addWidget(repairOutGrpStats);  
    
    repairOutAggSample = new QCheckBox();
    repairOutAggSample->setText("Aggregate Sample");
    repairOutAggSample->setTristate(false);
    repairOutAggSample->setToolTip("Aggregate repair consequences across the "
                                   "entire building and save the total "
                                   "consequence in every realization.");
    repairOutAggSample->setChecked(true);
    loGenV->addWidget(repairOutAggSample);

    repairOutAggStats = new QCheckBox();
    repairOutAggStats->setText("Aggregate Statistics");
    repairOutAggStats->setTristate(false);
    repairOutAggStats->setToolTip("Aggregate repair consequences across the "
                                  "entire building and save statistics that "
                                  "summarize the total consequence across all "
                                  "realizations.");
    repairOutAggStats->setChecked(true);
    loGenV->addWidget(repairOutAggStats);  

    loGenV->addStretch();

    // assemble the widgets ---------------------------------------------------

    gridLayout->addWidget(OutputGB,0,0);
    gridLayout->setRowStretch(1, 1);

    this->setLayout(gridLayout);
}

PelicunOutputsContainer::~PelicunOutputsContainer()
{}

bool PelicunOutputsContainer::outputToJSON(QJsonObject &outputObject) {

    QJsonObject outputData;

    QJsonObject formatData;
    formatData["CSV"] = outputCSV->isChecked();
    formatData["JSON"] = outputJSON->isChecked();
    outputData["Format"] = formatData;

    QJsonObject assetData;
    assetData["Sample"] = assetOutSample->isChecked();
    assetData["Statistics"] = assetOutStats->isChecked();
    outputData["Asset"] = assetData;

    QJsonObject demandData;
    demandData["Sample"] = demandOutSample->isChecked();
    demandData["Statistics"] = demandOutStats->isChecked();
    outputData["Demand"] = demandData;

    QJsonObject damageData;
    damageData["Sample"] = damageOutSample->isChecked();
    damageData["Statistics"] = damageOutStats->isChecked();
    damageData["GroupedSample"] = damageOutGrpSample->isChecked();
    damageData["GroupedStatistics"] = damageOutGrpStats->isChecked();
    outputData["Damage"] = damageData;

    QJsonObject lossData;

    QJsonObject repairData;
    repairData["Sample"] = repairOutSample->isChecked();
    repairData["Statistics"] = repairOutStats->isChecked();
    repairData["GroupedSample"] = repairOutGrpSample->isChecked();
    repairData["GroupedStatistics"] = repairOutGrpStats->isChecked();
    repairData["AggregateSample"] = repairOutAggSample->isChecked();
    repairData["AggregateStatistics"] = repairOutAggStats->isChecked();

    int no_repair = repairData["Sample"].toInt() + \
                    repairData["Statistics"].toInt() + \
                    repairData["GroupedSample"].toInt() + \
                    repairData["GroupedStatistics"].toInt() + \
                    repairData["AggregateSample"].toInt() + \
                    repairData["AggregateStatistics"].toInt();

    if (no_repair > 0){

        lossData["Repair"] = repairData;

        outputData["Loss"] = lossData;
    }

    outputObject["Outputs"] = outputData;

    return 0;
}

bool PelicunOutputsContainer::inputFromJSON(QJsonObject & inputObject) {

    bool result = false;

    QJsonObject outputData = inputObject["Outputs"].toObject();

    if (outputData.contains("Format")) {
        QJsonObject formatData = outputData["Format"].toObject();

        if (formatData.contains("CSV")){
            outputCSV->setChecked(formatData["CSV"].toBool());
        }
        if (formatData.contains("JSON")){
            outputJSON->setChecked(formatData["JSON"].toBool());
        }
    }

    if (outputData.contains("Asset")) {
        QJsonObject assetData = outputData["Asset"].toObject();

        if (assetData.contains("Sample")){
            assetOutSample->setChecked(assetData["Sample"].toBool());
        }
        if (assetData.contains("Statistics")){
            assetOutStats->setChecked(assetData["Statistics"].toBool());
        }
    }

    if (outputData.contains("Demand")) {
        QJsonObject demandData = outputData["Demand"].toObject();

        if (demandData.contains("Sample")){
            demandOutSample->setChecked(demandData["Sample"].toBool());
        }
        if (demandData.contains("Statistics")){
            demandOutStats->setChecked(demandData["Statistics"].toBool());
        }
    }

    if (outputData.contains("Damage")) {
        QJsonObject damageData = outputData["Damage"].toObject();

        if (damageData.contains("Sample")){
            damageOutSample->setChecked(damageData["Sample"].toBool());
        }
        if (damageData.contains("Statistics")){
            damageOutStats->setChecked(damageData["Statistics"].toBool());
        }
        if (damageData.contains("GroupedSample")){
            damageOutGrpSample->setChecked(
                damageData["GroupedSample"].toBool());
        }
        if (damageData.contains("GroupedStatistics")){
            damageOutGrpStats->setChecked(
                damageData["GroupedStatistics"].toBool());
        }
    }

    if (outputData.contains("Loss")) {
        QJsonObject lossData = outputData["Loss"].toObject();

        if (lossData.contains("BldgRepair") || lossData.contains("Repair")) {

            QJsonObject repairData;
            if (lossData.contains("BldgRepair")) {
                // for the sake of backwards compatibility
                repairData = lossData["BldgRepair"].toObject();
            } else {
                repairData = lossData["Repair"].toObject();
            }

            if (repairData.contains("Sample")){
                repairOutSample->setChecked(
                    repairData["Sample"].toBool());
            }
            if (repairData.contains("Statistics")){
                repairOutStats->setChecked(
                    repairData["Statistics"].toBool());
            }
            if (repairData.contains("GroupedSample")){
                repairOutGrpSample->setChecked(
                    repairData["GroupedSample"].toBool());
            }
            if (repairData.contains("GroupedStatistics")){
                repairOutGrpStats->setChecked(
                    repairData["GroupedStatistics"].toBool());
            }
            if (repairData.contains("AggregateSample")){
                repairOutAggSample->setChecked(
                    repairData["AggregateSample"].toBool());
            }
            if (repairData.contains("AggregateStatistics")){
                repairOutAggStats->setChecked(
                    repairData["AggregateStatistics"].toBool());
            }
        }
    }

    result = true;

    return result;
}

