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

#include "P58ComponentContainer.h"
#include "P58Component.h"

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
#include <QGroupBox>
#include <QFormLayout>
#include <QComboBox>
#include <QStringListModel>
#include <QJsonDocument>
#include <QHBoxLayout>
#include <QMap>
#include <QCoreApplication>
#include <QSettings>

P58ComponentContainer::P58ComponentContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    verticalLayout = new QVBoxLayout();
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QVBoxLayout *verticalC1Layout = new QVBoxLayout();
    QVBoxLayout *verticalC2Layout = new QVBoxLayout();

    // title
    QHBoxLayout *titleLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Define building components"));
    title->setMinimumWidth(250);

    titleLayout->addWidget(title);
    titleLayout->addStretch();

    // component selection
    QGroupBox *compListGroupBox = new QGroupBox("Component Selection");
    compListGroupBox->setMaximumWidth(600);
    QFormLayout *compListFormLayout = new QFormLayout(compListGroupBox);

    // data folder
    QHBoxLayout *customFolderLayout = new QHBoxLayout();

    fragilityFolderPath = new QLineEdit;

    customFolderLayout->addWidget(fragilityFolderPath);

    QPushButton *chooseFragility = new QPushButton();
    chooseFragility->setText(tr("Choose"));
    connect(chooseFragility, SIGNAL(clicked()),this,SLOT(chooseFragilityFolder()));
    connect(fragilityFolderPath, SIGNAL(textChanged(QString)),this,SLOT(updateAvailableComponents()));

    customFolderLayout->addWidget(chooseFragility);

    compListFormLayout->addRow(tr("DL Data Folder: "),
                               customFolderLayout);

    QSpacerItem *spacerGroups1 = new QSpacerItem(10,10);
    compListFormLayout->addItem(spacerGroups1);

    // available components
    QHBoxLayout *availableCLayout = new QHBoxLayout();

    availableCompCombo = new QComboBox();
    availableCBModel = new QStringListModel();
    availableCompCombo->setModel(availableCBModel);
    availableCompCombo->setToolTip(tr("List of available components. Click on the Add button to add the component to the selection."));

    this-> updateAvailableComponents();

    availableCLayout->addWidget(availableCompCombo);

    QPushButton *addComponent = new QPushButton();
    addComponent->setMinimumWidth(75);
    addComponent->setMaximumWidth(75);
    addComponent->setText(tr("Add"));
    connect(addComponent,SIGNAL(clicked()),this,SLOT(addOneComponent()));

    availableCLayout->addWidget(addComponent);

    QPushButton *addAllComponents = new QPushButton();
    addAllComponents->setMinimumWidth(100);
    addAllComponents->setMaximumWidth(100);
    addAllComponents->setText(tr("Add All"));
    connect(addAllComponents,SIGNAL(clicked()),this,SLOT(addAllComponents()));

    availableCLayout->addWidget(addAllComponents);

    compListFormLayout->addRow(tr("Available Components: "),
                               availableCLayout);

    QSpacerItem *spacerGroups2 = new QSpacerItem(10,10);
    compListFormLayout->addItem(spacerGroups2);

    // selected components
    QHBoxLayout *selectedCLayout = new QHBoxLayout();

    selectedCompCombo = new QComboBox();
    selectedCBModel = new QStringListModel();
    selectedCompCombo->setModel(selectedCBModel);
    selectedCompCombo->setToolTip(tr("List of selected components. Click on the Remove button to remove the component from the selection."));
    connect(selectedCompCombo,SIGNAL(currentIndexChanged(int)),this,SLOT(showSelectedComponent()));

    selectedCLayout->addWidget(selectedCompCombo);

    QPushButton *removeComponent = new QPushButton();
    removeComponent->setMinimumWidth(75);
    removeComponent->setMaximumWidth(75);
    removeComponent->setText(tr("Remove"));
    connect(removeComponent,SIGNAL(clicked()),this,SLOT(removeOneComponent()));

    selectedCLayout->addWidget(removeComponent);

    QPushButton *removeAllComponents = new QPushButton();
    removeAllComponents->setMinimumWidth(100);
    removeAllComponents->setMaximumWidth(100);
    removeAllComponents->setText(tr("Remove All"));
    connect(removeAllComponents,SIGNAL(clicked()),this,
            SLOT(removeAllComponents()));

    selectedCLayout->addWidget(removeAllComponents);

    compListFormLayout->addRow(tr("Selected Components: "),
                               selectedCLayout);

    // component details
    QGroupBox *compDetailsGroupBox = new QGroupBox("Component Details");
    compDetailsGroupBox->setMaximumWidth(600);
    QFormLayout *compDetailsFormLayout = new QFormLayout(compDetailsGroupBox);

    // name
    compName = new QLabel();
    compName->setWordWrap(true);
    compName->setText("");

    compDetailsFormLayout->addRow(tr("Name: "), compName);

    // description
    compDescription = new QLabel();
    compDescription->setWordWrap(true);
    compDescription->setText("");

    compDetailsFormLayout->addRow(tr("Description: "), compDescription);

    // EDP type
    compEDP = new QLabel();
    compEDP->setWordWrap(true);
    compEDP->setText("");

    compDetailsFormLayout->addRow(tr("EDP type: "), compEDP);

    // quantities
    QHBoxLayout *compQLayout = new QHBoxLayout();

    compQuantity = new QLineEdit();
    compQuantity->setToolTip(
                tr("Quantity of component in the specified units."));
    compQuantity->setMaximumWidth(200);
    compQuantity->setMinimumWidth(200);
    connect(compQuantity, SIGNAL(editingFinished()), this,
            SLOT(storeCompQuantity()));

    compUnitSize = new QLabel();
    compUnitSize->setText("");
    compUnitType = new QLabel();
    compUnitType->setText("");

    compQLayout->addWidget(compQuantity);
    compQLayout->addWidget(compUnitSize);
    compQLayout->addWidget(compUnitType);
    compQLayout->addStretch();

    compDetailsFormLayout->addRow(tr("Median Quantity per Story: "), compQLayout);

    // quantity distribution
    QHBoxLayout *compQDLayout = new QHBoxLayout();

    QLabel *compQFamilyLabel = new QLabel();
    compQFamilyLabel->setText("family:");
    compQFamily = new QComboBox();
    compQFamily->setToolTip(tr("Distribution family assigned to the component quantity."));
    compQFamily->addItem(tr(""));
    compQFamily->addItem(tr("normal"));
    compQFamily->addItem(tr("lognormal"));
    compQFamily->setMaximumWidth(100);
    compQFamily->setMinimumWidth(100);
    connect(compQFamily, SIGNAL(currentIndexChanged(int)), this,
            SLOT(storeCompQuantityDist()));

    QLabel *compQcovLabel = new QLabel();
    compQcovLabel->setText("cov: ");
    compQcov = new QLineEdit();
    compQcov->setToolTip(tr("Coefficient of variation for the component quantity"));
    compQcov->setMaximumWidth(70);
    compQcov->setMinimumWidth(70);
    connect(compQcov, SIGNAL(editingFinished()), this,
            SLOT(storeCompQuantityCOV()));

    compQDLayout->addWidget(compQFamilyLabel);
    compQDLayout->addWidget(compQFamily);
    compQDLayout->addWidget(compQcovLabel);
    compQDLayout->addWidget(compQcov);
    compQDLayout->addStretch();

    compDetailsFormLayout->addRow(tr("Quantity Distribution: "), compQDLayout);

    // component groups

    QLabel *cgLabel = new QLabel();
    cgLabel->setText(tr("Component Groups"));
    compDetailsFormLayout->addRow(cgLabel);

    dir1Weights = new QLineEdit();
    dir1Weights->setToolTip(tr("Weight (i.e. proportion of the total quantity) of each component group in the performance group. The same weighting scheme is used for every story."));
    dir1Weights->setMaximumWidth(250);
    dir1Weights->setMinimumWidth(250);
    connect(dir1Weights, SIGNAL(editingFinished()), this,
            SLOT(storeWeightD1()));

    dir2Weights = new QLineEdit();
    dir2Weights->setToolTip(tr("Weight (i.e. proportion of the total quantity) of each component group in the performance group. The same weighting scheme is used for every story."));
    dir2Weights->setMaximumWidth(250);
    dir2Weights->setMinimumWidth(250);
    connect(dir2Weights, SIGNAL(editingFinished()), this,
            SLOT(storeWeightD2()));

    compDetailsFormLayout->addRow(tr("Weights in direction 1: "), dir1Weights);
    compDetailsFormLayout->addRow(tr("Weights in direction 2: "), dir2Weights);

    // component input overview
    compOverviewGroupBox = new QGroupBox("Component Input Overview");
    compOverviewGroupBox->setMinimumWidth(1000);
    compOverviewGroupBox->setMinimumHeight(450);
    compOverviewGroupBox->setMaximumHeight(450);
    compOverviewLayout = new QVBoxLayout(compOverviewGroupBox);

    this->refreshCompOverview();

    verticalLayout->addLayout(titleLayout);
    verticalLayout->addLayout(horizontalLayout);

    horizontalLayout->addLayout(verticalC1Layout);

    verticalC1Layout->addWidget(compListGroupBox);
    verticalC1Layout->addWidget(compDetailsGroupBox);
    verticalC1Layout->addStretch(1);
    verticalC1Layout->setSpacing(10);
    verticalC1Layout->setMargin(0);

    horizontalLayout->addLayout(verticalC2Layout);

    verticalC2Layout->addWidget(compOverviewGroupBox);
    verticalC2Layout->addStretch();
    verticalC2Layout->setSpacing(10);
    verticalC2Layout->setMargin(0);

    horizontalLayout->addStretch();
    horizontalLayout->addSpacing(10);

    verticalLayout->addStretch(1);
    verticalLayout->setSpacing(10);

    this->setLayout(verticalLayout);

    // initialize component property containers
    compQuantityMap = new QMap<QString, QString>;
    compQuantityDistMap = new QMap<QString, QString>;
    compQuantityCOVMap = new QMap<QString, QString>;
    compWeightD1Map = new QMap<QString, QString>;
    compWeightD2Map = new QMap<QString, QString>;

    //connect(addComponent, SIGNAL(pressed()), this, SLOT(addComponent()));
    //connect(removeComponent, SIGNAL(pressed()), this, SLOT(removeComponents()));
}

void
P58ComponentContainer::storeCompQuantity(){
    compQuantityMap -> insert(selectedCompCombo->currentText(),
                              compQuantity->text());

    this->refreshCompOverview();
}

void
P58ComponentContainer::retrieveCompQuantity(){

    compQuantity->setText(compQuantityMap -> value(
                              selectedCompCombo->currentText(), tr("")));
}

void
P58ComponentContainer::storeCompQuantityDist(){
    compQuantityDistMap -> insert(selectedCompCombo->currentText(),
                              compQFamily->currentText());

    this->refreshCompOverview();
}

void
P58ComponentContainer::retrieveCompQuantityDist(){

    compQFamily->setCurrentText(compQuantityDistMap -> value(
                                    selectedCompCombo->currentText(), tr("")));
}

void
P58ComponentContainer::storeCompQuantityCOV(){
    compQuantityCOVMap -> insert(selectedCompCombo->currentText(),
                              compQcov->text());

    this->refreshCompOverview();
}

void
P58ComponentContainer::retrieveCompQuantityCOV(){

    compQcov->setText(compQuantityCOVMap -> value(
                              selectedCompCombo->currentText(), tr("")));
}

void
P58ComponentContainer::storeWeightD1(){
    compWeightD1Map -> insert(selectedCompCombo->currentText(),
                              dir1Weights->text());

    this->refreshCompOverview();
}

void
P58ComponentContainer::retrieveWeightD1(){

    dir1Weights->setText(compWeightD1Map -> value(
                              selectedCompCombo->currentText(), tr("")));
}

void
P58ComponentContainer::storeWeightD2(){
    compWeightD2Map -> insert(selectedCompCombo->currentText(),
                              dir2Weights->text());

    this->refreshCompOverview();
}

void
P58ComponentContainer::retrieveWeightD2(){

    dir2Weights->setText(compWeightD2Map -> value(
                              selectedCompCombo->currentText(), tr("")));
}

void
P58ComponentContainer::refreshCompOverview(){

    // clear the layout
    QLayoutItem *wItem;
    while ((wItem = compOverviewLayout->takeAt(0)) != 0)
        delete wItem;

    int columnWidth = 200;

    // start with the header
    QWidget *header = new QWidget();
    QHBoxLayout *headerLayout = new QHBoxLayout();
    header->setLayout(headerLayout);

    QLabel *IDLabel = new QLabel();
    IDLabel->setText(tr("<b><i>ID</i></b>"));
    IDLabel->setAlignment(Qt::AlignLeft);
    IDLabel->setFixedWidth(150);
    headerLayout->addWidget(IDLabel);

    QLabel *quantityLabel = new QLabel();
    quantityLabel->setText(tr("<b><i>Median Quantity</i></b>"));
    quantityLabel->setAlignment(Qt::AlignLeft);
    quantityLabel->setFixedWidth(columnWidth);
    headerLayout->addWidget(quantityLabel);

    QLabel *qDistLabel = new QLabel();
    qDistLabel->setText(tr("<b><i>Quantity Distribution</i></b>"));
    qDistLabel->setAlignment(Qt::AlignLeft);
    qDistLabel->setFixedWidth(columnWidth);
    headerLayout->addWidget(qDistLabel);

    QLabel *groupD1Label = new QLabel();
    groupD1Label->setText(tr("<b><i>Groups in dir 1</i></b>"));
    groupD1Label->setAlignment(Qt::AlignLeft);
    groupD1Label->setFixedWidth(columnWidth);
    headerLayout->addWidget(groupD1Label);

    QLabel *groupD2Label = new QLabel();
    groupD2Label->setText(tr("<b><i>Groups in dir 2</i></b>"));
    groupD2Label->setAlignment(Qt::AlignLeft);
    groupD2Label->setFixedWidth(columnWidth);
    headerLayout->addWidget(groupD2Label);

    //headerLayout->addStretch();

    compOverviewLayout->addWidget(header);

    QScrollArea *compArea = new QScrollArea;
    compArea->setWidgetResizable(true);
    compArea->setLineWidth(0);
    compArea->setFrameShape(QFrame::NoFrame);
    compArea->setAlignment(Qt::AlignTop);

    QWidget *compAreaWidget = new QWidget();
    QVBoxLayout *compAreaVBox = new QVBoxLayout();
    compAreaVBox->setMargin(0);
    compAreaVBox->setSpacing(3);
    compAreaWidget->setLayout(compAreaVBox);
    compArea->setWidget(compAreaWidget);
    compOverviewLayout->addWidget(compArea);

    const QStringList compList = selectedCBModel->stringList();

    for (const auto& compName : compList)
    {
        addCompToOverview(compName, compAreaVBox);
    }
}

void
P58ComponentContainer::addCompToOverview(QString compName, QVBoxLayout *compAreaVBox){

    QWidget *compInfo = new QWidget();
    QHBoxLayout *compInfoLayout = new QHBoxLayout();
    compInfo->setLayout(compInfoLayout);

    int columnWidth = 200;

    // get the component ID

    QLabel *IDLabel = new QLabel();
    IDLabel->setText(compName);
    IDLabel->setAlignment(Qt::AlignLeft);
    IDLabel->setFixedWidth(150);
    compInfoLayout->addWidget(IDLabel);

    QLabel *quantityLabel = new QLabel();
    quantityLabel->setText(compQuantityMap -> value(compName, tr("")));
    quantityLabel->setAlignment(Qt::AlignLeft);
    quantityLabel->setFixedWidth(columnWidth);
    compInfoLayout->addWidget(quantityLabel);

    QLabel *qDistLabel = new QLabel();
    qDistLabel->setText(compQuantityDistMap -> value(compName, tr("")) +
                        "   " +
                        compQuantityCOVMap -> value(compName, tr("")));
    qDistLabel->setAlignment(Qt::AlignLeft);
    qDistLabel->setFixedWidth(columnWidth);
    compInfoLayout->addWidget(qDistLabel);

    QLabel *groupD1Label = new QLabel();
    groupD1Label->setText(compWeightD1Map -> value(compName, tr("")));
    groupD1Label->setAlignment(Qt::AlignLeft);
    groupD1Label->setFixedWidth(columnWidth);
    compInfoLayout->addWidget(groupD1Label);

    QLabel *groupD2Label = new QLabel();
    groupD2Label->setText(compWeightD2Map -> value(compName, tr("")));
    groupD2Label->setAlignment(Qt::AlignLeft);
    groupD2Label->setFixedWidth(columnWidth);
    compInfoLayout->addWidget(groupD2Label);

    //compInfoLayout->addStretch();

    compAreaVBox->addWidget(compInfo);
}


void
P58ComponentContainer::showSelectedComponent(){

    QString compFileName = selectedCompCombo->currentText() + ".json";
    QDir fragDir(this->getFragilityFolder());
    QFile compFile(fragDir.absoluteFilePath(compFileName));
    compFile.open(QFile::ReadOnly | QFile::Text);

    QString val;
    val = compFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject compData = doc.object();
    compFile.close();

    compName->setText(compData["Name"].toString());

    QJsonObject compGI = compData["GeneralInformation"].toObject();
    compDescription->setText(compGI["Description"].toString());

    QJsonObject compEDPVar = compData["EDP"].toObject();
    compEDP->setText(compEDPVar["Type"].toString());

    QJsonArray compQData = compData["QuantityUnit"].toArray();
    compUnitSize->setText(QString("%1").arg(compQData[0].toDouble()));
    compUnitType->setText(compQData[1].toString());

    this->retrieveCompQuantity();
    this->retrieveCompQuantityDist();
    this->retrieveCompQuantityCOV();
    this->retrieveWeightD1();
    this->retrieveWeightD2();
}

int
P58ComponentContainer::updateAvailableComponents(){

    while (availableCompCombo->count() > 0) {
        availableCompCombo->removeItem(availableCompCombo->currentIndex());
    }

    QDir directory(this->getFragilityFolder());

    QStringList DL_files = directory.entryList(QStringList() << "*.json" << "*.JSON", QDir::Files);

    int counter = 0;
    foreach(QString DL_filename, DL_files){
        availableCompCombo->addItem(DL_filename.remove(DL_filename.size()-5, 5),
                                    counter);
        counter += 1;
    }

    return 0;
}


QString
P58ComponentContainer::getFragilityFolder(){

    QString fragilityFolder = fragilityFolderPath->text();

    if (fragilityFolder == "") {

        QString appDir = QString("");
        QSettings settings("SimCenter", QCoreApplication::applicationName());
        QVariant  appDirVariant = settings.value("appDir");
        if (appDirVariant.isValid())
          appDir = appDirVariant.toString();

        fragilityFolder = appDir + "applications/performDL/pelicun/resources/FEMA P58 first edition/DL json";
    }

    return fragilityFolder;
}

int
P58ComponentContainer::setFragilityFolder(QString fragilityFolder){
    fragilityFolderPath->setText(fragilityFolder);
    return 0;
}

void
P58ComponentContainer::chooseFragilityFolder(void) {
    QString fragilityFolder;
    fragilityFolder=QFileDialog::getExistingDirectory(this,tr("Select Folder"),
        "C://");

    this->setFragilityFolder(fragilityFolder);

    this->updateAvailableComponents();
}


P58ComponentContainer::~P58ComponentContainer()
{

}

void P58ComponentContainer::addOneComponent()
{
    this->addComponent(true);
}

void P58ComponentContainer::removeOneComponent()
{
    this->removeComponent(true);
}

void P58ComponentContainer::addComponent(bool doRefresh)
{
    if (selectedCompCombo->findText(availableCompCombo->currentText()) == -1) {
        selectedCompCombo->addItem(availableCompCombo->currentText(),
                                   availableCompCombo->currentIndex());
    }

    if (doRefresh == true)
        this->refreshCompOverview();
}

void P58ComponentContainer::addAllComponents(void)
{
    const QStringList compList = availableCBModel->stringList();

    for (const auto& compName : compList)
    {
        availableCompCombo->setCurrentText(compName);
        this->addComponent(false);
    }

    this->refreshCompOverview();
}

void P58ComponentContainer::removeComponent(bool doRefresh)
{
    selectedCompCombo->removeItem(selectedCompCombo->currentIndex());

    if (doRefresh == true)
        this->refreshCompOverview();
}

void P58ComponentContainer::removeAllComponents(void)
{
    while (selectedCompCombo->count() > 0) {
        this->removeComponent(false);
    }

    this->refreshCompOverview();
}

bool
P58ComponentContainer::outputToJSON(QJsonObject &jsonObject)
{
    bool result = true;

    // first, save the DL data folder
    QString pathString;
    pathString = fragilityFolderPath->text();
    if (pathString != ""){
        QJsonObject dataSources;

        if (jsonObject.contains("DataSources"))
            dataSources = jsonObject["DataSources"].toObject();

        dataSources["ComponentDataFolder"] = pathString;
        jsonObject["DataSources"] = dataSources;
    }

    // then, for each component save the component information
    QJsonArray compArray;
    const QStringList compList = selectedCBModel-> stringList();
    foreach (const QString compID, compList){

        QJsonObject compObj;
        compObj["ID"] = compID;

        compObj["quantity"] = compQuantityMap->value(compID, tr(""));
        compObj["cov"] = compQuantityCOVMap->value(compID, tr(""));
        compObj["distribution"] = compQuantityDistMap->value(compID, tr(""));

        QStringList weightSL = QStringList();
        QStringList directionSL = QStringList();

        QStringList weightsD1 = (compWeightD1Map->value(compID, tr(""))).split(',');
        foreach (const QString weight, weightsD1) {
            QString netWeight = weight.simplified();
            netWeight.replace(" ", "");

            if (netWeight != ""){
                weightSL.append(netWeight);
                directionSL.append("1");
            }
        }

        QStringList weightsD2 = (compWeightD2Map->value(compID, tr(""))).split(',');
        foreach (const QString weight, weightsD2) {
            QString netWeight = weight.simplified();
            netWeight.replace(" ", "");

            if (netWeight != ""){
                weightSL.append(netWeight);
                directionSL.append("2");
            }
        }

        compObj["directions"] = directionSL.join(',');
        compObj["weights"] = weightSL.join(',');

        // get some of the data that is stored in the DL file
        // this will be removed eventually
        QString compFileName = compID + ".json";
        QDir fragDir(this->getFragilityFolder());
        QFile compFile(fragDir.absoluteFilePath(compFileName));
        compFile.open(QFile::ReadOnly | QFile::Text);

        QString val;
        val = compFile.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject compData = doc.object();
        compFile.close();

        QJsonArray compQData = compData["QuantityUnit"].toArray();

        compObj["unit_size"] = QString("%1").arg(compQData[0].toDouble());
        compObj["unit_type"] = compQData[1].toString();
        compObj["structural"] = (compID.at(0) == 'B');
        compObj["name"] = compData["Name"].toString();

        compArray.append(compObj);
    }
    jsonObject["Components"]=compArray;

/*


    bool result = true;
    QJsonArray theArray;
    for (int i = 0; i <theComponents.size(); ++i) {
        QJsonObject rv;
        if (theComponents.at(i)->outputToJSON(rv)) {
            theArray.append(rv);

        } else {
            qDebug() << "OUTPUT FAILED" << theComponents.at(i)->getComponentName();
            result = false;
        }
    }
    jsonObject["Components"]=theArray;
    return result;
*/
    return result;
}

bool
P58ComponentContainer::inputFromJSON(QJsonObject &rvObj)
{
  bool result = true;

  // first, load the DL data folder
  QJsonObject dataSources = rvObj["DataSources"].toObject();

  QString pathString;
  pathString = dataSources["ComponentDataFolder"].toString();

  fragilityFolderPath->setText(pathString);

  QJsonArray rvArray = rvObj["Components"].toArray();
  //for each component object in the array, load the component information
  foreach (const QJsonValue &rvValue, rvArray) {
      
      QJsonObject compObject = rvValue.toObject();

      if (compObject.contains("ID")) {
          QString compID = compObject["ID"].toString();
          if (availableCompCombo->findText(compID) != -1){
              availableCompCombo->setCurrentText(compID);
              this->addComponent(false);

              compQuantityMap->insert(compID,
                                      compObject["quantity"].toString());

              compQuantityDistMap->insert(compID,
                                      compObject["distribution"].toString());

              compQuantityCOVMap->insert(compID,
                                      compObject["cov"].toString());

              QStringList directionSL = compObject["directions"].toString().split(',');
              QStringList weightSL = compObject["weights"].toString().split(',');
              QStringList weightsD1 = QStringList();
              QStringList weightsD2 = QStringList();

              for (int i=0; i<weightSL.size(); i++){

                  if ((directionSL.at(i) == "1") || (directionSL.at(i) == " 1"))
                      weightsD1.append(weightSL.at(i));
                  else
                      weightsD2.append(weightSL.at(i));
              }

              if (weightsD1.size() > 0)
                compWeightD1Map->insert(compID, weightsD1.join(','));

              if (weightsD2.size() > 0)
                compWeightD2Map->insert(compID, weightsD2.join(','));
          }
      }
  }

  this->refreshCompOverview();

  return result;
}

bool 
P58ComponentContainer::copyFiles(QString &dirName) {
    return true;
}

void
P58ComponentContainer::errorMessage(QString message){
    emit sendErrorMessage(message);
}

