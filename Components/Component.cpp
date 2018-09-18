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

// Written: fmckenna
// Adapted: mhgardner

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QDebug>
#include <QJsonObject>

#include "Component.h"

Component::Component(QWidget *parent)
  : SimCenterWidget(parent) {

    mainLayout = new QHBoxLayout();

    // Vertical layout to deal with component name
    QVBoxLayout * nameLayout = new QVBoxLayout();
    QLabel *componentLabel = new QLabel();
    componentLabel->setText(tr("Component Name"));
    componentName = new QLineEdit();
    componentName->setToolTip(tr("Name of component"));
    componentName->setMaximumWidth(100);
    componentName->setMinimumWidth(100);
    nameLayout->addWidget(componentLabel);
    nameLayout->addWidget(componentName);
    nameLayout->setSpacing(1);
    nameLayout->setMargin(0);

    // Vertical layout to deal with component quantity
    QVBoxLayout * quantityLayout = new QVBoxLayout();
    QLabel *quantityLabel = new QLabel();
    quantityLabel->setText(tr("Quantity"));
    componentQuantity = new QLineEdit();
    componentQuantity->setToolTip(tr("Quantity of component"));
    componentQuantity->setMaximumWidth(100);
    componentQuantity->setMinimumWidth(100);
    quantityLayout->addWidget(quantityLabel);
    quantityLayout->addWidget(componentQuantity);
    quantityLayout->setSpacing(1);
    quantityLayout->setMargin(0);

    // Vertical layout to deal with component coefficient of variation
    QVBoxLayout * covLayout = new QVBoxLayout();
    QLabel *covLabel = new QLabel();
    covLabel->setText("COV");
    componentCov = new QLineEdit();
    componentCov->setToolTip(tr("Coefficient of variation for component"));
    componentCov->setMaximumWidth(100);
    componentCov->setMinimumWidth(100);
    covLayout->addWidget(covLabel);
    covLayout->addWidget(componentCov);
    covLayout->setSpacing(1);
    covLayout->setMargin(0);
    
    // Provide the selectable options & connect the combo boxes selection
    // signal to this class's unitsChanged slot method
    QVBoxLayout * unitLayout = new QVBoxLayout();
    QLabel *unitLabel = new QLabel();

    unitLabel->setText(tr("Units"));
    unitComboBox = new QComboBox();
    unitComboBox->setToolTip(tr("Units for component"));
    unitComboBox->addItem(tr("Linear Feet"));
    unitComboBox->addItem(tr("Square Feet"));
    unitLayout->addWidget(unitLabel);
    unitLayout->addWidget(unitComboBox);
    unitLayout->setSpacing(1);
    unitLayout->setMargin(0);    

    // Vertical layout for structural/non-structural components
    QVBoxLayout * structuralLayout = new QVBoxLayout();
    QLabel *structuralLabel = new QLabel();
    structuralLabel->setText(tr("Structural Component"));
    structuralCheckBox = new QCheckBox();
    structuralCheckBox->setToolTip(tr("Check this box if component is structural"));
    structuralCheckBox->setChecked(false);
    structuralLayout->addWidget(structuralLabel);
    structuralLayout->addWidget(structuralCheckBox);
    structuralLayout->setSpacing(1);
    structuralLayout->setMargin(0);    

    // Vertical layout for component directions
    QVBoxLayout * directionsLayout = new QVBoxLayout();
    QLabel *directionsLabel= new QLabel();

    directionsLabel->setText(tr("Directions"));
    componentDirections = new QLineEdit();
    componentDirections->setToolTip(tr("Directionts to include component in"));
    directionsLayout->addWidget(directionsLabel);
    directionsLayout->addWidget(componentDirections);
    directionsLayout->setSpacing(1);
    directionsLayout->setMargin(0);

    // Vertical layout for component weights
    QVBoxLayout * weightsLayout = new QVBoxLayout();
    QLabel *weightsLabel = new QLabel();
    weightsLabel->setText(tr("Weights"));
    componentWeights = new QLineEdit();
    componentWeights->setToolTip(tr("Weight for component in each input direction"));
    weightsLayout->addWidget(weightsLabel);
    weightsLayout->addWidget(componentWeights);
    weightsLayout->setSpacing(1);
    weightsLayout->setMargin(0);
    

    button = new QRadioButton();

    // Create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left



    // Set up main layout
    mainLayout->addWidget(button);
    mainLayout->addLayout(nameLayout);
    mainLayout->addLayout(quantityLayout);
    mainLayout->addLayout(covLayout);
    mainLayout->addLayout(unitLayout);
    mainLayout->addLayout(structuralLayout);
    mainLayout->addLayout(directionsLayout);
    mainLayout->addLayout(weightsLayout);    
    mainLayout->addStretch();    
    mainLayout->setSpacing(10);
    mainLayout->setMargin(0);

    this->setLayout(mainLayout);
}

Component::~Component()
{}

bool Component::outputToJSON(QJsonObject &outputObject) {

    if (!componentName->text().isEmpty()) {
        outputObject["name"] = componentName->text();
        outputObject["quantity"] = componentQuantity->text();
        outputObject["cov"] = componentCov->text();
        outputObject["unit"] = unitComboBox->currentText();
        outputObject["structural"] = structuralCheckBox->isChecked();
        outputObject["directions"] = componentDirections->text();
        outputObject["weights"] = componentWeights->text();
        return true;
    } else {
        emit sendErrorMessage("ERROR: Component - cannot output as no \"name\" entry!");
        return false;
    }
    return true;
}

bool Component::inputFromJSON(const QJsonObject & inputObject) {

    if (inputObject.contains("name")) {
        componentName->setText(inputObject["name"].toString());
        componentQuantity->setText(inputObject["quantity"].toString());
        componentCov->setText(inputObject["cov"].toString());
        unitComboBox->setCurrentText(inputObject["unit"].toString());
        structuralCheckBox->setChecked(inputObject["structural"].toBool());
        componentDirections->setText(inputObject["directions"].toString());
        componentWeights->setText(inputObject["weights"].toString());
        return true;
    } else {
        return false;
    }
    return true;
}

bool Component::isSelectedForRemoval() const {
  return button->isChecked();
}


QString Component::getComponentName() const {
     return componentName->text();
}

