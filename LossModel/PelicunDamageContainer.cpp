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
#include <SectionTitle.h>

#include "SimCenterPreferences.h"
#include "PelicunDamageContainer.h"

PelicunDamageContainer::PelicunDamageContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    int maxWidth = 800;

    this->setMaximumWidth(maxWidth);

    gridLayout = new QGridLayout();

    // Global Vulnerability ---------------------------------------------------

    QGroupBox *GlobalVulGB = new QGroupBox("Global Vulnerabilities");
    GlobalVulGB->setMaximumWidth(500);

    QVBoxLayout *loGV = new QVBoxLayout(GlobalVulGB);

    // Excessive RID

    QHBoxLayout *residualDriftLayout = new QHBoxLayout();
    residualDriftLayout->setContentsMargins(0,0,0,0);;

    excessiveRID = new QCheckBox();
    excessiveRID->setText("");
    excessiveRID->setTristate(false);
    excessiveRID->setToolTip("Consider damage from excessive residual interstory drift.");
    excessiveRID->setChecked(false);
    residualDriftLayout->addWidget(excessiveRID);

    QLabel *lblExcessiveRID = new QLabel();
    lblExcessiveRID->setText("Irreparable Damage");
    residualDriftLayout->addWidget(lblExcessiveRID);

    residualDriftLayout->addStretch();
    residualDriftLayout->setSpacing(10);

    loGV->addLayout(residualDriftLayout);

    irreparableSettings = new QWidget();
    QVBoxLayout * loIrrepV = new QVBoxLayout(irreparableSettings);
    loIrrepV->setContentsMargins(0,0,0,0);;

    // - - - -

    QHBoxLayout *loExcRIDHeader = new QHBoxLayout();
    loExcRIDHeader->setContentsMargins(0,0,0,0);;

    QLabel *lblPlaceHolder = new QLabel();
    lblPlaceHolder->setText("");
    lblPlaceHolder->setMaximumWidth(100);
    lblPlaceHolder->setMinimumWidth(100);
    loExcRIDHeader->addWidget(lblPlaceHolder);

    QLabel *lblRIDMedian = new QLabel();
    lblRIDMedian->setText("Median");
    lblRIDMedian->setMaximumWidth(75);
    lblRIDMedian->setMinimumWidth(75);
    loExcRIDHeader->addWidget(lblRIDMedian);

    QLabel *lblRIDStd = new QLabel();
    lblRIDStd->setText("Log Std");
    lblRIDStd->setMaximumWidth(75);
    lblRIDStd->setMinimumWidth(75);
    loExcRIDHeader->addWidget(lblRIDStd);

    loExcRIDHeader->addStretch();
    loExcRIDHeader->setSpacing(10);

    //loGV->addLayout(loExcRIDHeader);
    loIrrepV->addLayout(loExcRIDHeader);

    // - - - -

    QHBoxLayout *loExcRIDValues = new QHBoxLayout();
    loExcRIDValues->setContentsMargins(0,0,0,0);;

    QLabel *lblRIDCapacity = new QLabel();
    lblRIDCapacity->setText("Drift Capacity: ");
    lblRIDCapacity->setMaximumWidth(100);
    lblRIDCapacity->setMinimumWidth(100);
    loExcRIDValues->addWidget(lblRIDCapacity);

    rdMedian = new QLineEdit();
    rdMedian->setToolTip(
        tr("Median of the residual drift distribution conditioned on irreparable \n"
           "damage in the building."));
    rdMedian->setMaximumWidth(75);
    rdMedian->setMinimumWidth(75);
    rdMedian->setText("");
    rdMedian->setAlignment(Qt::AlignRight);
    loExcRIDValues->addWidget(rdMedian);

    rdStd = new QLineEdit();
    rdStd->setToolTip(
        tr("Logarithmic standard deviation of the residual drift distribution \n"
           "conditioned on irreparable damage in the building."));
    rdStd->setMaximumWidth(75);
    rdStd->setMinimumWidth(75);
    rdStd->setText("");
    rdStd->setAlignment(Qt::AlignRight);
    loExcRIDValues->addWidget(rdStd);

    loExcRIDValues->addStretch();
    loExcRIDValues->setSpacing(10);

    //loGV->addLayout(loExcRIDValues);
    loIrrepV->addLayout(loExcRIDValues);

    loGV->addWidget(irreparableSettings);

    connect(excessiveRID, SIGNAL(stateChanged(int)), this,
                SLOT(irreparableCheckChanged(int)));
    this->irreparableCheckChanged(excessiveRID->checkState());

    // add line
    QFrame *lineGV = new QFrame();
    lineGV->setFrameShape(QFrame::HLine);
    lineGV->setFrameShadow(QFrame::Sunken);
    loGV->addWidget(lineGV);

    // Collapse

    QHBoxLayout *collapseLayout = new QHBoxLayout();
    collapseLayout->setContentsMargins(0,0,0,0);;

    collapseCheck = new QCheckBox();
    collapseCheck->setText("");
    collapseCheck->setTristate(false);
    collapseCheck->setToolTip("Consider collapse as a possible outcome.");
    collapseCheck->setChecked(false);
    collapseLayout->addWidget(collapseCheck);

    QLabel *lblCollapse = new QLabel();
    lblCollapse->setText("Collapse");
    collapseLayout->addWidget(lblCollapse);

    collapseLayout->addStretch();
    collapseLayout->setSpacing(10);

    loGV->addLayout(collapseLayout);

    collapseSettings = new QWidget();
    QVBoxLayout * loColSetV = new QVBoxLayout(collapseSettings);
    loColSetV->setContentsMargins(0,0,0,0);;

    // - - - -

    QHBoxLayout *loCollapseHeader = new QHBoxLayout();
    loCollapseHeader->setContentsMargins(0,0,0,0);;

    QLabel *lblCollapseDemand = new QLabel();
    lblCollapseDemand->setText("Demand");
    lblCollapseDemand->setMaximumWidth(75);
    lblCollapseDemand->setMinimumWidth(75);
    loCollapseHeader->addWidget(lblCollapseDemand);

    QLabel *lblCollapseCapacity = new QLabel();
    lblCollapseCapacity->setText("Capacity");
    lblCollapseCapacity->setMaximumWidth(75);
    lblCollapseCapacity->setMinimumWidth(75);
    loCollapseHeader->addWidget(lblCollapseCapacity);

    QLabel *lblCollapseDistribution = new QLabel();
    lblCollapseDistribution->setText("Distribution");
    lblCollapseDistribution->setMaximumWidth(100);
    lblCollapseDistribution->setMinimumWidth(100);
    loCollapseHeader->addWidget(lblCollapseDistribution);

    QLabel *lblCollapseTheta1 = new QLabel();
    lblCollapseTheta1->setText("<p>&theta;<sub>1</sub></p>");
    lblCollapseTheta1->setMaximumWidth(50);
    lblCollapseTheta1->setMinimumWidth(50);
    loCollapseHeader->addWidget(lblCollapseTheta1);

    loCollapseHeader->addStretch();
    loCollapseHeader->setSpacing(10);

    //loGV->addLayout(loCollapseHeader);
    loColSetV->addLayout(loCollapseHeader);

    // - - - -

    QHBoxLayout *loCollapseValues = new QHBoxLayout();
    loCollapseValues->setContentsMargins(0,0,0,0);;

    colDemand = new QLineEdit();
    colDemand->setToolTip(
        tr("The demand type that controls collapse evaluation. The\n"
           "acronyms used here shall match those in the demand data.\n"
           "If you want to represent a collapse fragility function,\n"
           "specify the independent variable of that function here.\n"
           "If you want to represent an EDP threshold that triggers\n"
           "collapse, specify the EDP type here.\n"
           "Make sure the corresponding demands are available in \n"
           "the input demand data and that Pelicun can parse the \n"
           "specified demand type."));
    colDemand->setMaximumWidth(75);
    colDemand->setMinimumWidth(75);
    colDemand->setText("");
    colDemand->setAlignment(Qt::AlignRight);
    loCollapseValues->addWidget(colDemand);

    colMedian = new QLineEdit();
    colMedian->setToolTip(
        tr("Median of the specified demand distribution conditioned on collapse.\n"
           "For collapse fragility curves, this is the median of the curve.\n"
           "For deterministic EDP thresholds, this is the EDP limit.\n"
           "For probabilistic EDP thresholds, this is the median of the random EDP limit."));
    colMedian->setMaximumWidth(75);
    colMedian->setMinimumWidth(75);
    colMedian->setText("");
    colMedian->setAlignment(Qt::AlignRight);
    loCollapseValues->addWidget(colMedian);

    colDistribution = new QComboBox();
    colDistribution->setToolTip(
        tr("Distribution family assigned to the collapse capacity.\n"
            "The N/A setting corresponds to a deterministic capacity."));
    colDistribution->addItem(tr("N/A"));
    colDistribution->addItem(tr("normal"));
    colDistribution->addItem(tr("lognormal"));
    colDistribution->setMaximumWidth(100);
    colDistribution->setMinimumWidth(100);
    colDistribution->setCurrentText(tr("N/A"));
    loCollapseValues->addWidget(colDistribution);

    colTheta2 = new QLineEdit();
    colTheta2->setToolTip(
        tr("<p>The second parameter of the assigned distribution function.<br>"
           "For a normal distribution, it is the <b>coefficient of variation</b><br>"
           "For a lognormal distribution, it is the logarithmic standard deviation.<br>"
           "Leave this field blank for deterministic capacities.</p>"));
    colTheta2->setMaximumWidth(50);
    colTheta2->setMinimumWidth(50);
    colTheta2->setText("");
    colTheta2->setAlignment(Qt::AlignRight);
    loCollapseValues->addWidget(colTheta2);

    loCollapseValues->addStretch();
    loCollapseValues->setSpacing(10);

    //loGV->addLayout(loCollapseValues);
    loColSetV->addLayout(loCollapseValues);

    loGV->addWidget(collapseSettings);

    connect(collapseCheck, SIGNAL(stateChanged(int)), this,
                SLOT(collapseCheckChanged(int)));
    this->collapseCheckChanged(collapseCheck->checkState());

    loGV->addStretch();

    // Data Source ------------------------------------------------------------

    QGroupBox *DamageProcessGB = new QGroupBox("Damage Process");
    //DamageProcessGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loDP = new QVBoxLayout(DamageProcessGB);

    QHBoxLayout *selectDPLayout = new QHBoxLayout();

    QLabel *lblDPApproach = new QLabel();
    lblDPApproach->setText("Approach:");
    lblDPApproach->setMaximumWidth(100);
    lblDPApproach->setMinimumWidth(100);
    selectDPLayout->addWidget(lblDPApproach);

    // approach selection
    dpApproach = new QComboBox();
    dpApproach->setToolTip(tr("The Damage Process defines the dependencies between component and global damages"));
    dpApproach->addItem("FEMA P-58",0);
    dpApproach->addItem("Hazus Earthquake",1);
    dpApproach->addItem("User Defined",2);
    dpApproach->addItem("None",3);

    dpApproach->setItemData(0, "<p>Collapse and irreparable damage, if applicable, is evaluated first; component damages are only returned for non-collapse cases.</p>", Qt::ToolTipRole);
    dpApproach->setItemData(1, "<p>Collapse is triggered by the second mutually exclusive damage state in the highest limit state of structural components. When collapse is triggered, all components have their highest limit state assigned.</p>", Qt::ToolTipRole);
    dpApproach->setItemData(2, "<p>Custom damage process provided in a JSON file</p>", Qt::ToolTipRole);
    dpApproach->setItemData(3, "<p>No damage process applied; i.e., no relationship modeled between component damages.</p>", Qt::ToolTipRole);

    selectDPLayout->addWidget(dpApproach, 1);

    loDP->addLayout(selectDPLayout);

    // damage process path
    QHBoxLayout *customSourceLayout = new QHBoxLayout();

    btnChooseDP = new QPushButton();
    btnChooseDP->setMinimumWidth(100);
    btnChooseDP->setMaximumWidth(100);
    btnChooseDP->setText(tr("Choose"));
    connect(btnChooseDP, SIGNAL(clicked()),this,SLOT(chooseDPData()));
    customSourceLayout->addWidget(btnChooseDP);
    btnChooseDP->setVisible(false);

    dpDataPath = new QLineEdit;
    dpDataPath->setToolTip(tr("Location of the user-defined damage process data."));
    customSourceLayout->addWidget(dpDataPath, 1);
    dpDataPath->setVisible(false);
    dpDataPath->setEnabled(false);

    connect(dpApproach, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(DPApproachSelectionChanged(QString)));

    loDP->addLayout(customSourceLayout);

    loDP->addStretch();

    // assemble the widgets ---------------------------------------------------

    gridLayout->addWidget(GlobalVulGB,0,0);
    gridLayout->addWidget(DamageProcessGB,0,1);
    
    gridLayout->setRowStretch(0, 1);

    this->setLayout(gridLayout);
}

PelicunDamageContainer::~PelicunDamageContainer()
{}

void
PelicunDamageContainer::chooseDPData(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString dpDataPath;
    dpDataPath =
        QFileDialog::getOpenFileName(this,
            tr("Select Damage Process JSON File"), appDir);

    this->setDPData(dpDataPath);

}

int
PelicunDamageContainer::setDPData(QString dpPath){

    dpDataPath->setText(dpPath);
    return 0;
}

void
PelicunDamageContainer::DPApproachSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("User Defined")) {
        dpDataPath->setVisible(true);
        btnChooseDP->setVisible(true);
    } else {
        dpDataPath->setVisible(false);
        btnChooseDP->setVisible(false);
    }
}

void
PelicunDamageContainer::irreparableCheckChanged(int newState)
{
    if (newState == 2) {
        irreparableSettings->show();
    } else {
        irreparableSettings->hide();
    }
}

void
PelicunDamageContainer::collapseCheckChanged(int newState)
{
    if (newState == 2) {
        collapseSettings->show();
    } else {
        collapseSettings->hide();
    }
}

bool PelicunDamageContainer::outputToJSON(QJsonObject &outputObject) {

    QJsonObject damageData;

    if (excessiveRID->isChecked()) {

        QJsonObject irrepData;

        irrepData["DriftCapacityMedian"] = rdMedian->text();
        irrepData["DriftCapacityLogStd"] = rdStd->text();

        damageData["IrreparableDamage"] = irrepData;

    }

    if (collapseCheck->isChecked()) {

        QJsonObject collData;

        collData["DemandType"] = colDemand->text();
        collData["CapacityMedian"] = colMedian->text();

        if (colDistribution->currentText() != QString("N/A")) {
            collData["CapacityDistribution"] = colDistribution->currentText();
            collData["Theta_1"] = colTheta2->text();
        }

        damageData["CollapseFragility"] = collData;
    }

    damageData["DamageProcess"] = dpApproach->currentText();

    if (dpApproach->currentText() == "User Defined") {
        damageData["DamageProcessFilePath"] = dpDataPath->text();
    }

    outputObject["Damage"] = damageData;

    return 0;
}

bool PelicunDamageContainer::inputFromJSON(QJsonObject & inputObject) {

    bool result = false;

    QJsonObject damageData = inputObject["Damage"].toObject();

    if (damageData.contains("IrreparableDamage")) {

        excessiveRID->setChecked(true);

        QJsonObject irrepData = damageData["IrreparableDamage"].toObject();

        if (irrepData.contains("DriftCapacityMedian")) {
            rdMedian->setText(irrepData["DriftCapacityMedian"].toString());
        }
        if (irrepData.contains("DriftCapacityLogStd")) {
            rdStd->setText(irrepData["DriftCapacityLogStd"].toString());
        }

    } else {

        excessiveRID->setChecked(false);
    }

    if (damageData.contains("CollapseFragility")) {

        collapseCheck->setChecked(true);

        QJsonObject collData = damageData["CollapseFragility"].toObject();

        if (collData.contains("DemandType")) {
            colDemand->setText(collData["DemandType"].toString());
        }
        if (collData.contains("CapacityMedian")) {
            colMedian->setText(collData["CapacityMedian"].toString());
        }
        if (collData.contains("CapacityDistribution")) {
            colDistribution->setCurrentText(collData["CapacityDistribution"].toString());
        } else {
            colDistribution->setCurrentText(QString("N/A"));
        }
        if (collData.contains("Theta_1")) {
            colTheta2->setText(collData["Theta_1"].toString());
        }

    } else {

        collapseCheck->setChecked(false);
    }

    if (damageData.contains("DamageProcess")) {
        dpApproach->setCurrentText(damageData["DamageProcess"].toString());

        if (dpApproach->currentText() == "User Defined"){
            dpDataPath->setText(damageData["DamageProcessFilePath"].toString());
        }

    } else {
        dpApproach->setCurrentText("FEMA P-58");
    }

    result = true;

    return result;
}

