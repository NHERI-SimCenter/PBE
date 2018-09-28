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

#include "CollapseModeContainer.h"
#include "CollapseMode.h"

#include <QPushButton>
#include <QScrollArea>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QDebug>
#include <sectiontitle.h>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>
#include <QScrollArea>


CollapseModeContainer::CollapseModeContainer(RandomVariableInputWidget *theRV_IW, QWidget *parent)
    : SimCenterAppWidget(parent), theRandVariableIW(theRV_IW)
{
    verticalLayout = new QVBoxLayout();

    // title, add & remove button
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("list of collapse modes"));
    title->setMinimumWidth(250);
    QSpacerItem *spacer1 = new QSpacerItem(50,10);
    QSpacerItem *spacer2 = new QSpacerItem(20,10);

    QPushButton *addCollapseMode = new QPushButton();
    addCollapseMode->setMinimumWidth(75);
    addCollapseMode->setMaximumWidth(75);
    addCollapseMode->setText(tr("add"));
    connect(addCollapseMode,SIGNAL(clicked()),this,SLOT(addInputWidgetExistingCollapseMode()));

    QPushButton *removeCollapseMode = new QPushButton();
    removeCollapseMode->setMinimumWidth(75);
    removeCollapseMode->setMaximumWidth(75);
    removeCollapseMode->setText(tr("remove"));
    connect(removeCollapseMode,SIGNAL(clicked()),this,SLOT(removeInputWidgetExistingCollapseMode()));

    titleLayout->addWidget(title);
    titleLayout->addItem(spacer1);
    titleLayout->addWidget(addCollapseMode);
    titleLayout->addItem(spacer2);
    titleLayout->addWidget(removeCollapseMode);
    titleLayout->addStretch();

    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);

    QWidget *eventsWidget = new QWidget;

    eventLayout = new QVBoxLayout();
    eventLayout->addStretch();
    eventsWidget->setLayout(eventLayout);
    sa->setWidget(eventsWidget);


    verticalLayout->addLayout(titleLayout);
    verticalLayout->addWidget(sa);
    //verticalLayout->addStretch();

    this->setLayout(verticalLayout);
    connect(addCollapseMode, SIGNAL(pressed()), this, SLOT(addCollapseMode()));
    connect(removeCollapseMode, SIGNAL(pressed()), this, SLOT(removeCollapseModes()));
}


CollapseModeContainer::~CollapseModeContainer()
{

}


void CollapseModeContainer::addCollapseMode(void)
{
   CollapseMode *theCollapseMode = new CollapseMode();
   theCollapseModes.append(theCollapseMode);
   eventLayout->insertWidget(eventLayout->count()-1, theCollapseMode);
   connect(this,SLOT(InputWidgetExistingCollapseModeErrorMessage(QString)), theCollapseMode, SIGNAL(sendErrorMessage(QString)));
}


void CollapseModeContainer::removeCollapseModes(void)
{
    // find the ones selected & remove them
    int numInputWidgetExistingCollapseModes = theCollapseModes.size();
    for (int i = numInputWidgetExistingCollapseModes-1; i >= 0; i--) {
      CollapseMode *theCollapseMode = theCollapseModes.at(i);
      if (theCollapseMode->isSelectedForRemoval()) {
          theCollapseMode->close();
          eventLayout->removeWidget(theCollapseMode);
          theCollapseModes.remove(i);
          theCollapseMode->setParent(0);
          delete theCollapseMode;
      }
    }
}


void
CollapseModeContainer::clear(void)
{
  // loop over random variables, removing from layout & deleting
  for (int i = 0; i <theCollapseModes.size(); ++i) {
    CollapseMode *theCollapseMode = theCollapseModes.at(i);
    eventLayout->removeWidget(theCollapseMode);
    delete theCollapseMode;
  }
  theCollapseModes.clear();
}


bool
CollapseModeContainer::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;
    QJsonArray theArray;
    for (int i = 0; i <theCollapseModes.size(); ++i) {
        QJsonObject rv;
        if (theCollapseModes.at(i)->outputToJSON(rv)) {
            theArray.append(rv);

        } else {
            qDebug() << "OUTPUT FAILED" << theCollapseModes.at(i)->getCollapseModeName();
            result = false;
        }
    }
    jsonObject["CollapseModes"]=theArray;
    return result;
}

bool
CollapseModeContainer::inputFromJSON(QJsonObject &rvObject)
{
  bool result = true;

  // clean out current list
  this->clear();

  // get array

  QJsonArray rvArray = rvObject["CollapseModes"].toArray();

  // foreach object in array
  foreach (const QJsonValue &rvValue, rvArray) {

      QJsonObject rvObject = rvValue.toObject();
      CollapseMode *theCollapseMode = new CollapseMode();

      if (theCollapseMode->inputFromJSON(rvObject)) { // this method is where type is set
          theCollapseModes.append(theCollapseMode);
          eventLayout->insertWidget(eventLayout->count()-1, theCollapseMode);
      } else {
          result = false;
      }
  }

  return result;
}

bool 
CollapseModeContainer::copyFiles(QString &dirName) {
    return true;
}

void
CollapseModeContainer::errorMessage(QString message){
    emit sendErrorMessage(message);
}

