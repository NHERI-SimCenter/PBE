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

#include "P58DependenciesContainer.h"

P58DependenciesContainer::P58DependenciesContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    mainLayout = new QVBoxLayout();
    QHBoxLayout *mainHLayout = new QHBoxLayout();    
    QVBoxLayout *mainVLayout = new QVBoxLayout();

    // title -------------------------------------------------------------------
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title = new SectionTitle();
    title->setText(tr("Model Dependencies"));
    title->setMinimumWidth(250);

    titleLayout->addWidget(title);
    titleLayout->addStretch();

    // dependencies -------------------------------------------------
    QGroupBox * dependencyGroupBox = new QGroupBox("Perfect Correlation in ...");
    dependencyGroupBox->setToolTip(tr("Specify at which organizational level shall the following random variables be correlated. If no correlation is desired, choose Independent."));
    QFormLayout * dependencyFormLayout = new QFormLayout(dependencyGroupBox);

    // quantities
    quantityDep = new QComboBox();
    quantityDep->setToolTip(tr("Some tooltip"));
    quantityDep->addItem("btw. Fragility Groups",0);
    quantityDep->addItem("btw. Performance Groups",1);
    quantityDep->addItem("btw. Floors",2);
    quantityDep->addItem("btw. Directions",3);    
    quantityDep->addItem("Independent",4);
    quantityDep->setCurrentIndex(4);
    dependencyFormLayout->addRow(tr("Component Quantities"), quantityDep);

     QSpacerItem *spacerGroups9 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups9);

    // fragilities
    fragilityDep = new QComboBox();
    fragilityDep->setToolTip(tr("Some tooltip"));
    fragilityDep->addItem("btw. Performance Groups",0);
    fragilityDep->addItem("btw. Floors",1);
    fragilityDep->addItem("btw. Directions",2);
    fragilityDep->addItem("btw. Damage States",3);
    fragilityDep->addItem("btw. Component Groups",4);
    fragilityDep->addItem("Independent",5);
    fragilityDep->addItem("per ATC recommendation",6);
    fragilityDep->setCurrentIndex(6);
    dependencyFormLayout->addRow(tr("Component Fragilities"), fragilityDep);

     QSpacerItem *spacerGroups10 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups10);

    // reconstruction cost
    costDep = new QComboBox();
    costDep->setToolTip(tr("Some tooltip"));
    costDep->addItem("btw. Fragility Groups",0);
    costDep->addItem("btw. Performance Groups",1);
    costDep->addItem("btw. Floors",2);
    costDep->addItem("btw. Directions",3);
    costDep->addItem("btw. Damage States", 4);
    costDep->addItem("Independent",5);
    costDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("Reconstruction Costs"), costDep);

    // reconstruction time
    timeDep = new QComboBox();
    timeDep->setToolTip(tr("Some tooltip"));
    timeDep->addItem("btw. Fragility Groups",0);
    timeDep->addItem("btw. Performance Groups",1);
    timeDep->addItem("btw. Floors",2);
    timeDep->addItem("btw. Directions",3);
    timeDep->addItem("btw. Damage States", 4);
    timeDep->addItem("Independent",5);
    timeDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("Reconstruction Times"), timeDep);

    // rec. cost and time
    costAndTimeDep = new QCheckBox();
    costAndTimeDep->setText("");
    costAndTimeDep->setToolTip(tr("Some tooltip"));
    costAndTimeDep->setChecked(false);
    dependencyFormLayout->addRow(tr("between Rec. Cost and Time"),
                                 costAndTimeDep);

     QSpacerItem *spacerGroups11 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups11);

    // injuries
    injuryDep = new QComboBox();
    injuryDep->setToolTip(tr("Some tooltip"));
    injuryDep->addItem("btw. Fragility Groups",0);
    injuryDep->addItem("btw. Performance Groups",1);
    injuryDep->addItem("btw. Floors",2);
    injuryDep->addItem("btw. Directions",3);
    injuryDep->addItem("btw. Damage States", 4);
    injuryDep->addItem("Independent",5);
    injuryDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("Injuries"), injuryDep);

    // injuries and fatalities
    injSeverityDep = new QCheckBox();
    injSeverityDep->setText("");
    injSeverityDep->setToolTip(tr("Some tooltip"));
    injSeverityDep->setChecked(false);
    dependencyFormLayout->addRow(tr("between Injuries and Fatalities"),
                                 injSeverityDep);

     QSpacerItem *spacerGroups12 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups12);

    // red tag
    redTagDep = new QComboBox();
    redTagDep->setToolTip(tr("Some tooltip"));
    redTagDep->addItem("btw. Fragility Groups",0);
    redTagDep->addItem("btw. Performance Groups",1);
    redTagDep->addItem("btw. Floors",2);
    redTagDep->addItem("btw. Directions",3);
    redTagDep->addItem("btw. Damage States", 4);
    redTagDep->addItem("Independent",5);
    redTagDep->setCurrentIndex(5);
    dependencyFormLayout->addRow(tr("Red Tag Probabilities"), redTagDep);

     QSpacerItem *spacerGroups13 = new QSpacerItem(10,10);
    dependencyFormLayout->addItem(spacerGroups13);   

    // assemble the widgets-----------------------------------------------------

    mainVLayout->addWidget(dependencyGroupBox);
    mainVLayout->addStretch(1);
    mainVLayout->setSpacing(10);
    mainVLayout->setMargin(0);

    mainHLayout->addLayout(mainVLayout, 0);
    mainHLayout->addStretch(1);
    mainHLayout->setSpacing(10);
    mainHLayout->setMargin(0);

    mainLayout->addLayout(titleLayout);
    //mainLayout->addItem(spacerTop1);
    mainLayout->addLayout(mainHLayout, 0);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    //mainLayout->setMargin(0);

    this->setLayout(mainLayout);
}

P58DependenciesContainer::~P58DependenciesContainer()
{}

bool P58DependenciesContainer::outputToJSON(QJsonObject &outputObject) {

    QJsonObject dependenciesObj;

    // Dependencies -----------------------------------------------------------
    dependenciesObj["Quantities"] = quantityDep->currentText();
    dependenciesObj["Fragilities"] = fragilityDep->currentText();
    dependenciesObj["ReconstructionCosts"] = costDep->currentText();
    dependenciesObj["ReconstructionTimes"] = timeDep->currentText();
    dependenciesObj["CostAndTime"] = costAndTimeDep->isChecked();
    dependenciesObj["Injuries"] = injuryDep->currentText();
    dependenciesObj["InjurySeverities"] = injSeverityDep->isChecked();
    dependenciesObj["RedTagProbabilities"] = redTagDep->currentText();

    outputObject["Dependencies"] = dependenciesObj;

    return 0;
}

bool P58DependenciesContainer::inputFromJSON(QJsonObject & inputObject) {

    QJsonObject dependenciesObj = inputObject["Dependencies"].toObject();

    // Dependencies -----------------------------------------------------------
    quantityDep->setCurrentText(dependenciesObj["Quantities"].toString());
    fragilityDep->setCurrentText(dependenciesObj["Fragilities"].toString());
    costDep->setCurrentText(dependenciesObj["ReconstructionCosts"].toString());
    timeDep->setCurrentText(dependenciesObj["ReconstructionTimes"].toString());
    costAndTimeDep->setChecked(dependenciesObj["CostAndTime"].toBool());
    injuryDep->setCurrentText(dependenciesObj["Injuries"].toString());
    injSeverityDep->setChecked(dependenciesObj["InjurySeverities"].toBool());
    redTagDep->setCurrentText(dependenciesObj["RedTagProbabilities"].toString());

    return 0;
}

