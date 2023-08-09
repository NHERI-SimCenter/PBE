/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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

#include "WorkflowAppPBE.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QDebug>
#include <QFileDialog>
#include <QJsonObject>
#include <QScrollArea>
#include <SectionTitle.h>
#include <QSignalMapper>

#include "SimCenterPreferences.h"
#include "PelicunDemandContainer.h"
#include "PelicunCollapseLimit.h"
#include "PelicunTruncationLimit.h"

PelicunDemandContainer::PelicunDemandContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    this->setMaximumWidth(800);

    int maxWidth = 400;

    collConfig = new QVector<QMap<QString, QString>* >;
    truncConfig = new QVector<QMap<QString, QString>* >;
    residualConfig = new QVector<QMap<QString, QString>* >;

    gridLayout = new QGridLayout();

    // Data Source ------------------------------------------------------------

    QGroupBox *DataSourceGB = new QGroupBox("Data Source");
    DataSourceGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loDS = new QVBoxLayout(DataSourceGB);

    QHBoxLayout *selectDSLayout = new QHBoxLayout();

    QLabel *lblDSdemand = new QLabel();
    lblDSdemand->setText("Demand Data:");
    lblDSdemand->setMaximumWidth(100);
    lblDSdemand->setMinimumWidth(100);
    selectDSLayout->addWidget(lblDSdemand);

    // database selection
    databaseCombo = new QComboBox();
    databaseCombo->setToolTip(tr("This data source provides the raw demand data for the analysis."));
    databaseCombo->addItem("from Workflow",0);
    databaseCombo->addItem("from File",1);

    databaseCombo->setItemData(0, "Demands calculated as part of the workflow and passed on to the performance assessment module automatically.", Qt::ToolTipRole);
    databaseCombo->setItemData(1, "Demands pre-calculated and formatted by the user and provided as an external file.", Qt::ToolTipRole);

    selectDSLayout->addWidget(databaseCombo, 1);

    loDS->addLayout(selectDSLayout);

    // database path
    QHBoxLayout *customSourceLayout = new QHBoxLayout();

    btnChooseDemand = new QPushButton();
    btnChooseDemand->setMinimumWidth(100);
    btnChooseDemand->setMaximumWidth(100);
    btnChooseDemand->setText(tr("Choose"));
    connect(btnChooseDemand, SIGNAL(clicked()),this,SLOT(chooseDemandData()));
    customSourceLayout->addWidget(btnChooseDemand);
    btnChooseDemand->setVisible(false);

    demandDataPath = new QLineEdit;
    demandDataPath->setToolTip(tr("Location of the user-defined demand data."));
    customSourceLayout->addWidget(demandDataPath, 1);
    demandDataPath->setVisible(false);
    demandDataPath->setEnabled(false);

    connect(databaseCombo, SIGNAL(currentIndexChanged(QString)), this,
                SLOT(DemandDSSelectionChanged(QString)));

    loDS->addLayout(customSourceLayout);

    loDS->addStretch();


    // Demand Model ------------------------------------------------------------

    QGroupBox *DemandModelGB = new QGroupBox("Probabilistic Model");
    DemandModelGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loDM = new QVBoxLayout(DemandModelGB);
    loDM->setAlignment(Qt::AlignTop);

    // distribution to fit

    QHBoxLayout *distLayout = new QHBoxLayout();

    QLabel *lblDistdemand = new QLabel();
    lblDistdemand->setText("Distribution:");
    lblDistdemand->setMaximumWidth(100);
    lblDistdemand->setMinimumWidth(100);
    distLayout->addWidget(lblDistdemand);

    demandDistribution = new QComboBox();
    demandDistribution->setToolTip(tr("Approach used to re-sample EDPs.\n"
                                      "lognormal - use multivariate lognormal distribution\n"
                                      "truncated lognormal - truncate the multivar. lognorm. at pre-defined limits\n"
                                      "empirical - use raw data as is\n"));
    demandDistribution->addItem("fit lognormal",0);
    demandDistribution->addItem("fit truncated lognormal",1);
    demandDistribution->addItem("use empirical",2);

    demandDistribution->setCurrentIndex(0);
    distLayout->addWidget(demandDistribution);

    loDM->addLayout(distLayout);

    // add line
    lineCollapse = new QFrame();
    lineCollapse->setFrameShape(QFrame::HLine);
    lineCollapse->setFrameShadow(QFrame::Sunken);
    loDM->addWidget(lineCollapse);

    //truncation limits label

    truncationSettings = new QWidget();
    QVBoxLayout * loTRV = new QVBoxLayout(truncationSettings);

    QHBoxLayout *truncationLimitsLayout = new QHBoxLayout();

    QLabel *lblTruncLimits = new QLabel();
    lblTruncLimits->setText("Truncation Limits");
    lblTruncLimits->setToolTip(tr("Specify lower and upper bounds for the truncated lognormal distribution."));
    lblTruncLimits->setMaximumWidth(100);
    lblTruncLimits->setMinimumWidth(100);
    truncationLimitsLayout->addWidget(lblTruncLimits);

    truncationLimitsLayout->addStretch();

    loTRV->addLayout(truncationLimitsLayout);

    // Truncation Limit header

    QHBoxLayout *loTLimit_header = new QHBoxLayout();

    QPushButton *addTLimit = new QPushButton();
    addTLimit->setMinimumWidth(20);
    addTLimit->setMaximumWidth(20);
    addTLimit->setMaximumHeight(20);
    addTLimit->setText(tr("+"));
    loTLimit_header->addWidget(addTLimit);
    connect(addTLimit, SIGNAL(pressed()), this, SLOT(addTruncationLimit()));

    QLabel *lblTDEM_type = new QLabel();
    lblTDEM_type->setText("Demand");
    lblTDEM_type->setMaximumWidth(75);
    lblTDEM_type->setMinimumWidth(75);
    loTLimit_header->addWidget(lblTDEM_type);

    QLabel *lblLower_limit = new QLabel();
    lblLower_limit->setText("Lower Lim");
    lblLower_limit->setMaximumWidth(75);
    lblLower_limit->setMinimumWidth(75);
    loTLimit_header->addWidget(lblLower_limit);

    QLabel *lblUpper_limit = new QLabel();
    lblUpper_limit->setText("Upper Lim");
    lblUpper_limit->setMaximumWidth(75);
    lblUpper_limit->setMinimumWidth(75);
    loTLimit_header->addWidget(lblUpper_limit);

    loTLimit_header->addStretch();
    loTLimit_header->setSpacing(10);
    loTLimit_header->setContentsMargins(0,0,0,0);;

    loTRV->addLayout(loTLimit_header);

    // Truncation Limit list
    QHBoxLayout *loTLimitLayout = new QHBoxLayout();

    loTruncLimRemove = new QVBoxLayout();
    loTruncLimRemove->setAlignment(Qt::AlignTop);

    loTruncLimRemove->addSpacing(2);
    loTruncLimRemove->setSpacing(5);
    loTruncLimRemove->setContentsMargins(0,0,0,0);;

    smRemoveTL = new QSignalMapper(this);
    connect(smRemoveTL, SIGNAL(mapped(QWidget*)), this,
            SLOT(removeTruncationLimit(QWidget*)));

    loTLimitLayout->addLayout(loTruncLimRemove);

    // truncation limits

    QScrollArea *saTruncLimits = new QScrollArea;
    saTruncLimits->setWidgetResizable(true);
    saTruncLimits->setLineWidth(0);
    saTruncLimits->setFrameShape(QFrame::NoFrame);

    QWidget *TLWidget = new QWidget;
    loTLList = new QVBoxLayout();
    loTLList->setAlignment(Qt::AlignTop);
    loTLList->setSpacing(5);
    loTLList->setContentsMargins(0,0,0,0);;
    TLWidget->setLayout(loTLList);
    saTruncLimits->setWidget(TLWidget);

    loTLimitLayout->addWidget(saTruncLimits);
    loTRV->addLayout(loTLimitLayout);

    loDM->addWidget(truncationSettings);

    connect(demandDistribution,
            SIGNAL(currentIndexChanged(QString)), this,
            SLOT(DistributionSelectionChanged(QString)));
    this->DistributionSelectionChanged(demandDistribution->currentText());

    // additional uncertainty

    QHBoxLayout *betaLayout = new QHBoxLayout();

    addBeta = new QCheckBox();
    addBeta->setText("");
    addBeta->setToolTip("Increase the logarithmic standard deviation of demands \n"
                        "to consider additional sources of uncertainty.");
    addBeta->setChecked(false);
    addBeta->setTristate(false);
    betaLayout->addWidget(addBeta);

    QLabel *lblBetaCheck = new QLabel();
    lblBetaCheck->setText("Add Uncertainty");
    betaLayout->addWidget(lblBetaCheck,1);

    loDM->addLayout(betaLayout);

    uncertaintySettings = new QWidget();
    QVBoxLayout * loUQV = new QVBoxLayout(uncertaintySettings);

    // additional uncertainty values

    QHBoxLayout *betaHLayout = new QHBoxLayout();

    QLabel *lblbeta = new QLabel();
    lblbeta->setText("Log Std Increase:");
    lblbeta->setMaximumWidth(100);
    lblbeta->setMinimumWidth(100);
    betaHLayout->addWidget(lblbeta);

    addedUncertainty = new QLineEdit();
    addedUncertainty->setToolTip(tr(""));
    addedUncertainty->setText("");
    addedUncertainty->setAlignment(Qt::AlignRight);
    betaHLayout->addWidget(addedUncertainty, 1);

    loUQV->addLayout(betaHLayout);

    loDM->addWidget(uncertaintySettings);

    connect(addBeta, SIGNAL(stateChanged(int)), this,
                SLOT(uncertaintyCheckChanged(int)));
    this->uncertaintyCheckChanged(addBeta->checkState());

    // add line
    lineTL = new QFrame();
    lineTL->setFrameShape(QFrame::HLine);
    lineTL->setFrameShadow(QFrame::Sunken);
    loDM->addWidget(lineTL);

    // remove collapses?

    QHBoxLayout *collapseLayout = new QHBoxLayout();

    removeCollapse = new QCheckBox();
    removeCollapse->setText("");
    removeCollapse->setToolTip("Remove collapsed cases from raw demands");
    removeCollapse->setTristate(false);
    removeCollapse->setChecked(false);
    collapseLayout->addWidget(removeCollapse);

    QLabel *lblCollapseCheck = new QLabel();
    lblCollapseCheck->setText("Remove collapses");
    collapseLayout->addWidget(lblCollapseCheck,1);

    loDM->addLayout(collapseLayout);

    collapseSettings = new QWidget();
    QVBoxLayout * loCollV = new QVBoxLayout(collapseSettings);

    // Collapse Limit header
    QHBoxLayout *loCLimit_header = new QHBoxLayout();

    QPushButton *addCLimit = new QPushButton();
    addCLimit->setMinimumWidth(20);
    addCLimit->setMaximumWidth(20);
    addCLimit->setMaximumHeight(20);
    addCLimit->setText(tr("+"));
    loCLimit_header->addWidget(addCLimit);
    connect(addCLimit, SIGNAL(pressed()), this, SLOT(addCollapseLimit()));

    QLabel *lblDEM_type = new QLabel();
    lblDEM_type->setText("Demand");
    lblDEM_type->setMaximumWidth(75);
    lblDEM_type->setMinimumWidth(75);
    loCLimit_header->addWidget(lblDEM_type);

    QLabel *lblDEM_limit = new QLabel();
    lblDEM_limit->setText("Collapse Lim");
    lblDEM_limit->setMaximumWidth(75);
    lblDEM_limit->setMinimumWidth(75);
    loCLimit_header->addWidget(lblDEM_limit);

    loCLimit_header->addStretch();
    loCLimit_header->setSpacing(10);
    loCLimit_header->setContentsMargins(0,0,0,0);;

    loCollV->addLayout(loCLimit_header);

    // Collapse Limit list
    QHBoxLayout *loCLimitLayout = new QHBoxLayout();

    loCollLimRemove = new QVBoxLayout();
    loCollLimRemove->setAlignment(Qt::AlignTop);

    loCollLimRemove->addSpacing(2);
    loCollLimRemove->setSpacing(5);
    loCollLimRemove->setContentsMargins(0,0,0,0);;

    smRemoveCL = new QSignalMapper(this);
    connect(smRemoveCL, SIGNAL(mapped(QWidget*)), this,
            SLOT(removeCollapseLimit(QWidget*)));

    loCLimitLayout->addLayout(loCollLimRemove);

    QScrollArea *saCollapseLims = new QScrollArea;
    saCollapseLims->setWidgetResizable(true);
    saCollapseLims->setLineWidth(0);
    saCollapseLims->setFrameShape(QFrame::NoFrame);

    QWidget *CLWidget = new QWidget;
    loCLList = new QVBoxLayout();
    loCLList->setAlignment(Qt::AlignTop);
    loCLList->setContentsMargins(0,0,0,0);;
    loCLList->setSpacing(5);
    CLWidget->setLayout(loCLList);
    saCollapseLims->setWidget(CLWidget);

    loCLimitLayout->addWidget(saCollapseLims);
    loCollV->addLayout(loCLimitLayout, 1);

    // add line
    lineBeta = new QFrame();
    lineBeta->setFrameShape(QFrame::HLine);
    lineBeta->setFrameShadow(QFrame::Sunken);
    loCollV->addWidget(lineBeta);

    loDM->addWidget(collapseSettings);

    connect(removeCollapse, SIGNAL(stateChanged(int)), this,
                SLOT(collapseCheckChanged(int)));
    this->collapseCheckChanged(removeCollapse->checkState());

    // Demand Sample -------

    QGroupBox *DemandSampleGB = new QGroupBox("Sample");
    DemandSampleGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loSample = new QVBoxLayout(DemandSampleGB);

    //sample size
    QHBoxLayout *sampleSizeLayout = new QHBoxLayout();

    QLabel *lblsampleS = new QLabel();
    lblsampleS->setText("Sample Size");
    lblsampleS->setMaximumWidth(100);
    lblsampleS->setMinimumWidth(100);
    sampleSizeLayout->addWidget(lblsampleS);

    sampleSize = new QLineEdit();
    sampleSize->setToolTip(tr("Number of demand realizations to sample."));
    sampleSize->setText("");
    sampleSize->setAlignment(Qt::AlignRight);
    sampleSizeLayout->addWidget(sampleSize, 1);

    loSample->addLayout(sampleSizeLayout);

    // coupled analysis
    QHBoxLayout *coupledLayout = new QHBoxLayout();

    needCoupled = new QCheckBox();
    needCoupled->setText("");
    needCoupled->setToolTip("Use raw data directly without re-sampling.\n"
                            "(If the sample size is larger than the raw data size, we re-use the data starting from the beginning.)");
    needCoupled->setChecked(false);
    coupledLayout->addWidget(needCoupled);

    QLabel *lblCoupled = new QLabel();
    lblCoupled->setText("Directly use raw demand data");
    coupledLayout->addWidget(lblCoupled,1);

    loSample->addLayout(coupledLayout);

    loSample->addStretch();

    // Residual Drifts ------

    QGroupBox *ResidualDriftGB = new QGroupBox("Residual Drifts");
    ResidualDriftGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loResidual = new QVBoxLayout(ResidualDriftGB);
    loResidual->setAlignment(Qt::AlignTop);

    // inference method

    QHBoxLayout *inferenceLayout = new QHBoxLayout();

    inferenceCombo = new QComboBox();
    inferenceCombo->setToolTip(tr("Select the method to use to infer residual drift values."));
    inferenceCombo->addItem("do not infer",0);
    inferenceCombo->addItem("infer as per FEMA P-58",1);

    inferenceCombo->setItemData(0, "No inference; residual drifts are either provided by the user or irreparable damage is not part of the assessment.", Qt::ToolTipRole);
    inferenceCombo->setItemData(1, "Residual drifts inferred from peak drifts using the method described in FEMA P-58.", Qt::ToolTipRole);

    inferenceLayout->addWidget(inferenceCombo);

    loResidual->addLayout(inferenceLayout);

    // add line
    lineYDR = new QFrame();
    lineYDR->setFrameShape(QFrame::HLine);
    lineYDR->setFrameShadow(QFrame::Sunken);
    loResidual->addWidget(lineYDR);

    residualDSettings = new QWidget();
    QVBoxLayout * loRDV = new QVBoxLayout(residualDSettings);

    //yield drift ratio label
    QHBoxLayout *loRParam_header = new QHBoxLayout();

    QPushButton *addRParam = new QPushButton();
    addRParam->setMinimumWidth(20);
    addRParam->setMaximumWidth(20);
    addRParam->setMaximumHeight(20);
    addRParam->setText(tr("+"));
    loRParam_header->addWidget(addRParam);
    connect(addRParam, SIGNAL(pressed()), this, SLOT(addResidualParam()));

    QLabel *lblRP_Direction = new QLabel();
    lblRP_Direction->setText("Direction");
    lblRP_Direction->setToolTip(tr("Structural systems and structural behavior might be different\n"
                                   "in different horizontal directions. These panels allow you to\n"
                                   "provide direction-specific parameters. If you prefer to use the\n"
                                   "same parameters for every direction, use \"all\" for this field."));
    lblRP_Direction->setMaximumWidth(75);
    lblRP_Direction->setMinimumWidth(75);
    loRParam_header->addWidget(lblRP_Direction);

    QLabel *lblRP_YieldDrift = new QLabel();
    lblRP_YieldDrift->setText("Yield Drift Ratio");
    lblRP_YieldDrift->setMaximumWidth(100);
    lblRP_YieldDrift->setMinimumWidth(100);
    loRParam_header->addWidget(lblRP_YieldDrift);

    loRParam_header->addStretch();
    loRParam_header->setSpacing(10);
    loRParam_header->setContentsMargins(0,0,0,0);;

    loRDV->addLayout(loRParam_header);

    // residual param list
    QHBoxLayout *loRParamLayout = new QHBoxLayout();

    loResidualPRemove = new QVBoxLayout();
    loResidualPRemove->setAlignment(Qt::AlignTop);

    loResidualPRemove->addSpacing(2);
    loResidualPRemove->setSpacing(5);
    loResidualPRemove->setContentsMargins(0,0,0,0);;

    smRemoveRP = new QSignalMapper(this);
    connect(smRemoveRP, SIGNAL(mapped(QWidget*)), this,
            SLOT(removeResidualParam(QWidget*)));

    loRParamLayout->addLayout(loResidualPRemove);

    // residual param values

    QScrollArea *saYieldDrift = new QScrollArea;
    saYieldDrift->setWidgetResizable(true);
    saYieldDrift->setLineWidth(0);
    saYieldDrift->setFrameShape(QFrame::NoFrame);

    QWidget *YDWidget = new QWidget;
    loRPList = new QVBoxLayout();
    loRPList->setAlignment(Qt::AlignTop);
    loRPList->setSpacing(5);
    loRPList->setContentsMargins(0,0,0,0);;
    YDWidget->setLayout(loRPList);
    saYieldDrift->setWidget(YDWidget);

    loRParamLayout->addWidget(saYieldDrift);
    loRDV->addLayout(loRParamLayout, 1);

    loResidual->addWidget(residualDSettings);

    connect(inferenceCombo,
            SIGNAL(currentIndexChanged(QString)), this,
            SLOT(ResidualDSelectionChanged(QString)));
    this->ResidualDSelectionChanged(inferenceCombo->currentText());

    // assemble the widgets-----------------------------------------------------

    gridLayout->addWidget(DataSourceGB, 0, 0);
    gridLayout->addWidget(DemandModelGB, 1, 0);
    gridLayout->addWidget(DemandSampleGB, 0, 1);
    gridLayout->addWidget(ResidualDriftGB, 1, 1);
    gridLayout->setRowStretch(1, 1);

    this->setLayout(gridLayout);
}

PelicunDemandContainer::~PelicunDemandContainer()
{
}

void
PelicunDemandContainer::chooseDemandData(void) {

    QString appDir = SimCenterPreferences::getInstance()->getAppDir();

    QString demandDataPath;
    demandDataPath =
        QFileDialog::getOpenFileName(this, tr("Select Demand CSV File"), appDir);

    this->setDemandData(demandDataPath);

}

int
PelicunDemandContainer::setDemandData(QString demandPath){

    demandDataPath->setText(demandPath);
    return 0;
}

void
PelicunDemandContainer::DemandDSSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("from File")) {
        demandDataPath->setVisible(true);
        btnChooseDemand->setVisible(true);
    } else {
        demandDataPath->setVisible(false);
        btnChooseDemand->setVisible(false);
    }
}

void PelicunDemandContainer::addCollapseLimit(QMap<QString, QString> *CL_data_in)
{

    // add a new CL_data dict in the vector
    QMap<QString, QString> *CL_data;
    if (CL_data_in == nullptr) {

        // create a new CL_data dict and add it to the vector
        CL_data = new QMap<QString, QString>;

        collConfig->append(CL_data);

    } else {
       CL_data = CL_data_in;
    }

    // create the new UI object and assign CL_data to it
    CollapseLimit *theCollapseLimit = new CollapseLimit(nullptr, CL_data);

    // add the CollapseLimit to the vector of CLs
    vCollapseLimits.append(theCollapseLimit);

    theCollapseLimit->setMinimumHeight(20);
    theCollapseLimit->setMaximumHeight(20);

    // add the CollapseLimit to the UI
    loCLList->insertWidget(-1, theCollapseLimit);

    // add a remove button to the UI
    QPushButton *removeCLimit = new QPushButton();
    removeCLimit->setMinimumWidth(20);
    removeCLimit->setMaximumWidth(20);
    removeCLimit->setMaximumHeight(20);
    removeCLimit->setText(tr("-"));
    loCollLimRemove->insertWidget(-1, removeCLimit);

    smRemoveCL->setMapping(removeCLimit, theCollapseLimit);
    connect(removeCLimit, SIGNAL(clicked()), smRemoveCL, SLOT(map()));

    vRemoveCLButtons.append(removeCLimit);
}

void PelicunDemandContainer::removeCollapseLimit(QWidget *theCollapseLimit)
{

    if (collConfig != nullptr) {
        // remove the requested collapse limit
        int CLcount = vCollapseLimits.size();
        for (int i = CLcount-1; i >= 0; i--) {
          CollapseLimit *theCollapseLimit_i = vCollapseLimits.at(i);
          if (theCollapseLimit_i == theCollapseLimit){
              //remove the widget from the UI
              theCollapseLimit_i->close();
              loCLList->removeWidget(theCollapseLimit);
              //remove the CL_data from the database
              collConfig->remove(i);
              //remove the CL from the UI vector
              vCollapseLimits.remove(i);
              //delete the CL UI object
              theCollapseLimit->setParent(nullptr);
              delete theCollapseLimit;
              //remove the PushButton from the button list
              QPushButton *theRemoveButton = vRemoveCLButtons.at(i);
              vRemoveCLButtons.remove(i);
              theRemoveButton->setParent(nullptr);
              delete theRemoveButton;
          }
        }
    }
}

void PelicunDemandContainer::clearCollapseLimitWidget()
{
    int CLcount = vCollapseLimits.size();
    for (int i = CLcount-1; i >= 0; i--) {

        CollapseLimit *theCollapseLimit = vCollapseLimits.at(i);
        //remove the widget from the UI
        theCollapseLimit->close();
        loCLList->removeWidget(theCollapseLimit);
        //remove the TL from the UI vector
        vCollapseLimits.remove(i);
        //delete the TL UI object
        theCollapseLimit->setParent(nullptr);
        delete theCollapseLimit;

        // get the corresponding remove button
        QPushButton *theRemoveButton = vRemoveCLButtons.at(i);
        // remove the button from the UI
        theRemoveButton->close();
        loCollLimRemove->removeWidget(theRemoveButton);
        // remove the button from the UI vector
        vRemoveCLButtons.remove(i);
        // delete the button object
        theRemoveButton->setParent(nullptr);
        delete theRemoveButton;

    }
}

void PelicunDemandContainer::retrieveCollapseLimits()
{
    if (collConfig != nullptr) {
        // create the CL UI elements for the existing data
        for (int i=0; i<collConfig->count(); i++){

            addCollapseLimit(collConfig->at(i));
        }
    }
}

void PelicunDemandContainer::deleteCollConfig()
{
    collConfig->clear();

    delete collConfig;
}

void PelicunDemandContainer::addTruncationLimit(QMap<QString, QString> *TL_data_in)
{
    // add a new TL_data dict in the vector
    QMap<QString, QString> *TL_data;
    if (TL_data_in == nullptr) {

        // create a new TL_data dict and add it to the vector
        TL_data = new QMap<QString, QString>;

        truncConfig->append(TL_data);

    } else {
       TL_data = TL_data_in;
    }

    // create the new UI object and assign TL_data to it
    TruncationLimit *theTruncationLimit = new TruncationLimit(nullptr, TL_data);

    // add the TruncationLimit to the vector of TLs
    vTruncationLimits.append(theTruncationLimit);

    theTruncationLimit->setMinimumHeight(20);
    theTruncationLimit->setMaximumHeight(20);

    // add the TruncationLimit to the UI
    loTLList->insertWidget(-1, theTruncationLimit);

    // add a remove button to the UI
    QPushButton *removeTLimit = new QPushButton();
    removeTLimit->setMinimumWidth(20);
    removeTLimit->setMaximumWidth(20);
    removeTLimit->setMaximumHeight(20);
    removeTLimit->setText(tr("-"));
    loTruncLimRemove->insertWidget(-1, removeTLimit);

    smRemoveTL->setMapping(removeTLimit, theTruncationLimit);
    connect(removeTLimit, SIGNAL(clicked()), smRemoveTL, SLOT(map()));

    vRemoveTLButtons.append(removeTLimit);
}

void PelicunDemandContainer::removeTruncationLimit(QWidget *theTruncationLimit)
{

    if (truncConfig != nullptr) {
        // remove the requested truncation limit
        int TLcount = vTruncationLimits.size();
        for (int i = TLcount-1; i >= 0; i--) {
          TruncationLimit *theTruncationLimit_i = vTruncationLimits.at(i);
          if (theTruncationLimit_i == theTruncationLimit){
              //remove the widget from the UI
              theTruncationLimit_i->close();
              loTLList->removeWidget(theTruncationLimit);
              //remove the TL_data from the database
              truncConfig->remove(i);
              //remove the TL from the UI vector
              vTruncationLimits.remove(i);
              //delete the TL UI object
              theTruncationLimit->setParent(nullptr);
              delete theTruncationLimit;
              //remove the PushButton from the button list
              QPushButton *theRemoveButton = vRemoveTLButtons.at(i);
              vRemoveTLButtons.remove(i);
              theRemoveButton->setParent(nullptr);
              delete theRemoveButton;
          }
        }
    }
}

void PelicunDemandContainer::clearTruncationLimitWidget()
{
    int TLcount = vTruncationLimits.size();
    for (int i = TLcount-1; i >= 0; i--) {

        TruncationLimit *theTruncationLimit = vTruncationLimits.at(i);
        //remove the widget from the UI
        theTruncationLimit->close();
        loTLList->removeWidget(theTruncationLimit);
        //remove the TL from the UI vector
        vTruncationLimits.remove(i);
        //delete the TL UI object
        theTruncationLimit->setParent(nullptr);
        delete theTruncationLimit;

        // get the corresponding remove button
        QPushButton *theRemoveButton = vRemoveTLButtons.at(i);
        // remove the button from the UI
        theRemoveButton->close();
        loTruncLimRemove->removeWidget(theRemoveButton);
        // remove the button from the UI vector
        vRemoveTLButtons.remove(i);
        // delete the button object
        theRemoveButton->setParent(nullptr);
        delete theRemoveButton;

    }
}

void PelicunDemandContainer::retrieveTruncationLimits()
{
    if (truncConfig != nullptr) {
        // create the TL UI elements for the existing data
        for (int i=0; i<truncConfig->count(); i++){

            addTruncationLimit(truncConfig->at(i));
        }
    }
}

void PelicunDemandContainer::deleteTruncConfig()
{
    truncConfig->clear();

    delete truncConfig;
}

void PelicunDemandContainer::addResidualParam(QMap<QString, QString> *RP_data_in)
{
    // add a new RP_data dict in the vector
    QMap<QString, QString> *RP_data;
    if (RP_data_in == nullptr) {

        // create a new RP_data dict and add it to the vector
        RP_data = new QMap<QString, QString>;

        residualConfig->append(RP_data);

    } else {
       RP_data = RP_data_in;
    }    

    // create the new UI object and assign RP_data to it
    ResidualParam *theResidualParam = new ResidualParam(nullptr, RP_data);

    // add the ResidualParam to the vector of TLs
    vResidualParams.append(theResidualParam);

    theResidualParam->setMinimumHeight(20);
    theResidualParam->setMaximumHeight(20);

    // add the ResidualParam to the UI
    loRPList->insertWidget(-1, theResidualParam);

    // add a remove button to the UI
    QPushButton *removeRParam = new QPushButton();
    removeRParam->setMinimumWidth(20);
    removeRParam->setMaximumWidth(20);
    removeRParam->setMaximumHeight(20);
    removeRParam->setText(tr("-"));
    loResidualPRemove->insertWidget(-1, removeRParam);

    smRemoveRP->setMapping(removeRParam, theResidualParam);
    connect(removeRParam, SIGNAL(clicked()), smRemoveRP, SLOT(map()));

    vRemoveRPButtons.append(removeRParam);
}

void PelicunDemandContainer::removeResidualParam(QWidget *theResidualParam)
{

    if (residualConfig != nullptr) {
        // remove the requested truncation limit
        int count = vResidualParams.size();
        for (int i = count-1; i >= 0; i--) {
          ResidualParam *theResidualParam_i = vResidualParams.at(i);
          if (theResidualParam_i == theResidualParam){
              //remove the widget from the UI
              theResidualParam_i->close();
              loRPList->removeWidget(theResidualParam);
              //remove the RP_data from the database
              residualConfig->remove(i);
              //remove the RP from the UI vector
              vResidualParams.remove(i);
              //delete the RP UI object
              theResidualParam->setParent(nullptr);
              delete theResidualParam;
              //remove the PushButton from the button list
              QPushButton *theRemoveButton = vRemoveRPButtons.at(i);
              vRemoveRPButtons.remove(i);
              theRemoveButton->setParent(nullptr);
              delete theRemoveButton;
          }
        }
    }
}

void PelicunDemandContainer::clearResidualParamWidget()
{
    int RLcount = vResidualParams.size();
    for (int i = RLcount-1; i >= 0; i--) {

        ResidualParam *theResidualParam = vResidualParams.at(i);
        //remove the widget from the UI
        theResidualParam->close();
        loRPList->removeWidget(theResidualParam);
        //remove the RP from the UI vector
        vResidualParams.remove(i);
        //delete the RP UI object
        theResidualParam->setParent(nullptr);
        delete theResidualParam;

        // get the corresponding remove button
        QPushButton *theRemoveButton = vRemoveRPButtons.at(i);
        // remove the button from the UI
        theRemoveButton->close();
        loResidualPRemove->removeWidget(theRemoveButton);
        // remove the button from the UI vector
        vRemoveRPButtons.remove(i);
        // delete the button object
        theRemoveButton->setParent(nullptr);
        delete theRemoveButton;

    }
}

void PelicunDemandContainer::retrieveResidualParams()
{
    if (residualConfig != nullptr) {
        // create the RP UI elements for the existing data
        for (int i=0; i<residualConfig->count(); i++){

            addResidualParam(residualConfig->at(i));
        }
    }
}

void PelicunDemandContainer::deleteResidualConfig()
{
    residualConfig->clear();

    delete residualConfig;
}

void
PelicunDemandContainer::collapseCheckChanged(int newState)
{
    if (newState == 2) {
        collapseSettings->show();
    } else {
        collapseSettings->hide();
    }
}

void
PelicunDemandContainer::uncertaintyCheckChanged(int newState)
{
    if (newState == 2) {
        uncertaintySettings->show();
    } else {
        uncertaintySettings->hide();
    }
}

void
PelicunDemandContainer::DistributionSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("fit truncated lognormal")) {
        truncationSettings->show();
    } else {
        truncationSettings->hide();
    }

    if (arg1 == QString("use empirical")) {
        addBeta -> setChecked(false);
    }
}

void
PelicunDemandContainer::ResidualDSelectionChanged(const QString &arg1)
{
    if (arg1 == QString("infer as per FEMA P-58")) {
        residualDSettings->show();
    } else {
        residualDSettings->hide();
    }
}

bool PelicunDemandContainer::outputToJSON(QJsonObject &jsonObject) {

    bool result = true;

    QJsonObject demandData;

    QString workDir = SimCenterPreferences::getInstance()->getLocalWorkDir();

    if (databaseCombo->currentText() == "from File") {
        demandData["DemandFilePath"] = demandDataPath->text();
    }

    if (demandDistribution->currentText() != "use empirical") {

        QJsonObject calibrationData;
        QJsonObject allParams;

        allParams["DistributionFamily"] = "lognormal";

        calibrationData["ALL"] = allParams;

        if (demandDistribution->currentText() == "fit truncated lognormal") {

            if (truncConfig != nullptr) {

                int TLcount = vTruncationLimits.size();

                for (int i = 0; i<TLcount; i++) {

                    TruncationLimit *theTruncationLimit_i = vTruncationLimits.at(i);

                    theTruncationLimit_i -> outputToJSON(calibrationData);

                }

            }
        }

        if (addBeta->isChecked()) {

            foreach (const QString& demandID, calibrationData.keys()) {

                QJsonObject cData = calibrationData[demandID].toObject();
                cData["AddUncertainty"] = addedUncertainty->text();
                calibrationData[demandID] = cData;
            }
        }

        demandData["Calibration"] = calibrationData;

    }

    if (removeCollapse->isChecked()) {

        if (collConfig != nullptr) {

            QJsonObject collapseRemoveSettings;

            int CLcount = vCollapseLimits.size();

            for (int i = 0; i < CLcount; i++) {

                CollapseLimit *theCollapseLimit_i = vCollapseLimits.at(i);
                theCollapseLimit_i -> outputToJSON(collapseRemoveSettings);
            }

            demandData["CollapseLimits"] = collapseRemoveSettings;
        }
    }

    demandData["SampleSize"] = sampleSize->text();
    if (needCoupled->isChecked()) {
        demandData["CoupledDemands"] = needCoupled->isChecked();
    }

    if (inferenceCombo->currentText() == "infer as per FEMA P-58"){

        if (residualConfig != nullptr) {

            QJsonObject residualData;

            residualData["method"] = QString("FEMA P-58");

            int rCount = vResidualParams.size();

            for (int i = 0; i < rCount; i++) {

                ResidualParam *theResidualParam_i = vResidualParams.at(i);
                theResidualParam_i -> outputToJSON(residualData);
            }

            demandData["InferResidualDrift"] = residualData;
        }
    }

    jsonObject["Demands"] = demandData;

    result = false;

    return result;
}

bool PelicunDemandContainer::inputFromJSON(QJsonObject & jsonObject) {

    bool result = false;

    QJsonObject demandData = jsonObject["Demands"].toObject();

    if (demandData.contains("DemandFilePath")) {
        databaseCombo->setCurrentText("from File");
        demandDataPath->setText(demandData["DemandFilePath"].toString());
    } else {
        databaseCombo->setCurrentText("from Workflow");
    }

    this->clearTruncationLimitWidget();
    this->deleteTruncConfig();
    truncConfig = new QVector<QMap<QString, QString>* >;
    addBeta->setChecked(false);

    if (demandData.contains("Calibration")) {

        QJsonObject calibrationData = demandData["Calibration"].toObject();

        if (calibrationData.contains("ALL")) {
            QJsonObject allParams = calibrationData["ALL"].toObject();

            if (allParams["DistributionFamily"] == "lognormal") {
                demandDistribution->setCurrentText("fit lognormal");
            } else {
                qDebug() << "ERROR Unknown demand distribution: " << allParams["DistributionFamily"];
            }

            if (allParams.contains("AddUncertainty")) {

                addBeta->setChecked(true);

                addedUncertainty->setText(allParams["AddUncertainty"].toString());
            }

        }

        for (auto demandType: calibrationData.keys()) {

            if (demandType != "ALL") {

                QJsonObject demParams = calibrationData[demandType].toObject();

                QString trLower = "";
                QString trUpper = "";

                if (demParams.contains("TruncateLower")) {
                    trLower = demParams["TruncateLower"].toString();
                }
                if (demParams.contains("TruncateUpper")) {
                    trUpper = demParams["TruncateUpper"].toString();
                }

                if ((trLower != "") || (trUpper != "")) {

                    if (demandDistribution->currentText() != "fit truncated lognormal"){
                        demandDistribution->setCurrentText("fit truncated lognormal");
                    }

                    QMap<QString, QString> *TL_data = new QMap<QString, QString>;

                    TL_data->insert("type", demandType);
                    TL_data->insert("lower", trLower);
                    TL_data->insert("upper", trUpper);

                    truncConfig->append(TL_data);
                }

            }

        }
        this->retrieveTruncationLimits();

    } else {

        demandDistribution->setCurrentText("use empirical");

    }

    this->clearCollapseLimitWidget();
    this->deleteCollConfig();
    collConfig = new QVector<QMap<QString, QString>* >;

    if (demandData.contains("CollapseLimits")){

        removeCollapse->setChecked(true);

        QJsonObject collapseData = demandData["CollapseLimits"].toObject();

        for (auto demandType: collapseData.keys()) {

            QMap<QString, QString> *CL_data = new QMap<QString, QString>;

            CL_data->insert("type", demandType);
            CL_data->insert("limit", collapseData[demandType].toString());

            collConfig->append(CL_data);

        }
        this->retrieveCollapseLimits();

    } else {

        removeCollapse->setChecked(false);
    }

    if (demandData.contains("SampleSize")) {
        sampleSize->setText(demandData["SampleSize"].toString());
    }

    if (demandData.contains("CoupledDemands")) {
        needCoupled->setChecked(demandData["CoupledDemands"].toBool());
    } else {
        needCoupled->setChecked(false);
    }

    this->clearResidualParamWidget();
    this->deleteResidualConfig();
    residualConfig = new QVector<QMap<QString, QString>* >;

    if (demandData.contains("InferResidualDrift")){

        QJsonObject inferData = demandData["InferResidualDrift"].toObject();

        if (inferData["method"] == "FEMA P-58") {

            inferenceCombo->setCurrentText("infer as per FEMA P-58");

            for (auto directionLabel: inferData.keys()) {

                if (directionLabel != "method") {

                    QMap<QString, QString> *RP_data = new QMap<QString, QString>;

                    RP_data->insert("dir", directionLabel);
                    RP_data->insert("delta_yield", inferData[directionLabel].toString());

                    residualConfig->append(RP_data);
                }
            }
        }

        this->retrieveResidualParams();

    } else {

        inferenceCombo->setCurrentText("do not infer");
    }

    result = true;

    return result;
}

