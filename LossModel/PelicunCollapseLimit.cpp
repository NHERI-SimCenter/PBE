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

#include "PelicunCollapseLimit.h"

CollapseLimit::CollapseLimit(QWidget *parent, QMap<QString, QString> *CL_data_in)
  : SimCenterWidget(parent) {

    CL_data = CL_data_in;

    mainLayout = new QHBoxLayout();

    clDemandType = new QLineEdit();
    clDemandType->setToolTip(
      tr("The type of demand that triggers collapse.\n"
         "Providing location and direction of the demand is optional.\n"
         "If only the type ID is provided, demands across all locations\n"
         "and directions are checked against the specified limit."));
    clDemandType->setMaximumWidth(75);
    clDemandType->setMinimumWidth(75);
    clDemandType->setMinimumHeight(20);
    clDemandType->setText(CL_data->value("type", tr("")));
    this->storeCLDemandType();
    connect(clDemandType, SIGNAL(editingFinished()), this, SLOT(storeCLDemandType()));


    clDemandLimit = new QLineEdit();
    clDemandLimit->setToolTip(
      tr("The magnitude of the given demand type that triggers collapse."));
    clDemandLimit->setMaximumWidth(75);
    clDemandLimit->setMinimumWidth(75);
    clDemandLimit->setMinimumHeight(20);
    clDemandLimit->setText(CL_data->value("limit", tr("")));
    this->storeCLDemandLimit();
    connect(clDemandLimit, SIGNAL(editingFinished()), this, SLOT(storeCLDemandLimit()));

    // Set up main layout
    mainLayout->addWidget(clDemandType);
    mainLayout->addWidget(clDemandLimit);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0,0,0,0);;

    this->setLayout(mainLayout);
}

CollapseLimit::~CollapseLimit()
{
    this->delete_CL_data();
}

void
CollapseLimit::storeCLDemandType(){
    CL_data -> insert("type", clDemandType->text());
}

void
CollapseLimit::storeCLDemandLimit(){
    CL_data -> insert("limit", clDemandLimit->text());
}

bool CollapseLimit::outputToJSON(QJsonObject &outputObject) {   

    outputObject[clDemandType->text()] = clDemandLimit->text();

    return false;
}

bool CollapseLimit::inputFromJSON(const QJsonObject & inputObject) {

    return true;
}

void CollapseLimit::delete_CL_data() {
    //delete CL_data;
}
