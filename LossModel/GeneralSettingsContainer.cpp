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

#include "GeneralSettingsContainer.h"

GeneralSettingsContainer::GeneralSettingsContainer(RandomVariableInputWidget *theRV_IW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandVariableIW(theRV_IW)
{

    mainLayout = new QHBoxLayout();

    // Vertical layout to deal with general name
    QVBoxLayout * nameLayout = new QVBoxLayout();
    QLabel *generalLabel = new QLabel();
    generalLabel->setText(tr("name"));
    generalName = new QLineEdit();
    generalName->setToolTip(tr("Name of the general."));
    generalName->setMaximumWidth(150);
    generalName->setMinimumWidth(150);
    nameLayout->addWidget(generalLabel);
    nameLayout->addWidget(generalName);
    nameLayout->setSpacing(1);
    nameLayout->setMargin(0);

    // Vertical layout for general probability
    QVBoxLayout * probabilityLayout = new QVBoxLayout();
    QLabel *probabilityLabel = new QLabel();
    probabilityLabel->setText(tr("probability"));
    generalProbability = new QLineEdit();
    generalProbability->setToolTip(tr("The probability that the building collapses in this mode when it collapses."));
    generalProbability->setMaximumWidth(100);
    generalProbability->setMinimumWidth(100);
    probabilityLayout->addWidget(probabilityLabel);
    probabilityLayout->addWidget(generalProbability);
    probabilityLayout->setSpacing(1);
    probabilityLayout->setMargin(0);

    // Vertical layout to deal with the area affected by the general
    QVBoxLayout * affectedAreaLayout = new QVBoxLayout();
    QLabel *generalAffectedAreaLabel = new QLabel();
    generalAffectedAreaLabel->setText(tr("affected area"));
    generalAffectedArea = new QLineEdit();
    generalAffectedArea->setToolTip(tr("The proportion of the total floor area in each story affected by this general."));
    generalAffectedArea->setMaximumWidth(200);
    generalAffectedArea->setMinimumWidth(200);
    affectedAreaLayout->addWidget(generalAffectedAreaLabel);
    affectedAreaLayout->addWidget(generalAffectedArea);
    affectedAreaLayout->setSpacing(1);
    affectedAreaLayout->setMargin(0);

    // Vertical layout to deal with general injuries
    QVBoxLayout * injuriesLayout = new QVBoxLayout();
    QLabel *generalInjuriesLabel = new QLabel();
    generalInjuriesLabel->setText(tr("injuries"));
    generalInjuries = new QLineEdit();
    generalInjuries->setToolTip(tr("Proportion of the inhabitants in the affected area getting injured when this mode of collapse happens. Provide a list of numbers when multiple levels of injury severity are considered."));
    generalInjuries->setMaximumWidth(150);
    generalInjuries->setMinimumWidth(150);
    injuriesLayout->addWidget(generalInjuriesLabel);
    injuriesLayout->addWidget(generalInjuries);
    injuriesLayout->setSpacing(1);
    injuriesLayout->setMargin(0);

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
    mainLayout->setMargin(0);

    this->setLayout(mainLayout);
}

GeneralSettingsContainer::~GeneralSettingsContainer()
{}

bool GeneralSettingsContainer::outputToJSON(QJsonObject &outputObject) {

    if (!generalName->text().isEmpty()) {
        outputObject["name"] = generalName->text();
        outputObject["w"] = generalProbability->text();
        outputObject["affected_area"] = generalAffectedArea->text();
        outputObject["injuries"] = generalInjuries->text();        
        return true;
    } else {
        emit sendErrorMessage("ERROR: Component - cannot output as no \"name\" entry!");
        return false;
    }
}

bool GeneralSettingsContainer::inputFromJSON(const QJsonObject & inputObject) {

    if (inputObject.contains("name")) {
        generalName->setText(inputObject["name"].toString());
        generalProbability->setText(inputObject["w"].toString());
        generalAffectedArea->setText(inputObject["affected_area"].toString());
        generalInjuries->setText(inputObject["injuries"].toString());
        return true;
    } else {
        return false;
    }

}

