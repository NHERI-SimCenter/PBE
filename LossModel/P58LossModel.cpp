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

#include "P58GeneralSettingsContainer.h"
#include "P58ComponentContainer.h"
#include "P58CollapseModeContainer.h"
#include "P58DependenciesContainer.h"
#include "LossMethod.h"

#include <QTabWidget.h>

#include "P58LossModel.h"

P58LossModel::P58LossModel(QWidget *parent)
    : LossMethod(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();
    QTabWidget *theTab = new QTabWidget();

    // general settings
    contGeneralSettings = new P58GeneralSettingsContainer();
    theTab->addTab(contGeneralSettings,"General");

    // component definition
    // coming soon...

    // component quantities
    contComponents = new P58ComponentContainer();
    theTab->addTab(contComponents,"Components");
    
    // collapse modes
    contCollapseModes = new P58CollapseModeContainer();
    theTab->addTab(contCollapseModes,"Collapse Modes");

    // dependencies
    contDependencies = new P58DependenciesContainer();
    theTab->addTab(contDependencies,"Dependencies");

    layout->addWidget(theTab);
    this->setLayout(layout);
}

P58LossModel::~P58LossModel()
{

}

bool
P58LossModel::outputToJSON(QJsonObject &jsonObject)
{
    // set the type of analysis
    jsonObject["_method"] = "FEMA P58";

    // need to save data from all widgets
    contGeneralSettings->outputToJSON(jsonObject);

    contComponents->outputToJSON(jsonObject);

    contCollapseModes->outputToJSON(jsonObject);

    contDependencies->outputToJSON(jsonObject);

    return true;
}

bool
P58LossModel::inputFromJSON(QJsonObject &jsonObject)
{
    //qDebug() << "DecisionVariables";
    contGeneralSettings->inputFromJSON(jsonObject);
    if ((jsonObject.contains("ResponseModel")) &&
        (jsonObject.contains("DamageModel")) &&
        (jsonObject.contains("LossModel"))) {
        contGeneralSettings->inputFromJSON(jsonObject);
    } else
        return false;

    //qDebug() << "Components";
    if (jsonObject.contains("Components")) {
        contComponents->inputFromJSON(jsonObject);
    } else
        return false;

    //qDebug() << "CollapseModes";
    if (jsonObject.contains("CollapseModes")) {
        contCollapseModes->inputFromJSON(jsonObject);
    } else
        return false;

    if (jsonObject.contains("Dependencies"))
        contDependencies->inputFromJSON(jsonObject);
        
    return 0;
}

bool
P58LossModel::outputAppDataToJSON(QJsonObject &jsonObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    jsonObject["ComponentClassification"]="Earthquake";
    jsonObject["Application"] = "ComponentContainer";
    QJsonObject dataObj;
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool
P58LossModel::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}


bool 
P58LossModel::copyFiles(QString &dirName) {
    return true;
}

void
P58LossModel::errorMessage(QString message){
    emit sendErrorMessage(message);
}

QString
P58LossModel::getFragilityFolder(){
    return contComponents->getFragilityFolder();
}

QString
P58LossModel::getPopulationFile(){
    return contGeneralSettings->getPopulationFile();
}
