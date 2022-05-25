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

#include "PelicunLossContainer.h"

PelicunLossContainer::PelicunLossContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{

    int maxWidth = 300;

    mainLayout = new QVBoxLayout();
    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *mainVLayout = new QVBoxLayout();

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
                                        "Consequences in FEMA Pelicun are based on 2011 US dollars"));
    replacementCostValue->setText("");
    replacementCostValue->setAlignment(Qt::AlignRight);
    lossFormLayout->addRow(tr("    Replacement Cost"), replacementCostValue);

    // replacement cost
    replacementTimeValue = new QLineEdit();
    replacementTimeValue->setToolTip(tr("The time it takes to replace the building with a new one.\n"
                                        "Note: the time unit used for replacement time must be \n"
                                        "consistent with those used in the consequence functions.\n"
                                        "Consequence in FEMA Pelicun are based on days in the first edition\n"
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

    mainVLayout->addWidget(lossGroupBox, 0);
    mainVLayout->addStretch(1);
    mainVLayout->setSpacing(10);
    mainVLayout->setMargin(0);

    mainLayout->addLayout(mainVLayout, 0);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);

    this->setLayout(mainLayout);
}

PelicunLossContainer::~PelicunLossContainer()
{}

bool PelicunLossContainer::outputToJSON(QJsonObject &outputObject) {

    return 0;
}

bool PelicunLossContainer::inputFromJSON(QJsonObject & inputObject) {

    return 0;
}

