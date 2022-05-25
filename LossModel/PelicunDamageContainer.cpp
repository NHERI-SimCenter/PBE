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

#include "PelicunDamageContainer.h"

PelicunDamageContainer::PelicunDamageContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    int maxWidth = 300;

    mainLayout = new QVBoxLayout();
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *mainVLayout = new QVBoxLayout();

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

    // assemble the widgets-----------------------------------------------------

    mainVLayout->addWidget(damageGroupBox);
    mainHLayout->addStretch(1);
    mainHLayout->setSpacing(10);
    mainHLayout->setMargin(0);

    mainLayout->addLayout(mainVLayout, 0);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

PelicunDamageContainer::~PelicunDamageContainer()
{}

bool PelicunDamageContainer::outputToJSON(QJsonObject &outputObject) {

    return 0;
}

bool PelicunDamageContainer::inputFromJSON(QJsonObject & inputObject) {

    return 0;
}

