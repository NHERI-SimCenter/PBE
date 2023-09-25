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

// Written: fmckenna, mhgardner, adamzs

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QDebug>
#include <QJsonObject>

#include "PelicunPopulationGroup.h"

PopulationGroup::PopulationGroup(QWidget *parent, QMap<QString, QString> *PG_data_in)
  : SimCenterWidget(parent) {

    PG_data = PG_data_in;

    mainLayout = new QHBoxLayout();

    pgLocation = new QLineEdit();
    pgLocation->setToolTip(tr("In buildings, locations are typically stories.\n"
                              "The ground floor is story 1. Providing 'all' \n"
                              "assigns the same setting to every story. You can use\n"
                              "a dash to specify a range of stories, such as '3-7'.\n"
                              "If a component is only assigned to the top story, or \n"
                              "the roof, you can use 'top' or 'roof'. You can also\n"
                              "combine these and use '3-roof', for example."));
    pgLocation->setMaximumWidth(75);
    pgLocation->setMinimumWidth(75);
    pgLocation->setText(PG_data->value("location", tr("")));
    this->storePGLocation();
    connect(pgLocation, SIGNAL(editingFinished()), this, SLOT(storePGLocation()));

    pgMedianQNT = new QLineEdit();
    pgMedianQNT->setToolTip(tr("The list of quantities provided here specifies the number\n"
                               "of Component Groups in each Performance Group that is \n"
                               "created by this row.\n"
                               "If the fragility of component groups is assumed perfectly \n"
                               "correlated, there is no good reason to have more than \n"
                               "one Component Group (i.e. more than one quantity value)\n"
                               "defined for each Performance Group below."));
    pgMedianQNT->setMaximumWidth(75);
    pgMedianQNT->setMinimumWidth(75);
    pgMedianQNT->setText(PG_data->value("median", tr("")));
    this->storePGMedian();
    connect(pgMedianQNT, SIGNAL(editingFinished()), this, SLOT(storePGMedian()));

    pgDistribution = new QComboBox();
    pgDistribution->setToolTip(tr("Distribution family assigned to the component quantities.\n"
                                  "The N/A setting corresponds to known quantities with no uncertainty."));
    pgDistribution->addItem(tr("N/A"));
    pgDistribution->addItem(tr("normal"));
    pgDistribution->addItem(tr("lognormal"));
    pgDistribution->setMaximumWidth(100);
    pgDistribution->setMinimumWidth(100);
    pgDistribution->setCurrentText(PG_data->value("distribution", tr("N/A")));
    this->storePGDistribution();
    connect(pgDistribution, SIGNAL(currentIndexChanged(int)), this, SLOT(storePGDistribution()));

    pgDispersion = new QLineEdit();
    pgDispersion->setToolTip(tr("Coefficient of variation for the random distribution of \n"
                         "component quantities.\n"
                         "If the distribution is set to N/A, this field will not \n"
                         "be considered and it can be left blank."));
    pgDispersion->setMaximumWidth(45);
    pgDispersion->setMinimumWidth(45);
    pgDispersion->setText(PG_data->value("dispersion", tr("")));
    this->storePGDispersion();
    connect(pgDispersion, SIGNAL(editingFinished()), this, SLOT(storePGDispersion()));

    pgComment = new QLineEdit();
    pgComment->setToolTip(tr("Notes or comments about this entry.\n"
                             "Only used to provide information here, not used in the calculation."));
    pgComment->setText(PG_data->value("comment", tr("")));
    this->storePGComment();
    connect(pgComment, SIGNAL(editingFinished()), this, SLOT(storePGComment()));

    // Create the main layout inside which we place a spacer & main widget
    // implementation note: spacer added first to ensure it always lines up on left

    // Set up main layout
    mainLayout->addWidget(pgLocation);
    mainLayout->addWidget(pgMedianQNT);
    mainLayout->addWidget(pgDistribution);
    mainLayout->addWidget(pgDispersion);
    mainLayout->addWidget(pgComment, 1);
    mainLayout->addStretch();
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0,0,0,0);

    this->setLayout(mainLayout);
}

PopulationGroup::~PopulationGroup()
{
    this->delete_PG_data();
}

void
PopulationGroup::storePGLocation(){
    PG_data -> insert("location", pgLocation->text());
}

void
PopulationGroup::storePGMedian(){
    PG_data -> insert("median", pgMedianQNT->text());
}

void
PopulationGroup::storePGDistribution(){
    PG_data -> insert("distribution", pgDistribution->currentText());
}

void
PopulationGroup::storePGDispersion(){
    PG_data -> insert("dispersion", pgDispersion->text());
}

void
PopulationGroup::storePGComment(){
    PG_data -> insert("comment", pgComment->text());
}

bool PopulationGroup::outputToJSON(QJsonObject &outputObject) {

    return true;
}

bool PopulationGroup::inputFromJSON(const QJsonObject & inputObject) {

    return true;
}

//bool PopulationGroup::isSelectedForRemoval() const {
//  return button->isChecked();
//}


QString PopulationGroup::getPopulationName() const {
     //return componentName->text();
    return "";
}

void PopulationGroup::delete_PG_data() {
    delete PG_data;
}
