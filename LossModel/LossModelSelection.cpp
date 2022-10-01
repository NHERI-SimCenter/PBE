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
#include <QScrollArea>

#include "LossModelSelection.h"
#include <GoogleAnalytics.h>

LossModelSelection::LossModelSelection(

    QWidget *parent)
    : SimCenterAppWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout();

    QHBoxLayout *theSelectionLayout = new QHBoxLayout();

    // set up the selection part

    SectionTitle *label=new SectionTitle();
    label->setMinimumWidth(250);
    label->setText(QString("Damage and Loss Assessment"));

    dlSelection = new QComboBox();
    dlSelection->setObjectName("DLMethodCombox");

    dlSelection->addItem(tr("Pelicun"));
    dlSelection->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    dlSelection->setItemData(1, "Use the Pelicun integrated performance assessment framework.", Qt::ToolTipRole);

    theSelectionLayout->addWidget(label);
    QSpacerItem *spacer = new QSpacerItem(50,10);
    theSelectionLayout->addItem(spacer);
    theSelectionLayout->addWidget(dlSelection);
    theSelectionLayout->addStretch();
    layout->addLayout(theSelectionLayout);

    // create stacked widget

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    theStackedWidget = new QStackedWidget();

    sa->setWidget(theStackedWidget);

    // create individual DL widgets and add them to the stacked widget

    // Adding Pelicun DL widget
    thePelicunWidget = new PelicunLossModel();
    theStackedWidget->addWidget(thePelicunWidget);

    // add the stacked widget to the main layout

    layout->addWidget(sa);
    layout->setMargin(0);
    this->setLayout(layout);
    theCurrentMethod=thePelicunWidget;

    // connect signal and slots

    connect(dlSelection, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(dlSelectionChanged(QString)));

    // set pelicun as the default
    //dlSelection->setCurrentText("HAZUS MH");
    //this->dlSelectionChanged(QString("HAZUS MH"));
    //layout->setMargin(0);
}

LossModelSelection::~LossModelSelection()
{

}

void LossModelSelection::clear(void)
{

}

bool
LossModelSelection::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    result = theCurrentMethod->outputToJSON(jsonObject);

    return result;
}

bool
LossModelSelection::inputFromJSON(QJsonObject &jsonObject)
{
    bool result = false;

    if (theCurrentMethod != 0) {
        result = theCurrentMethod->inputFromJSON(jsonObject);
    }

    return result;
}

bool
LossModelSelection::outputAppDataToJSON(QJsonObject &jsonObject, QJsonObject &lossModelObject) {

    //
    // per API, need to add name of application to be called in AppLication
    // and all data to be used in ApplicationDate
    //

    bool result = true;

    if (theCurrentMethod != 0) {
        result = theCurrentMethod->outputAppDataToJSON(jsonObject);
    }

    return result;
}

bool
LossModelSelection::inputAppDataFromJSON(QJsonObject &jsonObject) {

    bool result = true;

    if (jsonObject.contains("Application")) {
        QJsonValue theDLAppName = jsonObject["Application"].toString();

        if (theDLAppName == QString("Pelicun3")) {
            dlSelection->setCurrentText("Pelicun");

        } else {
            // show a warning that the application is not recognized
            // TODO

            // default

            // this might seem silly, but it sets the template for future expansion
            dlSelection->setCurrentText("Pelicun");
        }

        // invoke inputAppDataFromJSON on new DL method
        if (theCurrentMethod != 0 && !jsonObject.isEmpty()) {
            result = theCurrentMethod->inputAppDataFromJSON(jsonObject);
        }

    }

    return result;
}

void LossModelSelection::dlSelectionChanged(const QString &arg1)
{

    //
    // switch stacked widgets depending on text
    // note type output in json and name in pull down are not the same and hence the ||
    //

    if (arg1 == QString("Pelicun")) {

        theStackedWidget->setCurrentIndex(0);
        theCurrentMethod = thePelicunWidget;

    } else {
        errorMessage("ERROR: Loss Input - no valid Method provided .. keeping old");
        qDebug() << "ERROR: Loss Input - no valid Method provided .. keeping old" << arg1;
    }

    return;
}

bool
LossModelSelection::copyFiles(QString &destDir) {

    bool result = true;

    if (theCurrentMethod != 0) {
       GoogleAnalytics::ReportAppUsage("DL-Pelicun");
       return  theCurrentMethod->copyFiles(destDir);
    }

    return result;
}


QString
LossModelSelection::getFragilityFolder(){
    return theCurrentMethod->getFragilityFolder();
}

QString
LossModelSelection::getPopulationFile(){
    return theCurrentMethod->getPopulationFile();
}
