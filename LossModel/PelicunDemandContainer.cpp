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
#include <sectiontitle.h>

#include "SimCenterPreferences.h"
#include "PelicunDemandContainer.h"

PelicunDemandContainer::PelicunDemandContainer(QWidget *parent)
    : SimCenterAppWidget(parent)
{
    this->setMaximumWidth(600);

    int maxWidth = 300;

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
    databaseCombo->addItem("Workflow",0);
    databaseCombo->addItem("User Defined",1);

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

    QGroupBox *DemandModelGB = new QGroupBox("Stochastic Model");
    DemandModelGB->setMaximumWidth(maxWidth);

    QVBoxLayout *loDM = new QVBoxLayout(DemandModelGB);

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

    // remove collapses?

    QHBoxLayout *collapseLayout = new QHBoxLayout();

    removeCollapse = new QCheckBox();
    removeCollapse->setText("");
    removeCollapse->setToolTip("Remove collapsed cases from raw demands");
    removeCollapse->setChecked(true);
    collapseLayout->addWidget(removeCollapse);

    QLabel *lblCollapseCheck = new QLabel();
    lblCollapseCheck->setText("Remove collapses");
    collapseLayout->addWidget(lblCollapseCheck,1);

    loDM->addLayout(collapseLayout);

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
    lblDEM_type->setText(" Demand Type");
    lblDEM_type->setMaximumWidth(75);
    lblDEM_type->setMinimumWidth(75);
    loCLimit_header->addWidget(lblDEM_type);

    QLabel *lblDEM_limit = new QLabel();
    lblDEM_limit->setText(" Collapse Limit");
    lblDEM_limit->setMaximumWidth(75);
    lblDEM_limit->setMinimumWidth(75);
    loCLimit_header->addWidget(lblDEM_limit);

    loCLimit_header->addStretch();
    loCLimit_header->setSpacing(10);
    loCLimit_header->setMargin(0);

    loDM->addLayout(loCLimit_header);

    // Collapse Limit list
    QHBoxLayout *loCLimitLayout = new QHBoxLayout();

    loCLimitRemove = new QVBoxLayout();

    loCLimitRemove->addStretch();

    smRemoveCL = new QSignalMapper(this);
    connect(smRemoveCL, SIGNAL(mapped(QWidget*)), this,
            SLOT(removeCollapseLimit(QWidget*)));

    loCLimitLayout->addLayout(loCLimitRemove);

    QScrollArea *saCollapseLims = new QScrollArea;
    saCollapseLims->setWidgetResizable(true);
    saCollapseLims->setLineWidth(0);
    saCollapseLims->setFrameShape(QFrame::NoFrame);

    QWidget *CLWidget = new QWidget;
    loCLList = new QVBoxLayout();
    loCLList->addStretch();
    loCLList->setMargin(0);
    CLWidget->setLayout(loCLList);
    saCollapseLims->setWidget(CLWidget);

    loCLimitLayout->addWidget(saCollapseLims);
    loDM->addLayout(loCLimitLayout);

    // add line
    lineBeta = new QFrame();
    lineBeta->setFrameShape(QFrame::HLine);
    lineBeta->setFrameShadow(QFrame::Sunken);
    loDM->addWidget(lineBeta);

    // additional uncertainty

    QHBoxLayout *betaLayout = new QHBoxLayout();

    addBeta = new QCheckBox();
    addBeta->setText("");
    addBeta->setToolTip("Increase the logarithmic standard deviation of demands \n"
                        "to consider additional sources of uncertainty.");
    addBeta->setChecked(true);
    betaLayout->addWidget(addBeta);

    QLabel *lblBetaCheck = new QLabel();
    lblBetaCheck->setText("Add Uncertainty");
    betaLayout->addWidget(lblBetaCheck,1);

    loDM->addLayout(betaLayout);

    // additional uncertainty values

    QHBoxLayout *betaGMLayout = new QHBoxLayout();

    QLabel *lblbetaGM = new QLabel();
    lblbetaGM->setText("Ground Motion");
    lblbetaGM->setMaximumWidth(100);
    lblbetaGM->setMinimumWidth(100);
    betaGMLayout->addWidget(lblbetaGM);

    addedUncertaintyGM = new QLineEdit();
    addedUncertaintyGM->setToolTip(tr("Uncertainty in the shape and amplitude of the target spectrum \n"
                                      "for intensity- and scenario-based assessment."));
    addedUncertaintyGM->setText("");
    addedUncertaintyGM->setAlignment(Qt::AlignRight);
    betaGMLayout->addWidget(addedUncertaintyGM, 1);

    loDM->addLayout(betaGMLayout);


    QHBoxLayout *betaMLayout = new QHBoxLayout();

    QLabel *lblbetaM = new QLabel();
    lblbetaM->setText("Model");
    lblbetaM->setMaximumWidth(100);
    lblbetaM->setMinimumWidth(100);
    betaMLayout->addWidget(lblbetaM);

    addedUncertaintyModel = new QLineEdit();
    addedUncertaintyModel->setToolTip(tr("Uncertainty resulting from inaccuracies in component modeling,\n"
                                         "damping and mass assumptions."));
    addedUncertaintyModel->setText("");
    addedUncertaintyModel->setAlignment(Qt::AlignRight);
    betaMLayout->addWidget(addedUncertaintyModel, 1);

    loDM->addLayout(betaMLayout);

    // add line
    lineTL = new QFrame();
    lineTL->setFrameShape(QFrame::HLine);
    lineTL->setFrameShadow(QFrame::Sunken);
    loDM->addWidget(lineTL);

    //truncation limits label
    QHBoxLayout *truncationLimitsLayout = new QHBoxLayout();

    QLabel *lblTruncLimits = new QLabel();
    lblTruncLimits->setText("Truncation Limits");
    lblTruncLimits->setToolTip(tr("Specify lower and upper bounds for the truncated lognormal distribution."));
    lblTruncLimits->setMaximumWidth(100);
    lblTruncLimits->setMinimumWidth(100);
    truncationLimitsLayout->addWidget(lblTruncLimits);

    truncationLimitsLayout->addStretch();

    loDM->addLayout(truncationLimitsLayout);

    // truncation limits

    QScrollArea *saTruncLimits = new QScrollArea;
    saTruncLimits->setWidgetResizable(true);
    saTruncLimits->setLineWidth(0);
    saTruncLimits->setFrameShape(QFrame::NoFrame);

    QWidget *TLWidget = new QWidget;
    loTLList = new QVBoxLayout();
    loTLList->addStretch();
    loTLList->setMargin(0);
    TLWidget->setLayout(loTLList);
    saTruncLimits->setWidget(TLWidget);

    loDM->addWidget(saTruncLimits);

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
    needCoupled->setChecked(true);
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

    // inference method

    QHBoxLayout *inferenceLayout = new QHBoxLayout();

    inferenceCombo = new QComboBox();
    inferenceCombo->setToolTip(tr("Select the method to use to infer residual drift values."));
    inferenceCombo->addItem("None",0);
    inferenceCombo->addItem("FEMA P58",1);

    inferenceCombo->setItemData(0, "No inference; residual drifts are either provided by the user or irreparable damage is not part of the assessment.", Qt::ToolTipRole);
    inferenceCombo->setItemData(1, "Residual drifts inferred from peak drifts using the method described in FEMA P58.", Qt::ToolTipRole);

    inferenceLayout->addWidget(inferenceCombo);

    loResidual->addLayout(inferenceLayout);

    // add line
    lineYDR = new QFrame();
    lineYDR->setFrameShape(QFrame::HLine);
    lineYDR->setFrameShadow(QFrame::Sunken);
    loResidual->addWidget(lineYDR);

    //yield drift ratio label
    QHBoxLayout *yieldDriftLayout = new QHBoxLayout();

    QLabel *lblyieldDrift = new QLabel();
    lblyieldDrift->setText("Yield Drift Ratio");
    lblyieldDrift->setToolTip(tr("Interstory drift ratio corresponding to significant yielding in the structure \n"
                              "(i.e. when the forces in the main components of the lateral load resisting \n"
                              "system reach the plastic capacity of the components) in the structure."));
    lblyieldDrift->setMaximumWidth(100);
    lblyieldDrift->setMinimumWidth(100);
    yieldDriftLayout->addWidget(lblyieldDrift);

    yieldDriftLayout->addStretch();

    loResidual->addLayout(yieldDriftLayout);

    // yield drift ratio values

    QScrollArea *saYieldDrift = new QScrollArea;
    saYieldDrift->setWidgetResizable(true);
    saYieldDrift->setLineWidth(0);
    saYieldDrift->setFrameShape(QFrame::NoFrame);

    QWidget *YDWidget = new QWidget;
    loYDList = new QVBoxLayout();
    loYDList->addStretch();
    loYDList->setMargin(0);
    YDWidget->setLayout(loYDList);
    saYieldDrift->setWidget(YDWidget);

    loResidual->addWidget(saYieldDrift);

    /*

    QHBoxLayout *mainHLayout = new QHBoxLayout();
    QVBoxLayout *mainVLayout = new QVBoxLayout();

    // response model ----------------------------------------------------------
    QGroupBox * responseGroupBox = new QGroupBox("Demand Model");
    responseGroupBox->setMaximumWidth(maxWidth);
    QFormLayout * responseFormLayout = new QFormLayout();

    QLabel *describeEDPLabel = new QLabel();
    describeEDPLabel->setText(tr("response description:"));
    responseFormLayout->addRow(describeEDPLabel);

    // EDP data
    QHBoxLayout *EDP_dataLayout = new QHBoxLayout();
    EDPFilePath = new QLineEdit;
    QPushButton *chooseEDP_data = new QPushButton();
    chooseEDP_data->setText(tr("Choose"));
    connect(chooseEDP_data, SIGNAL(clicked()),this,SLOT(chooseEDPFile()));
    EDP_dataLayout->addWidget(EDPFilePath);
    EDP_dataLayout->addWidget(chooseEDP_data);
    EDP_dataLayout->setSpacing(1);
    EDP_dataLayout->setMargin(0);
    responseFormLayout->addRow(tr("    EDP data: "), EDP_dataLayout);

    // EDP distribution

    responseFormLayout->addRow(tr("    EDP distribution:"), demandDistribution);

    // basis of EDP fitting
    EDP_Fitting = new QComboBox();
    EDP_Fitting ->setToolTip(tr("Basis of EDP fitting (only used when the EDP distribution is not empirical)\n"
                                "all results - use all samples\n"
                                "non-collapse results - use only the samples that have all EDP values below the corresponding collapse limits"));
    EDP_Fitting ->addItem("all results",0);
    EDP_Fitting ->addItem("non-collapse results",1);

    EDP_Fitting ->setCurrentIndex(0);
    responseFormLayout->addRow(tr("    Basis:"), EDP_Fitting );

    QSpacerItem *spacerGroups7 = new QSpacerItem(10,10);
    responseFormLayout->addItem(spacerGroups7);

    // set style
    responseFormLayout->setAlignment(Qt::AlignLeft);
    responseFormLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    responseFormLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    responseGroupBox->setLayout(responseFormLayout);
    */

    // assemble the widgets-----------------------------------------------------

    gridLayout->addWidget(DataSourceGB, 0, 0);
    gridLayout->addWidget(DemandModelGB, 1, 0);
    gridLayout->addWidget(DemandSampleGB, 0, 1);
    gridLayout->addWidget(ResidualDriftGB, 1, 1);
    gridLayout->setRowStretch(2, 1);

    this->setLayout(gridLayout);
}

PelicunDemandContainer::~PelicunDemandContainer()
{}

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
    if (arg1 == QString("User Defined")) {
        demandDataPath->setVisible(true);
        btnChooseDemand->setVisible(true);
    } else {
        demandDataPath->setVisible(false);
        btnChooseDemand->setVisible(false);
    }
}

bool PelicunDemandContainer::outputToJSON(QJsonObject &outputObject) {

    return 0;
}

bool PelicunDemandContainer::inputFromJSON(QJsonObject & inputObject) {

    return 0;
}

