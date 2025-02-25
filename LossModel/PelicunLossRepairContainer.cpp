/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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

#include <QProcess>
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
#include <QJsonDocument>
#include <SectionTitle.h>
#include <QStringListModel>

#include <RunPythonInThread.h>

#include "SimCenterPreferences.h"
#include "PelicunLossRepairContainer.h"

PelicunLossRepairContainer::PelicunLossRepairContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    int maxWidth = 800;
    this->setMaximumWidth(maxWidth);

    // initialize the compDB Map
    compDB = new QMap<QString, QMap<QString, QString>* >;

    gridLayout = new QGridLayout();

    // Loss Database ---------------------------------------------------

    QGroupBox *LossDataGB = new QGroupBox("Component Repair Consequences");
    LossDataGB->setMaximumWidth(500);

    QVBoxLayout *loLDB = new QVBoxLayout(LossDataGB);

    // component data folder
    QHBoxLayout *selectDBLayout = new QHBoxLayout();

    QLabel *lblCDB = new QLabel();
    lblCDB->setText("Database:");
    //lblCDB->setMaximumWidth(150);
    //lblCDB->setMinimumWidth(150);
    lblCDB->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    selectDBLayout->addWidget(lblCDB);

    // database selection
    databaseConseq = new QComboBox();
    databaseConseq->setToolTip(tr("This database defines the component repair consequence functions available for the analysis."));
    databaseConseq->addItem("FEMA P-58",0);
    databaseConseq->addItem("Hazus Earthquake - Buildings",1);
    databaseConseq->addItem("Hazus Earthquake - Transportation",2);
    databaseConseq->addItem("Hazus Hurricane - Buildings - Coupled",3);

    databaseConseq->addItem("None",4);

    databaseConseq->setItemData(0, "Based on the 2nd edition of FEMA P-58", Qt::ToolTipRole);
    databaseConseq->setItemData(1, "Based on the Hazus MH Earthquake Technical Manual v6.1", Qt::ToolTipRole);
    databaseConseq->setItemData(2, "Based on the Hazus MH Earthquake Technical Manual v5.1", Qt::ToolTipRole);
    databaseConseq->setItemData(3, "Based on the Hazus MH Hurricane Technical Manual v5.1", Qt::ToolTipRole);
    databaseConseq->setItemData(4, "None of the built-in databases will be used", Qt::ToolTipRole);

    connect(databaseConseq,SIGNAL(currentIndexChanged(int)),this,SLOT(updateComponentConsequenceDB()));

    selectDBLayout->addWidget(databaseConseq, 1);

    // export db
    QPushButton *btnExportDataBase = new QPushButton();
    btnExportDataBase->setMinimumWidth(100);
    btnExportDataBase->setMaximumWidth(100);
    btnExportDataBase->setText(tr("Export DB"));
    connect(btnExportDataBase, SIGNAL(clicked()),this,SLOT(exportConsequenceDB()));
    selectDBLayout->addWidget(btnExportDataBase);

    loLDB->addLayout(selectDBLayout);

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

    loLDB->addLayout(addCompLayout);

    // database path
    QHBoxLayout *customFolderLayout = new QHBoxLayout();

    leAdditionalComponentDB = new QLineEdit;
    leAdditionalComponentDB->setToolTip(tr("Location of the user-defined component consequence data."));
    customFolderLayout->addWidget(leAdditionalComponentDB, 1);
    leAdditionalComponentDB->setVisible(false);
    leAdditionalComponentDB->setReadOnly(true);

    btnChooseConsequence = new QPushButton();
    btnChooseConsequence->setMinimumWidth(100);
    btnChooseConsequence->setMaximumWidth(100);
    btnChooseConsequence->setText(tr("Choose"));
    connect(btnChooseConsequence, SIGNAL(clicked()),this,SLOT(chooseConsequenceDataBase()));
    customFolderLayout->addWidget(btnChooseConsequence);
    btnChooseConsequence->setVisible(false);

    connect(addComp, SIGNAL(stateChanged(int)), this, SLOT(addComponentCheckChanged(int)));

    //connect(databaseConseq, SIGNAL(currentIndexChanged(QString)), this,
    //            SLOT(DBSelectionChanged(QString)));

    loLDB->addLayout(customFolderLayout);

    loLDB->addStretch();

    // Consequence type selection ----------------------------------------------

    QGroupBox *ConseqSelectGB = new QGroupBox("");
    ConseqSelectGB->setMaximumWidth(300);

    QVBoxLayout *loCSel = new QVBoxLayout(ConseqSelectGB);

    QLabel *lblCTypes = new QLabel();
    lblCTypes->setText("<b>Repair Consequence Types</b>");
    lblCTypes->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    loCSel->addWidget(lblCTypes);

    cbCTypeCost = new QCheckBox();
    cbCTypeCost->setText("Repair Cost");
    cbCTypeCost->setChecked(false);
    cbCTypeCost->setTristate(false);
    cbCTypeCost->setEnabled(false);
    loCSel->addWidget(cbCTypeCost);

    cbCTypeTime = new QCheckBox();
    cbCTypeTime->setText("Repair Time");
    cbCTypeTime->setChecked(false);
    cbCTypeTime->setTristate(false);
    cbCTypeTime->setEnabled(false);
    loCSel->addWidget(cbCTypeTime);

    cbCTypeCarbon = new QCheckBox();
    cbCTypeCarbon->setText("Embodied Carbon in Repairs");
    cbCTypeCarbon->setChecked(false);
    cbCTypeCarbon->setTristate(false);
    cbCTypeCarbon->setEnabled(false);
    loCSel->addWidget(cbCTypeCarbon);

    cbCTypeEnergy = new QCheckBox();
    cbCTypeEnergy->setText("Embodied Energy in Repairs");
    cbCTypeEnergy->setChecked(false);
    cbCTypeEnergy->setTristate(false);
    cbCTypeEnergy->setEnabled(false);
    loCSel->addWidget(cbCTypeEnergy);

    loCSel->addStretch();

    connect(cbCTypeCost, SIGNAL(stateChanged(int)), this,
            SLOT(cTypeSetupChanged(int)));

    connect(cbCTypeTime, SIGNAL(stateChanged(int)), this,
            SLOT(cTypeSetupChanged(int)));

    connect(cbCTypeCarbon, SIGNAL(stateChanged(int)), this,
            SLOT(cTypeSetupChanged(int)));

    connect(cbCTypeEnergy, SIGNAL(stateChanged(int)), this,
            SLOT(cTypeSetupChanged(int)));

    // Consequence info -----------------------------------------------------

    QGroupBox *compDetailsGroupBox = new QGroupBox("");

    QVBoxLayout *loCDetails = new QVBoxLayout(compDetailsGroupBox);

    QLabel *lblCModels = new QLabel();
    lblCModels->setText("<b>Available Consequence Models</b>");
    lblCModels->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    loCDetails->addWidget(lblCModels);

    // select component and consequence type
    QHBoxLayout *selectedCLayout = new QHBoxLayout();

    QLabel *lblSelectedComp = new QLabel();
    lblSelectedComp->setText("Component:");
    lblSelectedComp->setMaximumWidth(70);
    lblSelectedComp->setMinimumWidth(70);
    lblSelectedComp->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    selectedCLayout->addWidget(lblSelectedComp);

    selectedCompCombo = new QComboBox();
    selectedCompCombo->setMinimumWidth(150);
    selectedCBModel = new QStringListModel();
    selectedCompCombo->setModel(selectedCBModel);
    selectedCompCombo->setToolTip(tr("List of available components in the selected databases."));
    connect(selectedCompCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(showSelectedComponent()));
    selectedCLayout->addWidget(selectedCompCombo);

    QLabel *lblSelectedCType = new QLabel();
    lblSelectedCType->setText("Consequence Type:");
    lblSelectedCType->setMaximumWidth(150);
    lblSelectedCType->setMinimumWidth(150);
    lblSelectedCType->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    selectedCLayout->addWidget(lblSelectedCType);

    selectedCType = new QComboBox();
    selectedCType->setMinimumWidth(150);
    //selectedCTypeModel = new QStringListModel();
    //selectedCType->setModel(selectedCTypeModel);
    selectedCType->setToolTip(tr("List of available consequence types for the selected component."));
    connect(selectedCType,SIGNAL(currentIndexChanged(int)),this,SLOT(showSelectedCType()));
    selectedCLayout->addWidget(selectedCType);

    selectedCLayout->addStretch();

    loCDetails->addLayout(selectedCLayout);

    // Additional info
    QHBoxLayout *loCDInfo = new QHBoxLayout();

    compInfo = new QLabel();
    compInfo->setWordWrap(true);
    compInfo->setText("");
    loCDInfo->addWidget(compInfo, 1);

    loCDetails->addLayout(loCDInfo);

    // consequence info

    consequenceViz = new QWebEngineView();
    consequenceViz->setMinimumHeight(320);
    consequenceViz->setMaximumHeight(320);
    consequenceViz->page()->setBackgroundColor(Qt::transparent);
    //consequenceViz->page()->setBackgroundColor(Qt::red);
    consequenceViz->setHtml("");
    consequenceViz->setVisible(false);
    // sy - **NOTE** QWebEngineView display is VERY SLOW when the app is built in debug mode 
    // Max size of figure is limited to 2MB
    loCDetails->addWidget(consequenceViz, 0);


    // Global Consequences ---------------------------------------------

    QGroupBox *GlobalCsqGB = new QGroupBox("Global Consequences");
    GlobalCsqGB->setMaximumWidth(500);

    QVBoxLayout *loGC = new QVBoxLayout(GlobalCsqGB);

    // Replacement

    QHBoxLayout *replacementLayout = new QHBoxLayout();
    replacementLayout->setContentsMargins(0,0,0,0);;

    replacementCheck = new QCheckBox();
    replacementCheck->setText("");
    replacementCheck->setTristate(false);
    replacementCheck->setToolTip(
        tr("<p>Consider replacement as a possible consequence. If automatic loss mapping is used, this consequence is linked to global vulnerabilities, such as irreparable damage and collapse.</p>"));
    replacementCheck->setChecked(false);
    replacementLayout->addWidget(replacementCheck);

    QLabel *lblReplacement = new QLabel();
    lblReplacement->setText("Replacement");
    replacementLayout->addWidget(lblReplacement);

    replacementLayout->addStretch();
    replacementLayout->setSpacing(10);

    loGC->addLayout(replacementLayout);

    replacementSettings = new QWidget();
    QVBoxLayout * loRepSetV = new QVBoxLayout(replacementSettings);
    loRepSetV->setContentsMargins(0,0,0,0);;

    // - - - -
    QHBoxLayout * loReplacementHeader = new QHBoxLayout();
    loReplacementHeader->setContentsMargins(0,0,0,0);;

    QLabel *lblPlaceholder = new QLabel();
    lblPlaceholder->setText("");
    lblPlaceholder->setMaximumWidth(50);
    lblPlaceholder->setMinimumWidth(50);
    loReplacementHeader->addWidget(lblPlaceholder);

    QLabel *lblReplacementUnit = new QLabel();
    lblReplacementUnit->setText("Unit");
    lblReplacementUnit->setMaximumWidth(85);
    lblReplacementUnit->setMinimumWidth(85);
    loReplacementHeader->addWidget(lblReplacementUnit);

    QLabel *lblReplacementMedian = new QLabel();
    lblReplacementMedian->setText("Median");
    lblReplacementMedian->setMaximumWidth(75);
    lblReplacementMedian->setMinimumWidth(75);
    loReplacementHeader->addWidget(lblReplacementMedian);

    QLabel *lblReplacementDistribution = new QLabel();
    lblReplacementDistribution->setText("Distribution");
    lblReplacementDistribution->setMaximumWidth(100);
    lblReplacementDistribution->setMinimumWidth(100);
    loReplacementHeader->addWidget(lblReplacementDistribution);

    QLabel *lblReplacementTheta1 = new QLabel();
    lblReplacementTheta1->setText("<p>&theta;<sub>1</sub></p>");
    lblReplacementTheta1->setMaximumWidth(50);
    lblReplacementTheta1->setMinimumWidth(50);
    loReplacementHeader->addWidget(lblReplacementTheta1);

    loReplacementHeader->addStretch();
    loReplacementHeader->setSpacing(10);

    //loGC->addLayout(loReplacementHeader);
    loRepSetV->addLayout(loReplacementHeader);

    connect(replacementCheck, SIGNAL(stateChanged(int)), this,
                SLOT(replacementCheckChanged(int)));

    // - - - - Cost

    repCostSettings = new QWidget();
    QHBoxLayout * loReplacementCostValues = new QHBoxLayout(repCostSettings);
    loReplacementCostValues->setContentsMargins(0,0,0,0);;

    QLabel *lblCost = new QLabel();
    lblCost->setText("Cost: ");
    lblCost->setMaximumWidth(50);
    lblCost->setMinimumWidth(50);
    loReplacementCostValues->addWidget(lblCost);

    repCostUnit = new QLineEdit();
    repCostUnit->setToolTip(
        tr("Unit used to measure replacement cost. Make sure the specified unit\n"
           "is consistent with the repair cost unit used in the chosen\n"
           "component loss database.\n"
           "FEMA P-58 uses \"USD_2011\";\n"));
    repCostUnit->setMaximumWidth(85);
    repCostUnit->setMinimumWidth(85);
    repCostUnit->setText("");
    repCostUnit->setAlignment(Qt::AlignRight);
    loReplacementCostValues->addWidget(repCostUnit);

    repCostMedian = new QLineEdit();
    repCostMedian->setToolTip(
        tr("Median replacement cost measured in the specified unit.\n"));
    repCostMedian->setMaximumWidth(75);
    repCostMedian->setMinimumWidth(75);
    repCostMedian->setText("");
    repCostMedian->setAlignment(Qt::AlignRight);
    loReplacementCostValues->addWidget(repCostMedian);

    repCostDistribution = new QComboBox();
    repCostDistribution->setToolTip(
        tr("Distribution family assigned to the replacement cost.\n"
            "The N/A setting corresponds to a deterministic cost."));
    repCostDistribution->addItem(tr("N/A"));
    repCostDistribution->addItem(tr("normal"));
    repCostDistribution->addItem(tr("lognormal"));
    repCostDistribution->setMaximumWidth(100);
    repCostDistribution->setMinimumWidth(100);
    repCostDistribution->setCurrentText(tr("N/A"));
    loReplacementCostValues->addWidget(repCostDistribution);

    repCostTheta1 = new QLineEdit();
    repCostTheta1->setToolTip(
        tr("<p>The second parameter of the assigned distribution function.<br>"
           "For a normal distribution, it is the <b>coefficient of variation</b><br>"
           "For a lognormal distribution, it is the logarithmic standard deviation.<br>"
           "Leave this field blank if the assigned cost is deterministic.</p>"));
    repCostTheta1->setMaximumWidth(50);
    repCostTheta1->setMinimumWidth(50);
    repCostTheta1->setText("");
    repCostTheta1->setAlignment(Qt::AlignRight);
    loReplacementCostValues->addWidget(repCostTheta1);

    loReplacementCostValues->addStretch();
    loReplacementCostValues->setSpacing(10);

    //loGC->addLayout(loReplacementCostValues);
    //loRepSetV->addLayout(loReplacementCostValues);
    loRepSetV->addWidget(repCostSettings);

    // - - - - Time

    repTimeSettings = new QWidget();
    QHBoxLayout * loReplacementTimeValues = new QHBoxLayout(repTimeSettings);
    loReplacementTimeValues->setContentsMargins(0,0,0,0);;

    QLabel *lblTime = new QLabel();
    lblTime->setText("Time: ");
    lblTime->setMaximumWidth(50);
    lblTime->setMinimumWidth(50);
    loReplacementTimeValues->addWidget(lblTime);

    repTimeUnit = new QLineEdit();
    repTimeUnit->setToolTip(
        tr("Unit used to measure replacement time. Make sure the specified unit\n"
           "is consistent with the repair time unit used in the chosen\n"
           "component loss database.\n"
           "FEMA P-58 uses \"worker_day\";\n"));
    repTimeUnit->setMaximumWidth(85);
    repTimeUnit->setMinimumWidth(85);
    repTimeUnit->setText("");
    repTimeUnit->setAlignment(Qt::AlignRight);
    loReplacementTimeValues->addWidget(repTimeUnit);

    repTimeMedian = new QLineEdit();
    repTimeMedian->setToolTip(
        tr("Median replacement time measured in the specified unit.\n"));
    repTimeMedian->setMaximumWidth(75);
    repTimeMedian->setMinimumWidth(75);
    repTimeMedian->setText("");
    repTimeMedian->setAlignment(Qt::AlignRight);
    loReplacementTimeValues->addWidget(repTimeMedian);

    repTimeDistribution = new QComboBox();
    repTimeDistribution->setToolTip(
        tr("Distribution family assigned to the replacement time.\n"
            "The N/A setting corresponds to a deterministic time."));
    repTimeDistribution->addItem(tr("N/A"));
    repTimeDistribution->addItem(tr("normal"));
    repTimeDistribution->addItem(tr("lognormal"));
    repTimeDistribution->setMaximumWidth(100);
    repTimeDistribution->setMinimumWidth(100);
    repTimeDistribution->setCurrentText(tr("N/A"));
    loReplacementTimeValues->addWidget(repTimeDistribution);

    repTimeTheta1 = new QLineEdit();
    repTimeTheta1->setToolTip(
        tr("<p>The second parameter of the assigned distribution function.<br>"
           "For a normal distribution, it is the <b>coefficient of variation</b><br>"
           "For a lognormal distribution, it is the logarithmic standard deviation.<br>"
           "Leave this field blank if the assigned time is deterministic.</p>"));
    repTimeTheta1->setMaximumWidth(50);
    repTimeTheta1->setMinimumWidth(50);
    repTimeTheta1->setText("");
    repTimeTheta1->setAlignment(Qt::AlignRight);
    loReplacementTimeValues->addWidget(repTimeTheta1);

    loReplacementTimeValues->addStretch();
    loReplacementTimeValues->setSpacing(10);

    //loGC->addLayout(loReplacementTimeValues);
    //loRepSetV->addLayout(loReplacementTimeValues);
    loRepSetV->addWidget(repTimeSettings);

    // - - - - Carbon

    repCarbonSettings = new QWidget();
    QHBoxLayout * loReplacementCarbonValues = new QHBoxLayout(repCarbonSettings);
    loReplacementCarbonValues->setContentsMargins(0,0,0,0);;

    QLabel *lblCarbon = new QLabel();
    lblCarbon->setText("Carbon: ");
    lblCarbon->setMaximumWidth(50);
    lblCarbon->setMinimumWidth(50);
    loReplacementCarbonValues->addWidget(lblCarbon);

    repCarbonUnit = new QLineEdit();
    repCarbonUnit->setToolTip(
        tr("Unit used to measure carbon emissions replacement.\n" 
           "Make sure the specified unit is consistent with the one chosen\n"
           "for repair carbon emissions in the component loss database.\n"
           "FEMA P-58 uses \"kg\";\n"));
    repCarbonUnit->setMaximumWidth(85);
    repCarbonUnit->setMinimumWidth(85);
    repCarbonUnit->setText("");
    repCarbonUnit->setAlignment(Qt::AlignRight);
    loReplacementCarbonValues->addWidget(repCarbonUnit);

    repCarbonMedian = new QLineEdit();
    repCarbonMedian->setToolTip(
        tr("Median carbon emissions replacement measured in the specified unit.\n"));
    repCarbonMedian->setMaximumWidth(75);
    repCarbonMedian->setMinimumWidth(75);
    repCarbonMedian->setText("");
    repCarbonMedian->setAlignment(Qt::AlignRight);
    loReplacementCarbonValues->addWidget(repCarbonMedian);

    repCarbonDistribution = new QComboBox();
    repCarbonDistribution->setToolTip(
        tr("Distribution family assigned to the carbon emissions replacement.\n"
            "The N/A setting corresponds to a deterministic value."));
    repCarbonDistribution->addItem(tr("N/A"));
    repCarbonDistribution->addItem(tr("normal"));
    repCarbonDistribution->addItem(tr("lognormal"));
    repCarbonDistribution->setMaximumWidth(100);
    repCarbonDistribution->setMinimumWidth(100);
    repCarbonDistribution->setCurrentText(tr("N/A"));
    loReplacementCarbonValues->addWidget(repCarbonDistribution);

    repCarbonTheta1 = new QLineEdit();
    repCarbonTheta1->setToolTip(
        tr("<p>The second parameter of the assigned distribution function.<br>"
           "For a normal distribution, it is the <b>coefficient of variation</b><br>"
           "For a lognormal distribution, it is the logarithmic standard deviation.<br>"
           "Leave this field blank if the assigned time is deterministic.</p>"));
    repCarbonTheta1->setMaximumWidth(50);
    repCarbonTheta1->setMinimumWidth(50);
    repCarbonTheta1->setText("");
    repCarbonTheta1->setAlignment(Qt::AlignRight);
    loReplacementCarbonValues->addWidget(repCarbonTheta1);

    loReplacementCarbonValues->addStretch();
    loReplacementCarbonValues->setSpacing(10);

    //loRepSetV->addLayout(loReplacementCarbonValues);
    loRepSetV->addWidget(repCarbonSettings);

    // - - - - Energy

    repEnergySettings = new QWidget();
    QHBoxLayout * loReplacementEnergyValues = new QHBoxLayout(repEnergySettings);
    loReplacementEnergyValues->setContentsMargins(0,0,0,0);;

    QLabel *lblEnergy = new QLabel();
    lblEnergy->setText("Energy: ");
    lblEnergy->setMaximumWidth(50);
    lblEnergy->setMinimumWidth(50);
    loReplacementEnergyValues->addWidget(lblEnergy);

    repEnergyUnit = new QLineEdit();
    repEnergyUnit->setToolTip(
        tr("Unit used to measure embodied energy replacement.\n" 
           "Make sure the specified unit is consistent with the one chosen\n"
           "for repair embodied energy in the component loss database.\n"
           "FEMA P-58 uses \"MJ\";\n"));
    repEnergyUnit->setMaximumWidth(85);
    repEnergyUnit->setMinimumWidth(85);
    repEnergyUnit->setText("");
    repEnergyUnit->setAlignment(Qt::AlignRight);
    loReplacementEnergyValues->addWidget(repEnergyUnit);

    repEnergyMedian = new QLineEdit();
    repEnergyMedian->setToolTip(
        tr("Median embodied energy replacement measured in the specified unit.\n"));
    repEnergyMedian->setMaximumWidth(75);
    repEnergyMedian->setMinimumWidth(75);
    repEnergyMedian->setText("");
    repEnergyMedian->setAlignment(Qt::AlignRight);
    loReplacementEnergyValues->addWidget(repEnergyMedian);

    repEnergyDistribution = new QComboBox();
    repEnergyDistribution->setToolTip(
        tr("Distribution family assigned to the embodied energy replacement.\n"
            "The N/A setting corresponds to a deterministic value."));
    repEnergyDistribution->addItem(tr("N/A"));
    repEnergyDistribution->addItem(tr("normal"));
    repEnergyDistribution->addItem(tr("lognormal"));
    repEnergyDistribution->setMaximumWidth(100);
    repEnergyDistribution->setMinimumWidth(100);
    repEnergyDistribution->setCurrentText(tr("N/A"));
    loReplacementEnergyValues->addWidget(repEnergyDistribution);

    repEnergyTheta1 = new QLineEdit();
    repEnergyTheta1->setToolTip(
        tr("<p>The second parameter of the assigned distribution function.<br>"
           "For a normal distribution, it is the <b>coefficient of variation</b><br>"
           "For a lognormal distribution, it is the logarithmic standard deviation.<br>"
           "Leave this field blank if the assigned Energy is deterministic.</p>"));
    repEnergyTheta1->setMaximumWidth(50);
    repEnergyTheta1->setMinimumWidth(50);
    repEnergyTheta1->setText("");
    repEnergyTheta1->setAlignment(Qt::AlignRight);
    loReplacementEnergyValues->addWidget(repEnergyTheta1);

    loReplacementEnergyValues->addStretch();
    loReplacementEnergyValues->setSpacing(10);

    //loRepSetV->addLayout(loReplacementEnergyValues);
    loRepSetV->addWidget(repEnergySettings);

    loGC->addWidget(replacementSettings);

    loGC->addStretch();

    connect(replacementCheck, SIGNAL(stateChanged(int)), this,
                SLOT(replacementCheckChanged(int)));
    this->replacementCheckChanged(replacementCheck->checkState());

    // Loss Mapping ---------------------------------------------------

    QGroupBox *LossMappingGB = new QGroupBox("Mapping");
    LossMappingGB->setMaximumWidth(300);

    QVBoxLayout *loMAP = new QVBoxLayout(LossMappingGB);

    QHBoxLayout *selectMAPLayout = new QHBoxLayout();

    QLabel *lblMAPApproach = new QLabel();
    lblMAPApproach->setText("Approach:");
    //lblMAPApproach->setMaximumWidth(100);
    //lblMAPApproach->setMinimumWidth(100);
    selectMAPLayout->addWidget(lblMAPApproach);

    // approach selection
    mapApproach = new QComboBox();
    mapApproach->setToolTip(tr("Mapping links component and global damages to consequences."));
    mapApproach->addItem("Automatic",0);
    mapApproach->addItem("User Defined",1);

    mapApproach->setItemData(0, "<p>Automatically prepare mapping based on the selected Component Vulnerability database. Only applicable for built-in databases for FEMA P-58 and Hazus EQ - Buildings.</p>", Qt::ToolTipRole);
    mapApproach->setItemData(1, "<p>Custom mapping provided in a CSV file.</p>", Qt::ToolTipRole);

    selectMAPLayout->addWidget(mapApproach, 0);

    loMAP->addLayout(selectMAPLayout);

    // damage process path
    QHBoxLayout *customMapLayout = new QHBoxLayout();

    btnChooseMAP = new QPushButton();
    btnChooseMAP->setMinimumWidth(100);
    btnChooseMAP->setMaximumWidth(100);
    btnChooseMAP->setText(tr("Choose"));
    connect(btnChooseMAP, SIGNAL(clicked()),this,SLOT(chooseMAP()));
    customMapLayout->addWidget(btnChooseMAP);
    btnChooseMAP->setVisible(false);

    mapPath = new QLineEdit;
    mapPath->setToolTip(tr("Location of the user-defined loss mapping data."));
    customMapLayout->addWidget(mapPath, 1);
    mapPath->setVisible(false);
    mapPath->setEnabled(false);

    connect(mapApproach, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(MAPApproachSelectionChanged(QString)));

    loMAP->addLayout(customMapLayout);

    loMAP->addStretch();

    // assemble the widgets ---------------------------------------------------

    gridLayout->addWidget(LossDataGB,0,0);
    gridLayout->addWidget(ConseqSelectGB,0,1);
    gridLayout->addWidget(compDetailsGroupBox,1,0,1,2);
    gridLayout->addWidget(GlobalCsqGB,2,0);
    gridLayout->addWidget(LossMappingGB,2,1);

    gridLayout->setRowStretch(2, 1);

    this->setLayout(gridLayout);

    this-> updateComponentConsequenceDB();
}

PelicunLossRepairContainer::~PelicunLossRepairContainer()
{}

void
PelicunLossRepairContainer::addComponentCheckChanged(int newState)
{
    if (newState == 2) {
        leAdditionalComponentDB->setVisible(true);
        btnChooseConsequence->setVisible(true);
    } else {
        leAdditionalComponentDB->setVisible(false);
        btnChooseConsequence->setVisible(false);
    }

    this->updateComponentConsequenceDB();
}

int
PelicunLossRepairContainer::setAdditionalComponentDB(QString additionalComponentDB_path){

    leAdditionalComponentDB->setText(additionalComponentDB_path);
    if (addComp->checkState() != 2){
        addComp->setChecked(true);    
    }
    return 0;
}

void
PelicunLossRepairContainer::chooseConsequenceDataBase(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString additionalComponentDB_path;
    additionalComponentDB_path =
        QFileDialog::getOpenFileName(this, tr("Select Database CSV File"), appDir);

    this->setAdditionalComponentDB(additionalComponentDB_path);

    this->updateComponentConsequenceDB();
}

void
PelicunLossRepairContainer::updateComponentConsequenceDB(){

    bool cmpDataChanged = false;

    QString databasePath = this->getDefaultDatabasePath();
    databasePath += "/resources/DamageAndLossModelLibrary/";

    // check the component consequence database set in the combo box
    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString cmpConsequenceDB_tmp;

    if (databaseConseq->currentText() == "FEMA P-58") {

        cmpConsequenceDB_tmp = databasePath +
        "seismic/building/component/FEMA P-58 2nd Edition/consequence_repair.csv";

    } else if (databaseConseq->currentText() == "Hazus Earthquake - Buildings") {

        cmpConsequenceDB_tmp = databasePath +
        "seismic/building/portfolio/Hazus v6.1/consequence_repair.csv";

    } else if (databaseConseq->currentText() == "Hazus Earthquake - Transportation") {

        cmpConsequenceDB_tmp = databasePath +
        "seismic/transportation_network/portfolio/Hazus v5.1/consequence_repair.csv";

    } else if (databaseConseq->currentText() == "Hazus Hurricane - Buildings - Coupled") {

        cmpConsequenceDB_tmp = databasePath +
        "hurricane/building/portfolio/Hazus v5.1 coupled/consequence_repair.csv";
    
    } else {

        cmpConsequenceDB_tmp = "";

    }

    // check if the component consequence database has changed
    if (cmpConsequenceDB != cmpConsequenceDB_tmp)
    {
        cmpDataChanged = true;
        cmpConsequenceDB = cmpConsequenceDB_tmp;

        if (cmpConsequenceDB != "")
        {
            this->statusMessage("Updating component consequence list with " + 
                                QString(databaseConseq->currentText()) +
                                " data from "+ cmpConsequenceDB);

            // load the visualization path too 
            // (assume that we have a zip file for every bundled DB)
            cmpConsequenceDB_viz = generateConsequenceInfo(cmpConsequenceDB);
            
            /*
            cmpConsequenceDB_viz = cmpConsequenceDB;
            cmpConsequenceDB_viz.chop(4);
            cmpConsequenceDB_viz = cmpConsequenceDB_viz + QString(".zip");
            */

        } else {
            this->statusMessage("Removing built-in component consequence data from the list.");

            cmpConsequenceDB_viz = cmpConsequenceDB;
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
            this->statusMessage("Updating component consequence list with data from" + additionalComponentDB);    
            additionalComponentDB_viz = generateConsequenceInfo(additionalComponentDB);
        } else {
            this->statusMessage("Removing additional component consequence data from the list.");
            additionalComponentDB_viz = "";
        } 
    }

    if (cmpDataChanged == true)
        this->updateAvailableComponents();
}

QString
PelicunLossRepairContainer::getDefaultDatabasePath()
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
PelicunLossRepairContainer::generateConsequenceInfo(QString comp_DB_path)
{

    SimCenterPreferences *preferences = SimCenterPreferences::getInstance();
    QString python = preferences->getPython();
    QString workDir = preferences->getLocalWorkDir();
    QString appDir = preferences->getAppDir();

    QString DLML_folder = "DamageAndLossModelLibrary";

    int DLML_start = comp_DB_path.indexOf(DLML_folder);
    
    QString comp_DB_name;

    // show an error message if the incoming path does not have DLML in it
    if (DLML_start != -1) {

        // Adjust the index to the beginning of the subfolders
        int comp_DB_name_start = DLML_start += DLML_folder.length()+1;

        comp_DB_name = comp_DB_path.mid(comp_DB_name_start);
        comp_DB_name.chop(4); // to remove the .csv from the end
        comp_DB_name = comp_DB_name.replace("/","_");
        comp_DB_name = comp_DB_name.replace("\\","_");
        comp_DB_name = comp_DB_name.replace(" ","_");

    } else {

        // If there is no DLML in the comp path, then assume that we are 
        // dealing with a custom db file and use the filename as an ID
        comp_DB_name = comp_DB_path.mid(comp_DB_path.lastIndexOf("/"));
        comp_DB_name.chop(4);    
    }

    QString output_path = workDir + "/resources/consequence_viz/" + comp_DB_name + "/";

    //this->statusMessage(python);
    //this->statusMessage(workDir);
    //this->statusMessage(output_path);

    QString vizScript = appDir + QDir::separator() + "applications" + QDir::separator()
    + "performDL" + QDir::separator() + "pelicun3" + QDir::separator() + "DL_visuals.py";

    QStringList args; 
    args << QString("repair") << QString(comp_DB_path)
         << QString("--output_path") << QString(output_path);

    RunPythonInThread *vizThread = new RunPythonInThread(vizScript, args, workDir);
    //connect(vizThread, &RunPythonInThread::processFinished, this, &PelicunLossRepairContainer::vizFilesCreated);
    vizThread->runProcess();

    //QProcess proc;
    //QStringList params;

    //params << appDir + "/applications/performDL/pelicun3/" + "DL_visuals.py" << "repair" << comp_DB_path << "--output_path" << output_path;

    //proc.start(python, params);
    //proc.waitForFinished(-1);

    //this->statusMessage(proc.readAllStandardOutput());
    //this->errorMessage(proc.readAllStandardError());

    return output_path;
}

void
PelicunLossRepairContainer::deleteCompDB(){

    qDeleteAll(compDB->begin(), compDB->end());
    compDB->clear();
    delete compDB;
}

int 
PelicunLossRepairContainer::updateAvailableComponents(){

    this->statusMessage("Updating list of available component consequences. This might take a few minutes.");

    // initialize component combo
    selectedCompCombo->clear();

    // initialize ctype combo
    selectedCType->clear();

    QString componentDataBase;
    QStringList compIDs;

    QStringList availableCTypes;

    QVector<QString> qvComp;

    //initialize the component map
    deleteCompDB();
    compDB = new QMap<QString, QMap<QString, QString>* >;

    for (int db_i=0; db_i<2; db_i++)
    {
        if (db_i==0){
            componentDataBase = cmpConsequenceDB;
        } else if (db_i==1) {
            componentDataBase = additionalComponentDB;
        }

        if (componentDataBase==""){
            continue;
        }

        if (componentDataBase.endsWith("csv")) {

            //this->statusMessage("Parsing component consequence data file...");

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

                    parseCSVLine(line, line_list, this);

                    QString compName = line_list[0];

                    if (compName == "ID") {
                        parseCSVLine(line, header, this);
                        continue;
                    } 

                    QStringList compNameParts = compName.split(QRegularExpression("-"));
                    compName = compNameParts[0];
                    QString cType = compNameParts[1];

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
                            C_info -> insert(cType+header[i], line_list[i]);
                        }
                    }

                    // Store the consequence types available with this component
                    QStringList cTypeList;
                    QString cTypes;
                    if (C_info->contains("cTypes")){
                        cTypes = C_info->value("cTypes");
                        cTypeList = cTypes.split(QRegularExpression("-"));
                    }

                    if (!cTypeList.contains(cType)){
                        cTypeList.append(cType);
                        cTypes = cTypeList.join(QString("-"));
                        C_info -> insert("cTypes", cTypes);    

                        // update available ctypes
                        if (!availableCTypes.contains(cType)){
                            availableCTypes.append(cType);
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
                            if (compMetaData.contains("ControllingDemand")){
                                QString blockSize = compMetaData["ControllingDemand"].toString();
                                C_info -> insert("ControllingDemand", blockSize);
                            } else {
                                C_info -> insert("ControllingDemand", QString("N/A"));
                            }
                        }
                    }

                }                

                csvFile.close();

            } else {
	      QString errMsg(QString("Cannot open CSV file: ") + componentDataBase + QString(".csv"));
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
    selectedCompCombo->addItems(compIDs);

    // update ctype checkboxes
    // when the availability of a ctype changes, we turn and check the boxes
    if (availableCTypes.contains("Cost")){

        if (!cbCTypeCost->isEnabled()){
            cbCTypeCost->setChecked(true);
            cbCTypeCost->setEnabled(true);        
        }
    } else {

        if (cbCTypeCost->isEnabled()){
            cbCTypeCost->setChecked(false);
            cbCTypeCost->setEnabled(false);    
        }
    }

    if (availableCTypes.contains("Time")){

        if (!cbCTypeTime->isEnabled()){
            cbCTypeTime->setChecked(true);
            cbCTypeTime->setEnabled(true);        
        }
    } else {

        if (cbCTypeTime->isEnabled()){
            cbCTypeTime->setChecked(false);
            cbCTypeTime->setEnabled(false);    
        }
    }

    if (availableCTypes.contains("Carbon")){

        if (!cbCTypeCarbon->isEnabled()){
            cbCTypeCarbon->setChecked(true);
            cbCTypeCarbon->setEnabled(true);        
        }
    } else {

        if (cbCTypeCarbon->isEnabled()){
            cbCTypeCarbon->setChecked(false);
            cbCTypeCarbon->setEnabled(false);    
        }
    }

    if (availableCTypes.contains("Energy")){

        if (!cbCTypeEnergy->isEnabled()){
            cbCTypeEnergy->setChecked(true);
            cbCTypeEnergy->setEnabled(true);        
        }
    } else {

        if (cbCTypeEnergy->isEnabled()){
            cbCTypeEnergy->setChecked(false);
            cbCTypeEnergy->setEnabled(false);    
        }
    }
    
    return 0;
}

void
PelicunLossRepairContainer::showSelectedComponent(){

    if ((selectedCompCombo->count() > 0) && 
        (selectedCompCombo->currentText() != "") && 
        (compDB->contains(selectedCompCombo->currentText()))){

        selectedCType->clear();

        QString compID = selectedCompCombo->currentText();

        QMap<QString, QString>* C_info = compDB->value(compID);

        // assign the consequence types to the other combobox

        QString cTypes = C_info->value("cTypes");
        QStringList cTypeList = cTypes.split(QRegularExpression("-"));

        cTypeList.sort();
        selectedCType->addItems(cTypeList);
    }

    this->updateComponentInfo();
}

void
PelicunLossRepairContainer::showSelectedCType(){

    this->updateComponentInfo();
}

void
PelicunLossRepairContainer::updateComponentInfo(){

    if ((selectedCompCombo->count() > 0) && 
        (selectedCompCombo->currentText() != "") && 
        (compDB->contains(selectedCompCombo->currentText()))){

        // Start with the metadata

        QString compID = selectedCompCombo->currentText();
        QString cType = selectedCType->currentText();

        QMap<QString, QString>* C_info = compDB->value(compID);

        QString infoString = "";
        infoString += C_info->value("Description");
        infoString += "\nControlling Demand: " + C_info->value("ControllingDemand");

        if (C_info->value("Incomplete") == "1") {
            infoString += "\n\nINCOMPLETE DATA!";
        }

        compInfo->setText(infoString);

        consequenceViz->hide();
        consequenceViz->setVisible(false); // don't worry, we'll set it to true later, if needed

        // Then load the vulnerability model visualization

        QString vizDatabase;
        for (int db_i=1; db_i>=0; db_i--)
        {
            if (db_i==0){
                vizDatabase = cmpConsequenceDB_viz;
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

                if (!componentDBZip.setCurrentFile(compID+"-"+cType+".html")) {
                    this->statusMessage(QString("Cannot find figure for component ") + 
                        compID+"-"+cType + " in " + vizDatabase);
                    continue; // hoping that the component is in the other DB
                }

                QuaZipFile inFile(&componentDBZip);
                if (!inFile.open(QIODevice::ReadOnly)) {
                    this->errorMessage(QString(
                        "Error while trying to open figure for component " + 
                        compID+"-"+cType));
                } else {
                    QTextStream ts(&inFile);
                    htmlString = ts.readAll();
                    inFile.close();
                }
                
                componentDBZip.close();

            } else {

                // we assume that in every other case the viz DB points to a directory

                QFile inFile(vizDatabase + compID+"-"+cType + ".html");

                if (!inFile.exists()){
                    continue;
                }

                if (!inFile.open(QIODevice::ReadOnly)) {
                    this->errorMessage(QString(
                        "Error while trying to open figure for component " + 
                        compID+"-"+cType));
                } else {
                    QTextStream ts(&inFile);
                    htmlString = ts.readAll();
                    inFile.close();
                }
            }

            consequenceViz->setHtml(htmlString,
                QUrl::fromUserInput("/Users/"));                    
            // Zoom factor has a bug in Qt, below is a workaround
            QObject::connect(
                consequenceViz, &QWebEngineView::loadFinished,
                [=](bool arg) {
                    consequenceViz->setZoomFactor(0.75);
                    consequenceViz->show();
                    consequenceViz->setVisible(true);
                });

            break; // so that we do not check the following databases for the same component
        }

    } else {

        // Initialize the description fields and hide them if the combo is empty.
        compInfo->setText("");
    
        consequenceViz->hide();
        consequenceViz->setVisible(false);
    }

    
}

void
PelicunLossRepairContainer::exportConsequenceDB(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString destinationFolder;
    destinationFolder =
        QFileDialog::getExistingDirectory(this,
            tr("Select Destination Folder"), appDir);
    QDir destDir(destinationFolder);

    qDebug() << QString("Exporting consequence database...");

    // copy the db file(s) to the desired location
    QFileInfo fi = QFileInfo(leAdditionalComponentDB->text());

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

    qDebug() << QString("Successfully exported default consequence database...");
}

void
PelicunLossRepairContainer::MAPApproachSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        mapPath->setVisible(true);
        btnChooseMAP->setVisible(true);
    } else {
        mapPath->setVisible(false);
        btnChooseMAP->setVisible(false);
    }
}

void
PelicunLossRepairContainer::replacementCheckChanged(int newState)
{
    if (newState == 2) {
        replacementSettings->show();
    } else {
        replacementSettings->hide();
    }
}

void
PelicunLossRepairContainer::cTypeSetupChanged(int newState)
{
    if (cbCTypeCost->isChecked()){
        repCostSettings->show();
    } else {
        repCostSettings->hide();
    }

    if (cbCTypeTime->isChecked()){
        repTimeSettings->show();
    } else {
        repTimeSettings->hide();
    }

    if (cbCTypeCarbon->isChecked()){
        repCarbonSettings->show();
    } else {
        repCarbonSettings->hide();
    }

    if (cbCTypeEnergy->isChecked()){
        repEnergySettings->show();
    } else {
        repEnergySettings->hide();
    }

}

void
PelicunLossRepairContainer::chooseMAP(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString mapFilePath;
    mapFilePath =
        QFileDialog::getOpenFileName(this,
            tr("Select Loss Mapping CSV File"), appDir);

    mapPath->setText(mapFilePath);

}

bool PelicunLossRepairContainer::outputToJSON(QJsonObject &outputObject) {

    QJsonObject lossData;

    if (replacementCheck->isChecked()) {

        if ((cbCTypeCost->isChecked()) && 
            (repCostMedian->text() != QString(""))) {

            QJsonObject costData;

            costData["Unit"] = repCostUnit->text();
            costData["Median"] = repCostMedian->text();            

            if (repCostDistribution->currentText() != QString("N/A")) {
                costData["Distribution"] = repCostDistribution->currentText();
                costData["Theta_1"] = repCostTheta1->text();                
            }

            lossData["ReplacementCost"] = costData;        
        }

        if ((cbCTypeTime->isChecked()) && 
            (repTimeMedian->text() != QString(""))) {

            QJsonObject timeData;

            timeData["Unit"] = repTimeUnit->text();
            timeData["Median"] = repTimeMedian->text();        

            if (repTimeDistribution->currentText() != QString("N/A")) {
                timeData["Distribution"] = repTimeDistribution->currentText();
                timeData["Theta_1"] = repTimeTheta1->text();
            }

            lossData["ReplacementTime"] = timeData;
        }

        if ((cbCTypeCarbon->isChecked()) && 
            (repCarbonMedian->text() != QString(""))) {

            QJsonObject CarbonData;

            CarbonData["Unit"] = repCarbonUnit->text();
            CarbonData["Median"] = repCarbonMedian->text();        

            if (repCarbonDistribution->currentText() != QString("N/A")) {
                CarbonData["Distribution"] = repCarbonDistribution->currentText();
                CarbonData["Theta_1"] = repCarbonTheta1->text();
            }

            lossData["ReplacementCarbon"] = CarbonData;
        }

        if ((cbCTypeEnergy->isChecked()) && 
            (repEnergyMedian->text() != QString(""))) {

            QJsonObject EnergyData;

            EnergyData["Unit"] = repEnergyUnit->text();
            EnergyData["Median"] = repEnergyMedian->text();        

            if (repEnergyDistribution->currentText() != QString("N/A")) {
                EnergyData["Distribution"] = repEnergyDistribution->currentText();
                EnergyData["Theta_1"] = repEnergyTheta1->text();
            }

            lossData["ReplacementEnergy"] = EnergyData;
        }
    }

    QJsonObject decisionVariables;

    decisionVariables["Cost"] = cbCTypeCost->isChecked();
    decisionVariables["Time"] = cbCTypeTime->isChecked();
    decisionVariables["Carbon"] = cbCTypeCarbon->isChecked();
    decisionVariables["Energy"] = cbCTypeEnergy->isChecked();

    lossData["DecisionVariables"] = decisionVariables;

    lossData["ConsequenceDatabase"] = databaseConseq->currentText();

    if ((addComp->checkState() == 2) && (additionalComponentDB != "")){
        lossData["ConsequenceDatabasePath"] = additionalComponentDB;
    }

    lossData["MapApproach"] = mapApproach->currentText();

    if (mapApproach->currentText() == "User Defined") {
        lossData["MapFilePath"] = mapPath->text();
    }

    if ((addComp->checkState() != 2) && (databaseConseq->currentText() == "None")){
        // No loss calculation needed
    } else {
        outputObject["Repair"] = lossData;    
    }

    return 0;
}

void PelicunLossRepairContainer::initPanel(){

    replacementCheck->setChecked(false);

    cbCTypeCost->setChecked(false);
    cbCTypeTime->setChecked(false);
    cbCTypeCarbon->setChecked(false);
    cbCTypeEnergy->setChecked(false);

    databaseConseq->setCurrentText("FEMA P-58");
    addComp->setChecked(false);
    leAdditionalComponentDB->setText("");

    this->updateComponentConsequenceDB();

    repCostUnit->setText("");
    repCostMedian->setText("");
    repCostDistribution->setCurrentText("N/A");
    repCostTheta1->setText("");

    repTimeUnit->setText("");
    repTimeMedian->setText("");
    repTimeDistribution->setCurrentText("N/A");
    repTimeTheta1->setText("");

    repCarbonUnit->setText("");
    repCarbonMedian->setText("");
    repCarbonDistribution->setCurrentText("N/A");
    repCarbonTheta1->setText("");

    repEnergyUnit->setText("");
    repEnergyMedian->setText("");
    repEnergyDistribution->setCurrentText("N/A");
    repEnergyTheta1->setText("");

    mapApproach->setCurrentText("Automatic");
}

bool PelicunLossRepairContainer::inputFromJSON(QJsonObject & inputObject) {

    replacementCheck->setChecked(false);

    // initialize the panel
    this->initPanel();

    if (inputObject.contains("BldgRepair") || inputObject.contains("Repair")) {

        QJsonObject lossData;
        if (inputObject.contains("BldgRepair")) {
            // for the sake of backwards compatibility
            lossData = inputObject["BldgRepair"].toObject();
        } else {
            lossData = inputObject["Repair"].toObject();
        }

        if (lossData.contains("ConsequenceDatabase")) {

            // ---
            // this is kept for backward compatibility - drop after PBE 4.0
            QString in_componentDB = lossData["ConsequenceDatabase"].toString();

            if (in_componentDB == "User Defined") {
                in_componentDB = "None";
            }

            if (in_componentDB == "Hazus Earthquake") {
                in_componentDB = "Hazus Earthquake - Buildings";
            }

            // ---

            databaseConseq->setCurrentText(in_componentDB);
        }

        if (lossData.contains("ConsequenceDatabasePath")){
            this->setAdditionalComponentDB(lossData["ConsequenceDatabasePath"].toString());
        } else {
            addComp->setChecked(false);
        }
        this->updateComponentConsequenceDB();

        if (lossData.contains("DecisionVariables")){

            QJsonObject decisionVariables = lossData["DecisionVariables"].toObject();

            cbCTypeCost->setChecked(decisionVariables["Cost"].toBool());
            cbCTypeTime->setChecked(decisionVariables["Time"].toBool());
            cbCTypeCarbon->setChecked(decisionVariables["Carbon"].toBool());
            cbCTypeEnergy->setChecked(decisionVariables["Energy"].toBool());
        } else {

            // ---
            // this is kept for backward compatibility - drop after PBE 4.0

            cbCTypeCost->setChecked(cbCTypeCost->isEnabled());
            cbCTypeTime->setChecked(cbCTypeTime->isEnabled());
            cbCTypeCarbon->setChecked(cbCTypeCarbon->isEnabled());
            cbCTypeEnergy->setChecked(cbCTypeEnergy->isEnabled());

            // ---
        }
        

        if ((lossData.contains("ReplacementCost")) ||
            (lossData.contains("ReplacementTime")) ||
            (lossData.contains("ReplacementCarbon")) ||
            (lossData.contains("ReplacementEnergy"))) {

            replacementCheck->setChecked(true);
        }

        if (lossData.contains("ReplacementCost")) {

            QJsonObject costData = lossData["ReplacementCost"].toObject();

            if (costData.contains("Unit")) {
                repCostUnit->setText(costData["Unit"].toString());
            }
            if (costData.contains("Median")) {
                repCostMedian->setText(costData["Median"].toString());
            }
            if (costData.contains("Distribution")) {
                repCostDistribution->setCurrentText(costData["Distribution"].toString());
            } else {
                repCostDistribution->setCurrentText(QString("N/A"));
            }
            if (costData.contains("Theta_1")) {
                repCostTheta1->setText(costData["Theta_1"].toString());
            }
        }

        if (lossData.contains("ReplacementTime")) {

            QJsonObject timeData = lossData["ReplacementTime"].toObject();

            if (timeData.contains("Unit")) {
                repTimeUnit->setText(timeData["Unit"].toString());
            }
            if (timeData.contains("Median")) {
                repTimeMedian->setText(timeData["Median"].toString());
            }
            if (timeData.contains("Distribution")) {
                repTimeDistribution->setCurrentText(timeData["Distribution"].toString());
            }
            if (timeData.contains("Theta_1")) {
                repTimeTheta1->setText(timeData["Theta_1"].toString());
            }
        }

        if (lossData.contains("ReplacementCarbon")) {

            QJsonObject carbonData = lossData["ReplacementCarbon"].toObject();

            if (carbonData.contains("Unit")) {
                repCarbonUnit->setText(carbonData["Unit"].toString());
            }
            if (carbonData.contains("Median")) {
                repCarbonMedian->setText(carbonData["Median"].toString());
            }
            if (carbonData.contains("Distribution")) {
                repCarbonDistribution->setCurrentText(carbonData["Distribution"].toString());
            } 
            if (carbonData.contains("Theta_1")) {
                repCarbonTheta1->setText(carbonData["Theta_1"].toString());
            }
        }

        if (lossData.contains("ReplacementEnergy")) {

            QJsonObject energyData = lossData["ReplacementEnergy"].toObject();

            if (energyData.contains("Unit")) {
                repEnergyUnit->setText(energyData["Unit"].toString());
            }
            if (energyData.contains("Median")) {
                repEnergyMedian->setText(energyData["Median"].toString());
            }
            if (energyData.contains("Distribution")) {
                repEnergyDistribution->setCurrentText(energyData["Distribution"].toString());
            } 
            if (energyData.contains("Theta_1")) {
                repEnergyTheta1->setText(energyData["Theta_1"].toString());
            }
        }

        if (lossData.contains("MapApproach")) {
            mapApproach->setCurrentText(lossData["MapApproach"].toString());

            if (mapApproach->currentText() == "User Defined"){
                mapPath->setText(lossData["MapFilePath"].toString());
            }
        }
    }


    return 0;
}

