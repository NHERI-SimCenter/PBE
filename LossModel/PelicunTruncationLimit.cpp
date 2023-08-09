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

// Written: adamzs

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QDebug>
#include <QJsonObject>

#include "PelicunTruncationLimit.h"

TruncationLimit::TruncationLimit(QWidget *parent, QMap<QString, QString> *TL_data_in)
  : SimCenterWidget(parent) {

    TL_data = TL_data_in;

    mainLayout = new QHBoxLayout();

    tlDemandType = new QLineEdit();
    tlDemandType->setToolTip(
      tr("The type of demand that has a truncated marginal distribution.\n"
         "Providing location and direction of the demand is optional.\n"
         "If only the type ID is provided, demands across all locations\n"
         "and directions are truncated."));
    tlDemandType->setMaximumWidth(75);
    tlDemandType->setMinimumWidth(75);
    tlDemandType->setText(TL_data->value("type", tr("")));
    this->storeTLDemandType();
    connect(tlDemandType, SIGNAL(editingFinished()), this,
      SLOT(storeTLDemandType()));


    tlLowerLimit = new QLineEdit();
    tlLowerLimit->setToolTip(
      tr("The lower truncation limit for the given demand type. If left \n"
         "empty, no lower truncation limit is applied."));
    tlLowerLimit->setMaximumWidth(75);
    tlLowerLimit->setMinimumWidth(75);
    tlLowerLimit->setText(TL_data->value("lower", tr("")));
    this->storeTLLowerLimit();
    connect(tlLowerLimit, SIGNAL(editingFinished()), this, SLOT(storeTLLowerLimit()));

    tlUpperLimit = new QLineEdit();
    tlUpperLimit->setToolTip(
      tr("The upper truncation limit for the given demand type. If left \n"
         "empty, no upper truncation limit is applied."));
    tlUpperLimit->setMaximumWidth(75);
    tlUpperLimit->setMinimumWidth(75);
    tlUpperLimit->setText(TL_data->value("upper", tr("")));
    this->storeTLUpperLimit();
    connect(tlUpperLimit, SIGNAL(editingFinished()), this, SLOT(storeTLUpperLimit()));

    // Set up main layout
    mainLayout->addWidget(tlDemandType);
    mainLayout->addWidget(tlLowerLimit);
    mainLayout->addWidget(tlUpperLimit);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0,0,0,0);;

    this->setLayout(mainLayout);
}

TruncationLimit::~TruncationLimit()
{
    this->delete_TL_data();
}

void
TruncationLimit::storeTLDemandType(){
    TL_data -> insert("type", tlDemandType->text());
}

void
TruncationLimit::storeTLLowerLimit(){
    TL_data -> insert("lower", tlLowerLimit->text());
}

void
TruncationLimit::storeTLUpperLimit(){
    TL_data -> insert("upper", tlUpperLimit->text());
}


bool TruncationLimit::outputToJSON(QJsonObject &outputObject) {

    QJsonObject calibData;

    calibData["DistributionFamily"] = "lognormal";
    calibData["TruncateLower"] = tlLowerLimit->text();
    calibData["TruncateUpper"] = tlUpperLimit->text();

    outputObject[tlDemandType->text()] = calibData;

    return false;
}

bool TruncationLimit::inputFromJSON(const QJsonObject & inputObject) {

    return true;
}

void TruncationLimit::delete_TL_data() {
    //delete TL_data;
}
