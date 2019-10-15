#ifndef COMPONENT_CONTAINER_H
#define COMPONENT_CONTAINER_H

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

#include <SimCenterWidget.h>
#include <SimCenterAppWidget.h>

class ComponentGroup;

class QVBoxLayout;
class QLineEdit;
class QLabel;
class Component;
class QComboBox;
class QGroupBox;
class QStringListModel;

class P58ComponentContainer : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit P58ComponentContainer(QWidget *parent = 0);

    ~P58ComponentContainer();

    bool inputFromJSON(QJsonObject &jsonObject);
    bool outputToJSON(QJsonObject &jsonObject);
    bool copyFiles(QString &dirName);

    int setFragilityFolder(QString fragilityFolder);
    QString getFragilityFolder();

    void addCompToOverview(QString compName, QVBoxLayout *compAreaVBox);

    void addComponent();
    void removeComponent();

    void clearCompGroupWidget();
    void retrieveCompGroups();

public slots:
   void errorMessage(QString message);

   int updateAvailableComponents();

   void addOneComponent(void);
   void addAllComponents(void);
   void removeOneComponent(void);
   void removeAllComponents(void);
   void showSelectedComponent(void);

   void addComponentGroup(QMap<QString, QString> *CG_data_in=nullptr);
   void removeComponentGroup();

   void onLoadConfigClicked(void);
   void onSaveConfigClicked(void);

   void chooseFragilityFolder(void);

   /*
   void storeCompQuantity(void);
   void retrieveCompQuantity(void);

   void storeCompQuantityDist(void);
   void retrieveCompQuantityDist(void);

   void storeCompQuantityCOV(void);
   void retrieveCompQuantityCOV(void);

   void storeWeightD1(void);
   void retrieveWeightD1(void);
   void storeWeightD2(void);
   void retrieveWeightD2(void);

   void refreshCompOverview(void);
   */

private:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *eventLayout;
    QVBoxLayout *loCGList;

    QLineEdit * fragilityFolderPath;

    QComboBox *availableCompCombo;
    QStringListModel *availableCBModel;
    QComboBox *selectedCompCombo;
    QStringListModel *selectedCBModel;

    QLabel *compName;
    QLabel *compDescription;
    QLabel *compEDP;
    QLabel *compUnit;

    QLineEdit *compQuantity;
    QLabel *compUnitSize;
    QLabel *compUnitType;

    QComboBox *compQFamily;
    QLineEdit *compQcov;

    QLineEdit *dir1Weights;
    QLineEdit *dir2Weights;

    QGroupBox *compOverviewGroupBox;
    QVBoxLayout *compOverviewLayout;

    // component properties
    QMap<QString, QVector<QMap<QString, QString>* >* > *compConfig;

    /*
    QMap<QString, QVector<QString>> *compLocationMap;
    QMap<QString, QVector<QString>> *compDirectionMap;
    QMap<QString, QVector<QString>> *compCGMedianMap;
    QMap<QString, QVector<QString>> *compCGUnitMap;
    QMap<QString, QVector<QString>> *compCGDistributionMap;
    QMap<QString, QVector<QString>> *compCGCOVMap;
    */

    QVector<Component *>theComponents;
    QVector<ComponentGroup *>vComponentGroups;

};

#endif // COMPONENT_CONTAINER_H
