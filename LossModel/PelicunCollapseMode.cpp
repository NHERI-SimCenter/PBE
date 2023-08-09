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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QDebug>
#include <QJsonObject>

#include "PelicunCollapseMode.h"

CollapseMode::CollapseMode(QWidget *parent)
  : SimCenterWidget(parent) {

    mainLayout = new QHBoxLayout();

    // Vertical layout to deal with collapse mode name
    QVBoxLayout * nameLayout = new QVBoxLayout();
    QLabel *collapseModeLabel = new QLabel();
    collapseModeLabel->setText(tr("name"));
    collapseModeName = new QLineEdit();
    collapseModeName->setToolTip(tr("Name of the collapse mode."));
    collapseModeName->setMaximumWidth(150);
    collapseModeName->setMinimumWidth(150);
    nameLayout->addWidget(collapseModeLabel);
    nameLayout->addWidget(collapseModeName);
    nameLayout->setSpacing(1);
    nameLayout->setContentsMargins(0,0,0,0);;

    // Vertical layout for collapse mode probability
    QVBoxLayout * probabilityLayout = new QVBoxLayout();
    QLabel *probabilityLabel = new QLabel();
    probabilityLabel->setText(tr("probability"));
    collapseModeProbability = new QLineEdit();
    collapseModeProbability->setToolTip(tr("The probability that the building collapses in this \n"
                                           "mode when it collapses."));
    collapseModeProbability->setMaximumWidth(100);
    collapseModeProbability->setMinimumWidth(100);
    probabilityLayout->addWidget(probabilityLabel);
    probabilityLayout->addWidget(collapseModeProbability);
    probabilityLayout->setSpacing(1);
    probabilityLayout->setContentsMargins(0,0,0,0);;

    // Vertical layout to deal with the area affected by the collapse mode
    QVBoxLayout * affectedAreaLayout = new QVBoxLayout();
    QLabel *collapseModeAffectedAreaLabel = new QLabel();
    collapseModeAffectedAreaLabel->setText(tr("affected area"));
    collapseModeAffectedArea = new QLineEdit();
    collapseModeAffectedArea->setToolTip(tr("The proportion of the total floor area in each story \n"
                                            "affected by this collapse mode.\n"
                                            "If one value is provided, the same proportion is assumed \n"
                                            "to apply to all floors. A list of values allows you to define\n"
                                            "floor-specific proportions."));
    collapseModeAffectedArea->setMaximumWidth(200);
    collapseModeAffectedArea->setMinimumWidth(200);
    affectedAreaLayout->addWidget(collapseModeAffectedAreaLabel);
    affectedAreaLayout->addWidget(collapseModeAffectedArea);
    affectedAreaLayout->setSpacing(1);
    affectedAreaLayout->setContentsMargins(0,0,0,0);

    // Vertical layout to deal with collapse mode injuries
    QVBoxLayout * injuriesLayout = new QVBoxLayout();
    QLabel *collapseModeInjuriesLabel = new QLabel();
    collapseModeInjuriesLabel->setText(tr("injuries"));
    collapseModeInjuries = new QLineEdit();
    collapseModeInjuries->setToolTip(tr("Proportion of the inhabitants in the affected area getting injured\n"
                                        "when this mode of collapse happens. Provide a list of numbers when\n"
                                        "multiple levels of injury severity are considered. (2 for FEMA Pelicun)"));
    collapseModeInjuries->setMaximumWidth(150);
    collapseModeInjuries->setMinimumWidth(150);
    injuriesLayout->addWidget(collapseModeInjuriesLabel);
    injuriesLayout->addWidget(collapseModeInjuries);
    injuriesLayout->setSpacing(1);
    injuriesLayout->setContentsMargins(0,0,0,0);

    button = new QRadioButton();

    // Create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left

    // Set up main layout
    mainLayout->addWidget(button);
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(probabilityLayout);
    mainLayout->addLayout(affectedAreaLayout);
    mainLayout->addLayout(injuriesLayout);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0,0,0,0);

    this->setLayout(mainLayout);
}

CollapseMode::~CollapseMode()
{}

bool CollapseMode::outputToJSON(QJsonObject &outputObject) {

    if (!collapseModeName->text().isEmpty()) {
        outputObject["name"] = collapseModeName->text();
        outputObject["weight"] = collapseModeProbability->text();
        outputObject["affected_area"] = collapseModeAffectedArea->text();
        outputObject["injuries"] = collapseModeInjuries->text();
        return true;
    } else {
        errorMessage("ERROR: Component - cannot output as no \"name\" entry!");
        return false;
    }
}

bool CollapseMode::inputFromJSON(const QJsonObject & inputObject) {

    if (inputObject.contains("name")) {
        collapseModeName->setText(inputObject["name"].toString());
        collapseModeProbability->setText(inputObject["weight"].toString());
        collapseModeAffectedArea->setText(inputObject["affected_area"].toString());
        collapseModeInjuries->setText(inputObject["injuries"].toString());
        return true;
    } else {
        return false;
    }

}

bool CollapseMode::isSelectedForRemoval() const {
  return button->isChecked();
}


QString CollapseMode::getCollapseModeName() const {
     return collapseModeName->text();
}

