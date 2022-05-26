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

#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <sectiontitle.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QComboBox>

#include "PelicunPreferencesContainer.h"
#include "PelicunComponentContainer.h"
#include "PelicunDemandContainer.h"
#include "PelicunDamageContainer.h"
#include "PelicunLossContainer.h"
#include "PelicunCollapseModeContainer.h"
#include "PelicunOutputsContainer.h"
#include "LossMethod.h"

#include <QTabWidget>

#include "PelicunLossModel.h"

PelicunLossModel::PelicunLossModel(QWidget *parent)
    : LossMethod(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    QTabWidget *theTab = new QTabWidget();

    // asset
    contAsset = new PelicunComponentContainer();
    theTab->addTab(contAsset,"Asset");

    // demands
    contDemand = new PelicunDemandContainer();
    theTab->addTab(contDemand, "Demands");

    // damage
    contDamage = new PelicunDamageContainer();
    theTab->addTab(contDamage, "Damage");

    // losses
    contLoss = new PelicunLossContainer();
    theTab->addTab(contLoss, "Losses");

    /*
    // outputs
    contOutputs = new PelicunOutputsContainer();
    theTab->addTab(contOutputs,"Outputs");

    // preferences
    contPreferences = new PelicunPreferencesContainer();
    theTab->addTab(contPreferences,"Preferences");

    // collapse modes
    contCollapseModes = new PelicunCollapseModeContainer();
    theTab->addTab(contCollapseModes,"Collapse Modes");
    */

    layout->addWidget(theTab);
    this->setLayout(layout);
}

PelicunLossModel::~PelicunLossModel()
{

}

bool
PelicunLossModel::outputToJSON(QJsonObject &jsonObject)
{

    bool result = true;

    // need to save data from all widgets
    result = contAsset->outputToJSON(jsonObject);

    result += contDemand->outputToJSON(jsonObject);

    result += contDamage->outputToJSON(jsonObject);

    result += contLoss->outputToJSON(jsonObject);

    return result;
}

bool
PelicunLossModel::inputFromJSON(QJsonObject &jsonObject)
{

    bool result = true;

    // need to load data to all widgets
    result = contAsset->inputFromJSON(jsonObject);

    result += contDemand->inputFromJSON(jsonObject);

    result += contDamage->inputFromJSON(jsonObject);

    result += contLoss->inputFromJSON(jsonObject);

    return result;

}

bool
PelicunLossModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["ComponentClassification"]="Earthquake";
    jsonObject["Application"] = "Pelicun3";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return 0;
}

bool
PelicunLossModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    return 0;
}


bool
PelicunLossModel::copyFiles(QString &dirName) {

    bool result = true;

    // need to load data to all widgets
    result = contAsset->copyFiles(dirName);

    result += contDemand->copyFiles(dirName);

    result += contDamage->copyFiles(dirName);

    result += contLoss->copyFiles(dirName);

    return result;
}

QString
PelicunLossModel::getFragilityFolder(){
    //return contAsset->getFragilityDataBase();
    return 0;
}

QString
PelicunLossModel::getPopulationFile(){
    //return contAsset->getPopulationFile();
    return 0;
}
