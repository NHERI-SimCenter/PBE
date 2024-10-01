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
#include <SectionTitle.h>

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

    // outputs
    contOutputs = new PelicunOutputsContainer();
    theTab->addTab(contOutputs,"Outputs");

    /*
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
  qDebug() << "PelicunLossModel::outputToJSON() - start\n";
    bool result = true;

    // need to save data from all widgets
    result = contAsset->outputToJSON(jsonObject);
    if (result == false) {
      qDebug() << "PelicunLossModel::outputToJSON() - contAsset failed\n";
      return false;      
    }

    result += contDemand->outputToJSON(jsonObject);
    if (result == false) {
      qDebug() << "PelicunLossModel::outputToJSON() - contDemand failed\n";
      return false;      
    }    

    result += contDamage->outputToJSON(jsonObject);
    if (result == false) {
      qDebug() << "PelicunLossModel::outputToJSON() - contDamage failed\n";
      return false;      
    }        

    result += contLoss->outputToJSON(jsonObject);
    if (result == false) {
      qDebug() << "PelicunLossModel::outputToJSON() - contLoss failed\n";
      return false;
    }  

    result += contOutputs->outputToJSON(jsonObject);
    if (result == false) {
      qDebug() << "PelicunLossModel::outputToJSON() - contOutputs failed\n";
      return false;
    }           

    qDebug() << "PelicunLossModel::outputToJSON() - end, result = " << result;
    
    return result;
}

bool
PelicunLossModel::inputFromJSON(QJsonObject &jsonObject)
{

    bool result = false;

    // need to load data to all widgets
    result = contAsset->inputFromJSON(jsonObject);

    result += contDemand->inputFromJSON(jsonObject);

    result += contDamage->inputFromJSON(jsonObject);

    result += contLoss->inputFromJSON(jsonObject);

    result += contOutputs->inputFromJSON(jsonObject);

    return result;

}

bool
PelicunLossModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    //jsonObject["ComponentClassification"]="Earthquake";
    jsonObject["Application"] = "Pelicun3";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;

    return true;
}

bool
PelicunLossModel::inputAppDataFromJSON(QJsonObject &jsonObject) {

    return true;
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

bool
PelicunLossModel::outputCitation(QJsonObject &jsonObject)
{
  QJsonObject citationPelicun;
  citationPelicun.insert("citation",
"Adam Zsarnoczay, John Vouvakis Manousakis, Jinyan Zhao, Kuanshi Zhong, \
Pouria Kourehpaz (2024). NHERI-SimCenter/pelicun: v3.3. \
Zenodo. https://doi.org/10.5281/zenodo.10896145");
  citationPelicun.insert("description",
"This reference indicates the version of the tool used for the simulation.");

  QJsonObject citationPelicunMarker;
  citationPelicunMarker.insert("citation",
"Adam Zsarnoczay, Gregory G. Deierlein, \
PELICUN - A Computational Framework for Estimating Damage, Loss, and Community \
Resilience, \
Proceedings of the 17th World Conference on Earthquake Engineering, Japan, 2020");
  citationPelicunMarker.insert("description",
"This paper describes the Pelicun damage and loss assessment framework. Please \
reference it if your work results from using the Pelicun engine in the SimCenter \
tools.");

  QJsonArray citationsArray;
  citationsArray.push_back(citationPelicun);
  citationsArray.push_back(citationPelicunMarker);

  jsonObject.insert("citations", citationsArray);
  
  return true;
}
