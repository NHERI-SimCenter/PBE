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

#include "PelicunResidualParams.h"

ResidualParam::ResidualParam(QWidget *parent, QMap<QString, QString> *RP_data_in)
  : SimCenterWidget(parent) {

    RP_data = RP_data_in;

    mainLayout = new QHBoxLayout();

    rpDirection = new QLineEdit();
    rpDirection->setToolTip(
      tr("Structural systems and structural behavior might be different\n"
         "in different horizontal directions. These panels allow you to\n"
         "provide direction-specific parameters. If you prefer to use the\n"
         "same parameters for every direction, use \"all\" for this field."));
    rpDirection->setMaximumWidth(75);
    rpDirection->setMinimumWidth(75);
    rpDirection->setMinimumHeight(20);
    rpDirection->setText(RP_data->value("dir", tr("")));
    this->storeRPDirection();
    connect(rpDirection, SIGNAL(editingFinished()), this,
      SLOT(storeRPDirection()));


    rpYieldDrift = new QLineEdit();
    rpYieldDrift->setToolTip(
      tr("Interstory drift ratio corresponding to significant yielding in \n"
         "the structure (i.e. when the forces in the main components of the\n"
         "lateral load resisting system reach the plastic capacity of the \n"
         "components)"));
    rpYieldDrift->setMaximumWidth(100);
    rpYieldDrift->setMinimumWidth(100);
    rpYieldDrift->setMinimumHeight(20);
    rpYieldDrift->setText(RP_data->value("delta_yield", tr("")));
    this->storeRPYieldDrift();
    connect(rpYieldDrift, SIGNAL(editingFinished()), this, SLOT(storeRPYieldDrift()));

    // Set up main layout
    mainLayout->addWidget(rpDirection);
    mainLayout->addWidget(rpYieldDrift);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0,0,0,0);;

    this->setLayout(mainLayout);
}

ResidualParam::~ResidualParam()
{
    this->delete_RP_data();
}

void
ResidualParam::storeRPDirection(){
    RP_data -> insert("dir", rpDirection->text());
}

void
ResidualParam::storeRPYieldDrift(){
    RP_data -> insert("delta_yield", rpYieldDrift->text());
}


bool ResidualParam::outputToJSON(QJsonObject &outputObject) {

    outputObject[rpDirection->text()] = rpYieldDrift->text();

    return false;
}

bool ResidualParam::inputFromJSON(const QJsonObject & inputObject) {

    return true;
}

void ResidualParam::delete_RP_data() {
    //delete RP_data;
}
