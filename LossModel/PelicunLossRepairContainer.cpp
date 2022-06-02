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

#include "SimCenterPreferences.h"
#include "PelicunLossRepairContainer.h"

PelicunLossRepairContainer::PelicunLossRepairContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    this->setMaximumWidth(450);

    int maxWidth = 450;

    gridLayout = new QGridLayout();

    // Global Consequences ---------------------------------------------

    QGroupBox *GlobalCsqGB = new QGroupBox("Global Consequences");
    GlobalCsqGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loGC = new QVBoxLayout(GlobalCsqGB);

    // Replacement

    QHBoxLayout *replacementLayout = new QHBoxLayout();

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

    // - - - -
    QHBoxLayout * loReplacementHeader = new QHBoxLayout();

    QLabel *lblPlaceholder = new QLabel();
    lblPlaceholder->setText("");
    lblPlaceholder->setMaximumWidth(50);
    lblPlaceholder->setMinimumWidth(50);
    loReplacementHeader->addWidget(lblPlaceholder);

    QLabel *lblReplacementUnit = new QLabel();
    lblReplacementUnit->setText("Unit");
    lblReplacementUnit->setMaximumWidth(75);
    lblReplacementUnit->setMinimumWidth(75);
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

    // - - - -

    QHBoxLayout * loReplacementCostValues = new QHBoxLayout();

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
           "FEMA P-58 uses USD_2011;\n"));
    repCostUnit->setMaximumWidth(75);
    repCostUnit->setMinimumWidth(75);
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
    loRepSetV->addLayout(loReplacementCostValues);

    // - - - -

    QHBoxLayout * loReplacementTimeValues = new QHBoxLayout();

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
           "FEMA P-58 uses worker_days;\n"));
    repTimeUnit->setMaximumWidth(75);
    repTimeUnit->setMinimumWidth(75);
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
    loRepSetV->addLayout(loReplacementTimeValues);

    loGC->addWidget(replacementSettings);

    connect(replacementCheck, SIGNAL(stateChanged(int)), this,
                SLOT(replacementCheckChanged(int)));
    this->replacementCheckChanged(replacementCheck->checkState());

    // Loss Database ---------------------------------------------------

    QGroupBox *LossDataGB = new QGroupBox("Database");
    LossDataGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loLDB = new QVBoxLayout(LossDataGB);

    // component data folder
    QHBoxLayout *selectDBLayout = new QHBoxLayout();

    QLabel *lblCDB = new QLabel();
    lblCDB->setText("Consequence Data:");
    lblCDB->setMaximumWidth(150);
    lblCDB->setMinimumWidth(150);
    selectDBLayout->addWidget(lblCDB);

    // database selection
    databaseConseq = new QComboBox();
    databaseConseq->setToolTip(tr("This database defines the consequence functions available for the analysis."));
    databaseConseq->addItem("FEMA P58",0);
    databaseConseq->addItem("Hazus Earthquake",1);
    databaseConseq->addItem("User Defined",2);

    databaseConseq->setItemData(0, "Based on the 2nd edition of FEMA P58", Qt::ToolTipRole);
    databaseConseq->setItemData(1, "Based on the Hazus MH 2.1 Earthquake Technical Manual", Qt::ToolTipRole);
    databaseConseq->setItemData(2, "Custom database provided by the user", Qt::ToolTipRole);

    selectDBLayout->addWidget(databaseConseq, 0);

    QPushButton *btnExportDataBase = new QPushButton();
    btnExportDataBase->setMinimumWidth(100);
    btnExportDataBase->setMaximumWidth(100);
    btnExportDataBase->setText(tr("Export DB"));
    connect(btnExportDataBase, SIGNAL(clicked()),this,SLOT(exportConsequenceDB()));
    selectDBLayout->addWidget(btnExportDataBase);

    loLDB->addLayout(selectDBLayout);

    // database path
    QHBoxLayout *customFolderLayout = new QHBoxLayout();

    consequenceDataBasePath = new QLineEdit;
    consequenceDataBasePath->setToolTip(tr("Location of the user-defined consequence data."));
    customFolderLayout->addWidget(consequenceDataBasePath, 1);
    consequenceDataBasePath->setVisible(false);
    consequenceDataBasePath->setEnabled(false);

    btnChooseConsequence = new QPushButton();
    btnChooseConsequence->setMinimumWidth(100);
    btnChooseConsequence->setMaximumWidth(100);
    btnChooseConsequence->setText(tr("Choose"));
    connect(btnChooseConsequence, SIGNAL(clicked()),this,SLOT(chooseConsequenceDataBase()));
    customFolderLayout->addWidget(btnChooseConsequence);
    btnChooseConsequence->setVisible(false);

    connect(databaseConseq, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(DBSelectionChanged(QString)));

    loLDB->addLayout(customFolderLayout);

    // Loss Mapping ---------------------------------------------------

    QGroupBox *LossMappingGB = new QGroupBox("Mapping");
    LossMappingGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loMAP = new QVBoxLayout(LossMappingGB);

    QHBoxLayout *selectMAPLayout = new QHBoxLayout();

    QLabel *lblMAPApproach = new QLabel();
    lblMAPApproach->setText("Approach:");
    lblMAPApproach->setMaximumWidth(100);
    lblMAPApproach->setMinimumWidth(100);
    selectMAPLayout->addWidget(lblMAPApproach);

    // approach selection
    mapApproach = new QComboBox();
    mapApproach->setToolTip(tr("Mapping links component and global damages to consequences."));
    mapApproach->addItem("Automatic",0);
    mapApproach->addItem("User Defined",1);

    mapApproach->setItemData(0, "<p>Automatically prepare mapping based on the selected Component Vulnerability database. Only applicable for built-in databases, such as FEMA P-58 and Hazus data.</p>", Qt::ToolTipRole);
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

    gridLayout->addWidget(GlobalCsqGB,0,0);
    gridLayout->addWidget(LossDataGB,1,0);
    gridLayout->addWidget(LossMappingGB,2,0);

    this->setLayout(gridLayout);

}

PelicunLossRepairContainer::~PelicunLossRepairContainer()
{}

void
PelicunLossRepairContainer::DBSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        consequenceDataBasePath->setVisible(true);
        btnChooseConsequence->setVisible(true);
    } else {
        consequenceDataBasePath->setVisible(false);
        btnChooseConsequence->setVisible(false);
    }
}

void
PelicunLossRepairContainer::chooseConsequenceDataBase(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString consequenceDataBase;
    consequenceDataBase =
        QFileDialog::getOpenFileName(this, tr("Select Database CSV File"), appDir);

    consequenceDataBasePath->setText(consequenceDataBase);
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
    QFileInfo fi = consequenceDataBasePath->text();

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

        QJsonObject costData;

        costData["Unit"] = repCostUnit->text();
        costData["Median"] = repCostMedian->text();
        costData["Distribution"] = repCostDistribution->currentText();

        if (repCostDistribution->currentText() != "N/A") {
            costData["Theta_1"] = repCostTheta1->text();
        }

        lossData["ReplacementCost"] = costData;

        QJsonObject timeData;

        timeData["Unit"] = repTimeUnit->text();
        timeData["Median"] = repTimeMedian->text();
        timeData["Distribution"] = repTimeDistribution->currentText();

        if (repTimeDistribution->currentText() != "N/A") {
            timeData["Theta_1"] = repTimeTheta1->text();
        }

        lossData["ReplacementTime"] = timeData;
    }

    lossData["ConsequenceDatabase"] = databaseConseq->currentText();

    if (databaseConseq->currentText() == "User Defined") {
        lossData["ConsequenceDatabasePath"] = consequenceDataBasePath->text();
    }

    lossData["MapApproach"] = mapApproach->currentText();

    if (mapApproach->currentText() == "User Defined") {
        lossData["MapFilePath"] = mapPath->text();
    }

    outputObject["BldgRepair"] = lossData;

    return 0;
}

bool PelicunLossRepairContainer::inputFromJSON(QJsonObject & inputObject) {

    return 0;
}

