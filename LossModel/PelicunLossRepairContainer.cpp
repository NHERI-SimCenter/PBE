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

#include "PelicunLossRepairContainer.h"

PelicunLossRepairContainer::PelicunLossRepairContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    this->setMaximumWidth(450);

    int maxWidth = 450;

    gridLayout = new QGridLayout();

    // Global Losses ---------------------------------------------------

    QGroupBox *GlobalCsqGB = new QGroupBox("Global Losses");
    GlobalCsqGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loGC = new QVBoxLayout(GlobalCsqGB);

    // Replacement

    QHBoxLayout *replacementLayout = new QHBoxLayout();

    replacementCheck = new QCheckBox();
    replacementCheck->setText("");
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

    // - - - -

    QHBoxLayout *loReplacementHeader = new QHBoxLayout();

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

    loGC->addLayout(loReplacementHeader);

    // - - - -

    QHBoxLayout *loReplacementCostValues = new QHBoxLayout();

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

    loGC->addLayout(loReplacementCostValues);

    // - - - -

    QHBoxLayout *loReplacementTimeValues = new QHBoxLayout();

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

    loGC->addLayout(loReplacementTimeValues);

    loGC->addStretch();

    // Loss Database ---------------------------------------------------

    QGroupBox *LossDataGB = new QGroupBox("Loss Database");
    LossDataGB->setMaximumWidth(maxWidth);

    // Loss Mapping ---------------------------------------------------

    QGroupBox *LossMappingGB = new QGroupBox("Loss Mapping");
    LossMappingGB->setMaximumWidth(maxWidth);

    // assemble the widgets ---------------------------------------------------

    gridLayout->addWidget(GlobalCsqGB,0,0);
    gridLayout->addWidget(LossDataGB,1,0);
    gridLayout->addWidget(LossMappingGB,2,0);

    this->setLayout(gridLayout);

}

PelicunLossRepairContainer::~PelicunLossRepairContainer()
{}

bool PelicunLossRepairContainer::outputToJSON(QJsonObject &outputObject) {

    return 0;
}

bool PelicunLossRepairContainer::inputFromJSON(QJsonObject & inputObject) {

    return 0;
}

