#ifndef PelicunLOSS_REPAIR_CONTAINER_H
#define PelicunLOSS_REPAIR_CONTAINER_H

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
#include <QWebEngineView>

#include "SimCenterAppWidget.h"
#include "PelicunShared.h"

#include <quazip.h>
#include <quazipfile.h>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;
class QGridLayout;
class QPushButton;
class QStringListModel;

/*!
 * Widget for general loss assessment settings in PBE tool
 */
class PelicunLossRepairContainer : public SimCenterAppWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of PelicunGeneralSettingsContainer
     */
    explicit PelicunLossRepairContainer(QWidget *parent = 0);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~PelicunLossRepairContainer();

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
    QString getPelicunLossRepairContainerName() const;

    int setAdditionalComponentDB(QString additionalComponentDB);

signals:

public slots:
    int updateAvailableComponents();

    void updateComponentInfo(void);
    void showSelectedComponent(void);
    void showSelectedCType(void);

    void replacementCheckChanged(int newState);
    void cTypeSetupChanged(int newState);

    void addComponentCheckChanged(int newState);

    void chooseConsequenceDataBase(void);
    void exportConsequenceDB(void);

    //void DBSelectionChanged(const QString &arg1);

    void MAPApproachSelectionChanged(const QString &arg1);

    void updateComponentConsequenceDB();

    void chooseMAP(void);

private:

    void deleteCompDB();
    void initPanel();
    QString getDefaultDatabasePath();
    QString generateConsequenceInfo(QString comp_DB_path);

    QWidget * replacementSettings;
    QWidget * repCostSettings;
    QWidget * repTimeSettings;
    QWidget * repCarbonSettings;
    QWidget * repEnergySettings;

    QString   cmpConsequenceDB;
    QString   additionalComponentDB;
    QString   cmpConsequenceDB_viz;
    QString   additionalComponentDB_viz;

    QLineEdit * repCostUnit;
    QLineEdit * repCostMedian;
    QLineEdit * repCostTheta1;
    QLineEdit * repTimeUnit;
    QLineEdit * repTimeMedian;
    QLineEdit * repTimeTheta1;
    QLineEdit * repCarbonUnit;
    QLineEdit * repCarbonMedian;
    QLineEdit * repCarbonTheta1;
    QLineEdit * repEnergyUnit;
    QLineEdit * repEnergyMedian;
    QLineEdit * repEnergyTheta1;
    QLineEdit * leAdditionalComponentDB;
    QLineEdit * mapPath;

    QComboBox * repCostDistribution;
    QComboBox * repTimeDistribution;
    QComboBox * repCarbonDistribution;
    QComboBox * repEnergyDistribution;
    QComboBox * databaseConseq;
    QComboBox * mapApproach;
    QComboBox * selectedCompCombo;
    QStringListModel *selectedCBModel;
    QComboBox * selectedCType;

    QPushButton * btnChooseConsequence;
    QPushButton * btnChooseMAP;

    QCheckBox *replacementCheck;
    QCheckBox *addComp;
    QCheckBox *cbCTypeCost;
    QCheckBox *cbCTypeTime;
    QCheckBox *cbCTypeCarbon;
    QCheckBox *cbCTypeEnergy;

    QLabel *compInfo;

    // component DL information
    QMap<QString, QMap<QString, QString>* > *compDB;

    QGridLayout *gridLayout;

    // Display consequence information
    QWebEngineView *consequenceViz; 

};

#endif // PelicunLOSS_REPAIR_CONTAINER_H
