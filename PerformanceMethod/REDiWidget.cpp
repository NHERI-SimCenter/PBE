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

// Written: Stevan Gavrilovic

#include "REDiWidget.h"
#include <QFormLayout>
#include <QGroupBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QComboBox>
#include <QDebug>
#include <QList>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>>


REDiWidget::REDiWidget(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    riskParamsLE = new QLineEdit(this);
    riskParamsLE->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QString linkText = "here";
    QString url = "https://sgavrilovicarup.github.io/REDi-docs/#";
    QString htmlLink = QString("<a href=\"%1\">%2</a>").arg(url, linkText);

    auto infoTxt = QString("Resilience-based Engineering Design Initiative (REDi™) Seismic Building Recovery Methodology by ARUP. For more information see ")+htmlLink;

    setWindowTitle(infoTxt);

    auto redi_label = new QLabel(infoTxt);
    redi_label->setOpenExternalLinks(true);

    layout->addWidget(redi_label,0,Qt::AlignCenter);

    auto rpTextLabel = new QLabel("Path to risk parameters file:");

    auto browseButton = new QPushButton("Browse");
    browseButton->setFixedWidth(100);
    connect(browseButton, &QPushButton::pressed, this, &REDiWidget::handleBrowseButtonPressed);

    QHBoxLayout* inputLayout = new QHBoxLayout(this);
    inputLayout->addWidget(rpTextLabel);
    inputLayout->addWidget(riskParamsLE);
    inputLayout->addWidget(browseButton);

    layout->addLayout(inputLayout);
    layout->addStretch(1);

}


REDiWidget::~REDiWidget()
{

}


bool REDiWidget::outputToJSON(QJsonObject &jsonObj){

    jsonObj["Application"] = "REDi";

    return true;
}


bool REDiWidget::inputFromJSON(QJsonObject &jsonObject){

    Q_UNUSED(jsonObject);
    return true;
}



bool REDiWidget::outputAppDataToJSON(QJsonObject &jsonObj){

    jsonObj["Application"] = "REDi";
    QJsonObject dataObj;
    auto riskParamPath = riskParamsLE->text().trimmed();
    dataObj["riskParametersPath"] = riskParamPath;
    jsonObj["ApplicationData"] = dataObj;

    return true;
}


bool REDiWidget::inputAppDataFromJSON(QJsonObject &jsonObject){

    if (jsonObject.contains("ApplicationData"))
    {
        if (jsonObject["ApplicationData"].toObject().contains("riskParametersPath"))
        {
            QJsonValue riskParamsPathValue = jsonObject["ApplicationData"].toObject()["riskParametersPath"];

            if (riskParamsPathValue.isNull())
                return false;

            riskParamsLE->setText(riskParamsPathValue.toString());
            return true;
        }
    }

    return false;
}

bool
REDiWidget::outputCitation(QJsonObject &jsonObject)
{
  QJsonObject citationREDi;
  citationREDi.insert("citation",
"ARUP, pyREDi version 0.1.0, \
https://github.com/arup-group/REDi, 2023");
  citationREDi.insert("description",
"This reference indicates the version of the tool used for the simulation.");

  QJsonObject citationREDiMarker;
  citationREDiMarker.insert("citation",
"Merrifield S. and Almufti I., \
REDi Rating System: Downtime Assessment Methodology, ARUP, 2013");
  citationREDiMarker.insert("description",
"This document describes the REDi downtime assessment methodology. Please reference \
it if your workflow in the SimCenter tools includes the REDi methodology.");

  QJsonArray citationsArray;
  citationsArray.push_back(citationREDi);
  citationsArray.push_back(citationREDiMarker);

  jsonObject.insert("citations", citationsArray);
  
  return true;
}


void REDiWidget::clear(void)
{
    riskParamsLE->clear();
}


void REDiWidget::handleBrowseButtonPressed()
{

}





