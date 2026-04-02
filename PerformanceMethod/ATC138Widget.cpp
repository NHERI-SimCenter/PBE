/* *****************************************************************************
Copyright (c) 2026, The Regents of the University of California (Regents).
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

// Written: Adam Zsarnoczay

#include "ATC138Widget.h"
#include "GeneralInformationWidget.h"
#include "PelicunLossRepairContainer.h"
#include "MainWindowWorkflowApp.h"
#include "SimCenterPreferences.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>


ATC138Widget::ATC138Widget(QWidget *parent)
    : SimCenterAppWidget(parent), updatingGI(false), updatingLossRepair(false)
{
    theGI = GeneralInformationWidget::getInstance();
    theLossRepair = PelicunLossRepairContainer::getInstance();
    currentLengthUnit = theGI->getLengthUnit();

    QVBoxLayout *layout = new QVBoxLayout(this);

    // --- Building Information group ---
    auto buildingGroup = new QGroupBox("Building Information");
    auto formLayout = new QFormLayout();

    // GI-linked fields
    lengthSide1LE = new QLineEdit();
    lengthSide1LE->setToolTip(tr("Length of building side 1 in feet. \n"
                                  "Linked to Width in General Information."));
    formLayout->addRow("Length Side 1 (ft):", lengthSide1LE);

    lengthSide2LE = new QLineEdit();
    lengthSide2LE->setToolTip(tr("Length of building side 2 in feet. \n"
                                  "Linked to Depth in General Information."));
    formLayout->addRow("Length Side 2 (ft):", lengthSide2LE);

    numStoriesLE = new QLineEdit();
    numStoriesLE->setToolTip(tr("Number of above-ground stories. \n"
                                 "Linked to General Information."));
    formLayout->addRow("Number of Stories:", numStoriesLE);

    heightLE = new QLineEdit();
    heightLE->setToolTip(tr("Total building height in feet. \n"
                             "Linked to General Information."));
    formLayout->addRow("Height (ft):", heightLE);

    typStoryHtValueLabel = new QLabel("--");
    typStoryHtValueLabel->setToolTip(tr("Calculated as total height divided \n"
                                         "by number of stories."));
    formLayout->addRow("Typical Story Height (ft):", typStoryHtValueLabel);

    planAreaValueLabel = new QLabel("--");
    planAreaValueLabel->setToolTip(tr("Calculated as Length Side 1 x Length Side 2. \n"
                                       "Updated automatically when side lengths change."));
    formLayout->addRow("Plan Area (sq ft):", planAreaValueLabel);

    replacementCostLE = new QLineEdit();
    replacementCostLE->setToolTip(tr("Median replacement cost of the building. \n"
                                      "Linked to the Replacement Cost under the \n"
                                      "Loss tab in the DL panel."));
    formLayout->addRow("Replacement Cost (Median):", replacementCostLE);

    // ATC138-specific fields
    numEntryDoorsSB = new QSpinBox();
    numEntryDoorsSB->setRange(1, 100);
    numEntryDoorsSB->setValue(2);
    numEntryDoorsSB->setToolTip(tr("Total number of entry doors to the building. \n"
                                    "Used for egress assessment."));
    formLayout->addRow("Number of Entry Doors:", numEntryDoorsSB);

    typStructBayLengthDSB = new QDoubleSpinBox();
    typStructBayLengthDSB->setRange(1.0, 10000.0);
    typStructBayLengthDSB->setDecimals(1);
    typStructBayLengthDSB->setValue(30.0);
    typStructBayLengthDSB->setToolTip(tr("Typical length of a structural bay in \n"
                                          "feet. Used for scaffolding and \n"
                                          "repair crew allocation."));
    formLayout->addRow("Typical Structural Bay Length (ft):", typStructBayLengthDSB);

    peakOccRateDSB = new QDoubleSpinBox();
    peakOccRateDSB->setRange(0.0, 1.0);
    peakOccRateDSB->setDecimals(4);
    peakOccRateDSB->setSingleStep(0.0001);
    peakOccRateDSB->setValue(0.0031);
    peakOccRateDSB->setToolTip(tr("Peak building occupancy rate in occupants per \n"
                                    "square foot. Used for casualty and egress \n"
                                    "assessment."));
    formLayout->addRow("Peak Occupancy Rate (per sq ft):", peakOccRateDSB);

    engineeringCostRatioDSB = new QDoubleSpinBox();
    engineeringCostRatioDSB->setRange(0.0, 1.0);
    engineeringCostRatioDSB->setDecimals(2);
    engineeringCostRatioDSB->setSingleStep(0.01);
    engineeringCostRatioDSB->setValue(0.10);
    engineeringCostRatioDSB->setToolTip(tr("Fraction of total repair cost attributed \n"
                                             "to engineering and design."));
    formLayout->addRow("Engineering Cost Ratio:", engineeringCostRatioDSB);

    numElevatorsSB = new QSpinBox();
    numElevatorsSB->setRange(0, 50);
    numElevatorsSB->setValue(0);
    numElevatorsSB->setToolTip(tr("Total number of elevators in the building. \n"
                                   "If set to 0, inferred from component data."));
    formLayout->addRow("Number of Elevators:", numElevatorsSB);

    buildingGroup->setLayout(formLayout);
    layout->addWidget(buildingGroup, 0, Qt::AlignLeft);

    // --- Optional file inputs ---

    // Tenant unit list
    QHBoxLayout *tenantLayout = new QHBoxLayout();
    useTenantUnitCB = new QCheckBox("Use custom tenant unit list");
    useTenantUnitCB->setToolTip(tr("Provide a custom CSV file defining tenant units. \n"
                                    "If unchecked, a default file is auto-generated \n"
                                    "with one tenant unit per story. Each unit is \n"
                                    "assigned the full story area, a perimeter area \n"
                                    "based on the building edge lengths and story \n"
                                    "height, and a default occupancy type (id=1)."));
    tenantLayout->addWidget(useTenantUnitCB);

    tenantUnitPathLE = new QLineEdit();
    tenantUnitPathLE->setEnabled(false);
    tenantUnitPathLE->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    tenantLayout->addWidget(tenantUnitPathLE);

    tenantUnitBrowseBtn = new QPushButton("Browse");
    tenantUnitBrowseBtn->setFixedWidth(100);
    tenantUnitBrowseBtn->setEnabled(false);
    tenantLayout->addWidget(tenantUnitBrowseBtn);

    layout->addLayout(tenantLayout);

    connect(useTenantUnitCB, &QCheckBox::stateChanged,
            this, &ATC138Widget::onTenantUnitCheckChanged);
    connect(tenantUnitBrowseBtn, &QPushButton::pressed,
            this, &ATC138Widget::handleTenantUnitBrowsePressed);

    // Optional inputs
    QHBoxLayout *optionalLayout = new QHBoxLayout();
    useOptionalInputsCB = new QCheckBox("Use custom optional inputs");
    useOptionalInputsCB->setToolTip(tr("Provide a JSON file to override default \n"
                                        "assessment parameters for impedance, repair \n"
                                        "time, and functionality options. Only the \n"
                                        "parameters you want to change need to be \n"
                                        "specified; all others use defaults. See the \n"
                                        "documentation for the accepted parameters \n"
                                        "and the corresponding JSON schema."));
    optionalLayout->addWidget(useOptionalInputsCB);

    optionalInputsPathLE = new QLineEdit();
    optionalInputsPathLE->setEnabled(false);
    optionalInputsPathLE->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    optionalLayout->addWidget(optionalInputsPathLE);

    optionalInputsBrowseBtn = new QPushButton("Browse");
    optionalInputsBrowseBtn->setFixedWidth(100);
    optionalInputsBrowseBtn->setEnabled(false);
    optionalLayout->addWidget(optionalInputsBrowseBtn);

    layout->addLayout(optionalLayout);

    connect(useOptionalInputsCB, &QCheckBox::stateChanged,
            this, &ATC138Widget::onOptionalInputsCheckChanged);
    connect(optionalInputsBrowseBtn, &QPushButton::pressed,
            this, &ATC138Widget::handleOptionalInputsBrowsePressed);

    layout->addStretch(1);

    // --- GI signal/slot connections ---

    // Initialize from GI
    double w, d, plan;
    theGI->getBuildingDimensions(w, d, plan);
    int numFloors = theGI->getNumFloors();
    double height = theGI->getHeight();

    // Convert GI values (in current GI units) to feet for display
    double wFt = convertToFeet(w, currentLengthUnit);
    double dFt = convertToFeet(d, currentLengthUnit);
    double hFt = convertToFeet(height, currentLengthUnit);

    lengthSide1LE->setText(QString::number(wFt, 'f', 1));
    lengthSide2LE->setText(QString::number(dFt, 'f', 1));
    numStoriesLE->setText(QString::number(numFloors));
    heightLE->setText(QString::number(hFt, 'f', 1));
    recalcTypStoryHeight();
    recalcPlanArea();

    // GI -> ATC138
    connect(theGI, SIGNAL(numStoriesOrHeightChanged(int, double)),
            this, SLOT(setNumStoriesAndHeight(int, double)));
    connect(theGI, SIGNAL(buildingDimensionsChanged(double,double,double)),
            this, SLOT(setBuildingDimensions(double,double,double)));
    connect(theGI, SIGNAL(unitLengthChanged(QString)),
            this, SLOT(onUnitLengthChanged(QString)));

    // ATC138 -> GI
    connect(lengthSide1LE, &QLineEdit::editingFinished,
            this, &ATC138Widget::updateGIDimensions);
    connect(lengthSide2LE, &QLineEdit::editingFinished,
            this, &ATC138Widget::updateGIDimensions);
    connect(numStoriesLE, &QLineEdit::editingFinished,
            this, &ATC138Widget::updateGIStories);
    connect(heightLE, &QLineEdit::editingFinished,
            this, &ATC138Widget::updateGIStories);

    // Recalc derived fields when inputs change
    connect(numStoriesLE, &QLineEdit::editingFinished,
            this, &ATC138Widget::recalcTypStoryHeight);
    connect(heightLE, &QLineEdit::editingFinished,
            this, &ATC138Widget::recalcTypStoryHeight);
    connect(lengthSide1LE, &QLineEdit::editingFinished,
            this, &ATC138Widget::recalcPlanArea);
    connect(lengthSide2LE, &QLineEdit::editingFinished,
            this, &ATC138Widget::recalcPlanArea);

    // --- LossRepair signal/slot connections ---

    if (theLossRepair) {
        replacementCostLE->setText(theLossRepair->getReplacementCostMedian());

        // LossRepair -> ATC138
        connect(theLossRepair, &PelicunLossRepairContainer::replacementCostChanged,
                this, &ATC138Widget::setReplacementCost);

        // ATC138 -> LossRepair
        connect(replacementCostLE, &QLineEdit::editingFinished,
                this, &ATC138Widget::updateLossRepairReplacementCost);
    }
}


ATC138Widget::~ATC138Widget()
{
}


// --- Unit conversion helpers ---

double ATC138Widget::convertToFeet(double value, const QString &fromUnit)
{
    if (fromUnit == "ft" || fromUnit == "")
        return value;
    if (fromUnit == "in")
        return value / 12.0;
    if (fromUnit == "m")
        return value * 3.28084;
    if (fromUnit == "cm")
        return value * 0.0328084;
    if (fromUnit == "mm")
        return value * 0.00328084;
    return value;
}


double ATC138Widget::convertFromFeet(double value, const QString &toUnit)
{
    if (toUnit == "ft" || toUnit == "")
        return value;
    if (toUnit == "in")
        return value * 12.0;
    if (toUnit == "m")
        return value / 3.28084;
    if (toUnit == "cm")
        return value / 0.0328084;
    if (toUnit == "mm")
        return value / 0.00328084;
    return value;
}


QString ATC138Widget::getGeneralInputsFilePath()
{
    // Try to save next to the project file if a save path is available
    auto *mainWindow = qobject_cast<MainWindowWorkflowApp *>(
        QApplication::activeWindow());
    if (mainWindow && !mainWindow->outputFilePath.isEmpty()) {
        QFileInfo projectFile(mainWindow->outputFilePath);
        QString projectDir = projectFile.absolutePath();
        QDir dir(projectDir);
        if (dir.exists())
            return projectDir + QDir::separator() + "general_inputs.json";
    }

    // Fall back to the local working directory
    QString workDir = SimCenterPreferences::getInstance()->getLocalWorkDir();
    QDir dir(workDir);
    if (!dir.exists())
        dir.mkpath(workDir);
    return workDir + QDir::separator() + "general_inputs.json";
}


// --- GI sync slots ---

void ATC138Widget::setNumStoriesAndHeight(int numFloors, double height)
{
    if (updatingGI)
        return;

    double hFt = convertToFeet(height, currentLengthUnit);
    numStoriesLE->setText(QString::number(numFloors));
    heightLE->setText(QString::number(hFt, 'f', 1));
    recalcTypStoryHeight();
}


void ATC138Widget::setBuildingDimensions(double newWidth, double newDepth,
                                          double planArea)
{
    Q_UNUSED(planArea);
    if (updatingGI)
        return;

    double wFt = convertToFeet(newWidth, currentLengthUnit);
    double dFt = convertToFeet(newDepth, currentLengthUnit);
    lengthSide1LE->setText(QString::number(wFt, 'f', 1));
    lengthSide2LE->setText(QString::number(dFt, 'f', 1));
    recalcPlanArea();
}


void ATC138Widget::onUnitLengthChanged(QString unitName)
{
    // GI units changed — re-read GI values and convert to feet
    currentLengthUnit = unitName;

    double w, d, plan;
    theGI->getBuildingDimensions(w, d, plan);
    double height = theGI->getHeight();

    double wFt = convertToFeet(w, currentLengthUnit);
    double dFt = convertToFeet(d, currentLengthUnit);
    double hFt = convertToFeet(height, currentLengthUnit);

    lengthSide1LE->setText(QString::number(wFt, 'f', 1));
    lengthSide2LE->setText(QString::number(dFt, 'f', 1));
    heightLE->setText(QString::number(hFt, 'f', 1));
    recalcTypStoryHeight();
    recalcPlanArea();
}


void ATC138Widget::updateGIDimensions()
{
    updatingGI = true;

    double wFt = lengthSide1LE->text().toDouble();
    double dFt = lengthSide2LE->text().toDouble();

    double wGI = convertFromFeet(wFt, currentLengthUnit);
    double dGI = convertFromFeet(dFt, currentLengthUnit);
    double planArea = wGI * dGI;

    theGI->setBuildingDimensions(wGI, dGI, planArea);

    updatingGI = false;
}


void ATC138Widget::updateGIStories()
{
    updatingGI = true;

    int numStories = numStoriesLE->text().toInt();
    double hFt = heightLE->text().toDouble();
    double hGI = convertFromFeet(hFt, currentLengthUnit);

    theGI->setNumStoriesAndHeight(numStories, hGI);

    updatingGI = false;
}


void ATC138Widget::recalcTypStoryHeight()
{
    int numStories = numStoriesLE->text().toInt();
    double hFt = heightLE->text().toDouble();

    if (numStories > 0 && hFt > 0) {
        double typStoryHt = hFt / numStories;
        typStoryHtValueLabel->setText(QString::number(typStoryHt, 'f', 1));
    } else {
        typStoryHtValueLabel->setText("--");
    }
}


void ATC138Widget::recalcPlanArea()
{
    double side1 = lengthSide1LE->text().toDouble();
    double side2 = lengthSide2LE->text().toDouble();

    if (side1 > 0 && side2 > 0) {
        planAreaValueLabel->setText(QString::number(side1 * side2, 'f', 1));
    } else {
        planAreaValueLabel->setText("--");
    }
}


// --- LossRepair sync slots ---

void ATC138Widget::setReplacementCost(QString median)
{
    if (updatingLossRepair)
        return;

    updatingLossRepair = true;
    replacementCostLE->setText(median);
    updatingLossRepair = false;
}


void ATC138Widget::updateLossRepairReplacementCost()
{
    if (updatingLossRepair)
        return;

    updatingLossRepair = true;
    if (theLossRepair)
        theLossRepair->setReplacementCostMedian(replacementCostLE->text());
    updatingLossRepair = false;
}


// --- JSON I/O ---

bool ATC138Widget::outputToJSON(QJsonObject &jsonObj)
{
    jsonObj["Application"] = "ATC138";
    return true;
}


bool ATC138Widget::inputFromJSON(QJsonObject &jsonObject)
{
    Q_UNUSED(jsonObject);
    return true;
}


bool ATC138Widget::outputAppDataToJSON(QJsonObject &jsonObj)
{
    jsonObj["Application"] = "ATC138";

    QJsonObject dataObj;

    // Build general_inputs content
    int numStories = numStoriesLE->text().toInt();
    double hFt = heightLE->text().toDouble();
    double typStoryHt = (numStories > 0) ? hFt / numStories : 0.0;

    double side1Ft = lengthSide1LE->text().toDouble();
    double side2Ft = lengthSide2LE->text().toDouble();

    QJsonObject generalInputs;
    generalInputs["num_entry_doors"] = numEntryDoorsSB->value();
    generalInputs["typ_struct_bay_length_ft"] = typStructBayLengthDSB->value();
    generalInputs["typ_story_ht_ft"] = typStoryHt;
    generalInputs["length_side_1_ft"] = side1Ft;
    generalInputs["length_side_2_ft"] = side2Ft;
    generalInputs["peak_occ_rate"] = peakOccRateDSB->value();
    generalInputs["engineering_cost_ratio"] = engineeringCostRatioDSB->value();
    generalInputs["num_elevators"] = numElevatorsSB->value();
    generalInputs["number_of_stories"] = numStories;
    generalInputs["replacement_cost_median"] = replacementCostLE->text().toDouble();
    generalInputs["plan_area_ft2"] = side1Ft * side2Ft;

    // Write general_inputs.json file
    QString filePath = getGeneralInputsFilePath();
    QFile file(filePath);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QJsonDocument doc(generalInputs);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        this->errorMessage("Could not write general_inputs.json to " + filePath);
    }

    dataObj["generalInputsPath"] = filePath;

    // Optional files
    if (useTenantUnitCB->isChecked()) {
        QString path = tenantUnitPathLE->text().trimmed();
        if (!path.isEmpty())
            dataObj["tenantUnitListPath"] = path;
    }

    if (useOptionalInputsCB->isChecked()) {
        QString path = optionalInputsPathLE->text().trimmed();
        if (!path.isEmpty())
            dataObj["optionalInputsPath"] = path;
    }

    jsonObj["ApplicationData"] = dataObj;
    return true;
}


bool ATC138Widget::inputAppDataFromJSON(QJsonObject &jsonObject)
{
    if (!jsonObject.contains("ApplicationData"))
        return false;

    QJsonObject dataObj = jsonObject["ApplicationData"].toObject();

    // Load general_inputs.json
    if (dataObj.contains("generalInputsPath")) {
        QString filePath = dataObj["generalInputsPath"].toString();
        QFile file(filePath);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            QJsonObject gi = doc.object();
            file.close();

            if (gi.contains("num_entry_doors"))
                numEntryDoorsSB->setValue(gi["num_entry_doors"].toInt());
            if (gi.contains("typ_struct_bay_length_ft"))
                typStructBayLengthDSB->setValue(gi["typ_struct_bay_length_ft"].toDouble());
            else if (gi.contains("typ_struct_bay_area_ft"))
                typStructBayLengthDSB->setValue(gi["typ_struct_bay_area_ft"].toDouble());
            if (gi.contains("length_side_1_ft"))
                lengthSide1LE->setText(QString::number(gi["length_side_1_ft"].toDouble(), 'f', 1));
            if (gi.contains("length_side_2_ft"))
                lengthSide2LE->setText(QString::number(gi["length_side_2_ft"].toDouble(), 'f', 1));
            if (gi.contains("peak_occ_rate"))
                peakOccRateDSB->setValue(gi["peak_occ_rate"].toDouble());
            if (gi.contains("engineering_cost_ratio"))
                engineeringCostRatioDSB->setValue(gi["engineering_cost_ratio"].toDouble());
            if (gi.contains("num_elevators"))
                numElevatorsSB->setValue(gi["num_elevators"].toInt());

            // Derive stories and height from typ_story_ht_ft if available
            // Note: stories and height are primarily driven by GI sync,
            // but we update them here for completeness
            if (gi.contains("typ_story_ht_ft")) {
                // typ_story_ht is calculated, not directly set
                // stories and height come from GI
            }

            recalcTypStoryHeight();

            // Update GI with loaded values
            updateGIDimensions();
            updateGIStories();
        }
    }

    // Restore optional file paths
    if (dataObj.contains("tenantUnitListPath")) {
        useTenantUnitCB->setChecked(true);
        tenantUnitPathLE->setText(dataObj["tenantUnitListPath"].toString());
    }

    if (dataObj.contains("optionalInputsPath")) {
        useOptionalInputsCB->setChecked(true);
        optionalInputsPathLE->setText(dataObj["optionalInputsPath"].toString());
    }

    return true;
}


bool ATC138Widget::outputCitation(QJsonObject &jsonObject)
{
    QJsonObject citationATC138;
    citationATC138.insert("citation",
        "ATC, 2021, Seismic Performance Assessment of Buildings Volume 8 - "
        "Methodology for Assessment of Functional Recovery Time, "
        "FEMA, Washington, DC.");
    citationATC138.insert("description",
        "This document describes the ATC-138 functional recovery assessment "
        "methodology. Please reference it if your workflow in the SimCenter "
        "tools includes the ATC-138 methodology.");

    QJsonArray citationsArray;
    citationsArray.push_back(citationATC138);

    jsonObject.insert("citations", citationsArray);

    return true;
}


void ATC138Widget::clear(void)
{
    lengthSide1LE->clear();
    lengthSide2LE->clear();
    numStoriesLE->clear();
    heightLE->clear();
    typStoryHtValueLabel->setText("--");
    planAreaValueLabel->setText("--");
    replacementCostLE->clear();

    numEntryDoorsSB->setValue(2);
    typStructBayLengthDSB->setValue(30.0);
    peakOccRateDSB->setValue(0.0031);
    engineeringCostRatioDSB->setValue(0.10);
    numElevatorsSB->setValue(0);

    useTenantUnitCB->setChecked(false);
    tenantUnitPathLE->clear();
    useOptionalInputsCB->setChecked(false);
    optionalInputsPathLE->clear();
}


// --- Checkbox/Browse slots ---

void ATC138Widget::onTenantUnitCheckChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    tenantUnitPathLE->setEnabled(enabled);
    tenantUnitBrowseBtn->setEnabled(enabled);
}


void ATC138Widget::onOptionalInputsCheckChanged(int state)
{
    bool enabled = (state == Qt::Checked);
    optionalInputsPathLE->setEnabled(enabled);
    optionalInputsBrowseBtn->setEnabled(enabled);
}


void ATC138Widget::handleTenantUnitBrowsePressed()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Select Tenant Unit List"), "",
        tr("CSV Files (*.csv);;All Files (*)"));
    if (!fileName.isEmpty())
        tenantUnitPathLE->setText(fileName);
}


void ATC138Widget::handleOptionalInputsBrowsePressed()
{
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Select Optional Inputs"), "",
        tr("JSON Files (*.json);;All Files (*)"));
    if (!fileName.isEmpty())
        optionalInputsPathLE->setText(fileName);
}
