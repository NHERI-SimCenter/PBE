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

#include "LossModelContainer.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <sectiontitle.h>

#include <QVBoxLayout>
#include <QStackedWidget>
#include <QComboBox>

#include "ComponentContainer.h"
#include "CollapseModeContainer.h"

LossModelContainer::LossModelContainer(
    RandomVariableInputWidget *theRV_IW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandVariableIW(theRV_IW)
{
    QVBoxLayout *layout = new QVBoxLayout();

    // add the combo with loss model input types

    QHBoxLayout *theLossSettingsLayout = new QHBoxLayout();
    QLabel *label = new QLabel();
    label->setText(QString("Loss Model Settings"));
    eventSelection = new QComboBox();
    eventSelection->addItem(tr("General"));
    eventSelection->addItem(tr("Components"));
    eventSelection->addItem(tr("Collapse modes"));    

    theLossSettingsLayout->addWidget(label);
    theLossSettingsLayout->addWidget(eventSelection);
    theLossSettingsLayout->addStretch();
    layout->addLayout(theLossSettingsLayout);

    // add a stacked widget for the individual settings widgets

    theStackedWidget = new QStackedWidget();

    // general (components is a placeholder now)
    theComponentContainer = new ComponentContainer(theRV_IW);
    theStackedWidget->addWidget(theComponentContainer);

    // components
    theComponentContainer = new ComponentContainer(theRV_IW);
    theStackedWidget->addWidget(theComponentContainer);
    
    // collapse modes
    theCollapseModeContainer = new CollapseModeContainer(theRV_IW);
    theStackedWidget->addWidget(theCollapseModeContainer);

    layout->addWidget(theStackedWidget);
    this->setLayout(layout);
    theCurrentEvent=theExistingEvents;

    connect(eventSelection, SIGNAL(currentIndexChanged(QString)), this,
        SLOT(eventSelectionChanged(QString)));
}

LossModelContainer::~LossModelContainer()
{

}

bool
LossModelContainer::outputToJSON(QJsonObject &jsonObject)
{
    QJsonObject componentData;
    QJsonObject collapseModeData;

    // need to save data from all widgets
    theComponentContainer->outputToJSON(jsonObject);

    theCollapseModeContainer->outputToJSON(jsonObject);

    // other loss settings will come here

    return true;
}

bool
LossModelContainer::inputFromJSON(QJsonObject &jsonObject)
{
    if (jsonObject.contains("Components")) {
        if (jsonObject["Components"].isArray()) {
            theComponentContainer->inputFromJSON(jsonObject);
        } else
            return false;
    } else
        return false;

    if (jsonObject.contains("CollapseModes")) {
        if (jsonObject["CollapseModes"].isArray()) {
            theCollapseModeContainer->inputFromJSON(jsonObject);
        } else
            return false;
    } else
        return false;

    // other loss settings will come here

    return 0;
}

bool
LossModelContainer::outputAppDataToJSON(QJsonObject &jsonObject) {

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
LossModelContainer::inputAppDataFromJSON(QJsonObject &jsonObject) {
    return true;
}

void
LossModelContainer::eventSelectionChanged(const QString &arg1)
{
    if (arg1 == "Components") {
        theStackedWidget->setCurrentIndex(1);
        theCurrentEvent = theExistingEvents;
    } 

    else if (arg1 == "Collapse modes") {
        theStackedWidget->setCurrentIndex(2);
        theCurrentEvent = theExistingEvents;
    }

    // other settings will come here

    else {
        qDebug() << 
        "ERROR .. LossModelContainer selection .. type unknown: " << arg1;
    }
}

bool 
LossModelContainer::copyFiles(QString &dirName) {
    return true;
}

void
LossModelContainer::errorMessage(QString message){
    emit sendErrorMessage(message);
}

