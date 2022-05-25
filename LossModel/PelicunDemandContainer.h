#ifndef PelicunDEMAND_CONTAINER_H
#define PelicunDEMAND_CONTAINER_H

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

#include <QCheckBox>

#include "SimCenterAppWidget.h"
#include "PelicunCollapseLimit.h"
#include "PelicunTruncationLimit.h"
#include "PelicunResidualParams.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;
class QPushButton;
class QCheckBox;
class QGridLayout;
class QFrame;
class QSignalMapper;

/*!
 * Widget for general loss assessment settings in PBE tool
 */
class PelicunDemandContainer : public SimCenterAppWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of PelicunGeneralSettingsContainer
     */
    explicit PelicunDemandContainer(QWidget *parent = 0);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~PelicunDemandContainer();

    /*!
     * Writes general settings contents to JSON
     * @param[in, out] outputObject Write contents of general settings to this object
     * @return Returns true if successful, false otherwise
     */
    virtual bool outputToJSON(QJsonObject &outputObject);

    /*!
     * Reads general settings contents from JSON
     * @param[in] inputObject Read general settings contents from this object
     * @return Returns true if successfull, false otherwise
     */
    virtual bool inputFromJSON(QJsonObject &inputObject);

    /*!
     * Get the name of the general settings
     * @return Returns the name of the general settings as a QString
     */
    QString getPelicunDemandContainerName() const;

    int setDemandData(QString demandPath);

signals:

public slots:

    void addCollapseLimit(QMap<QString, QString> *CL_data_in=nullptr);
    void removeCollapseLimit(QWidget *theCollapseLimit);

    void addTruncationLimit(QMap<QString, QString> *TL_data_in=nullptr);
    void removeTruncationLimit(QWidget *theTruncationLimit);

    void addResidualParam(QMap<QString, QString> *RP_data_in=nullptr);
    void removeResidualParam(QWidget *theResidualParam);

    void DemandDSSelectionChanged(const QString &arg1);

    void chooseDemandData(void);

private:

    QSignalMapper *smRemoveCL;
    QSignalMapper *smRemoveTL;
    QSignalMapper *smRemoveRP;

    QLineEdit *demandDataPath;
    QLineEdit *addedUncertaintyGM;
    QLineEdit *addedUncertaintyModel;
    QLineEdit *sampleSize;

    QComboBox *databaseCombo;
    QComboBox *demandDistribution;
    QComboBox *inferenceCombo;

    QPushButton *btnChooseDemand;

    QCheckBox *removeCollapse;
    QCheckBox *addBeta;
    QCheckBox *needCoupled;

    QVBoxLayout *loCLList;
    QVBoxLayout *loTLList;
    QVBoxLayout *loRPList;
    QVBoxLayout *loCollLimRemove;
    QVBoxLayout *loTruncLimRemove;
    QVBoxLayout *loResidualPRemove;

    QFrame *lineTL;
    QFrame *lineBeta;
    QFrame *lineCollapse;
    QFrame *lineYDR;

    QLineEdit * driftDetLim;
    QLineEdit * accDetLim;
    QLineEdit * realizationsValue;
    QLineEdit * EDPFilePath;

    QComboBox * EDP_Fitting;

    // collapse limit data
    QVector<QMap<QString, QString>* > *collConfig;

    QVector<CollapseLimit *> vCollapseLimits;
    QVector<QPushButton *> vRemoveCLButtons;

    // truncation limit data
    QVector<QMap<QString, QString>* > *truncConfig;

    QVector<TruncationLimit *> vTruncationLimits;
    QVector<QPushButton *> vRemoveTLButtons;

    // residual drift data
    QVector<QMap<QString, QString>* > *residualConfig;

    QVector<ResidualParam *> vResidualParams;
    QVector<QPushButton *> vRemoveRPButtons;

    QGridLayout *gridLayout; /*!< Main layout for general settings */
};

#endif // PelicunDEMAND_CONTAINER_H
