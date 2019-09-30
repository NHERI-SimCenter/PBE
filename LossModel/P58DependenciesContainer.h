#ifndef P58DEPENDENCIES_CONTAINER_H
#define P58DEPENDENCIES_CONTAINER_H

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

#include <QCheckBox>

#include "SimCenterAppWidget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QRadioButton;

/*!
 * Widget for general loss assessment settings in PBE tool
 */
class P58DependenciesContainer : public SimCenterAppWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of P58GeneralSettingsContainer
     */
    explicit P58DependenciesContainer(QWidget *parent = 0);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~P58DependenciesContainer();

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
    QString getP58DependenciesContainerName() const;
    
signals:

public slots:
       
private:

    /*
    QLineEdit * yieldDriftValue;
    QLineEdit * driftDetLim;
    QLineEdit * accDetLim;
    QLineEdit * irrepResDriftMedian;
    QLineEdit * irrepResDriftStd;
    QLineEdit * driftColLim;
    QLineEdit * accColLim;
    QLineEdit * realizationsValue;
    QLineEdit * addedUncertaintyGM;
    QLineEdit * addedUncertaintyModel;
    QLineEdit * peakPopulation;
    QLineEdit * replacementCostValue;
    QLineEdit * replacementTimeValue;
    //QLineEdit * fragilityFolderPath;
    QLineEdit * populationFilePath;
    QLineEdit * colProbValue;
    */

    QComboBox * quantityDep;
    QComboBox * fragilityDep;
    QComboBox * costDep;
    QComboBox * timeDep;
    QComboBox * injuryDep;
    QComboBox * redTagDep;
    /*
    QComboBox * occupancyType;
    QComboBox * collProbApproach;
    QComboBox * EDP_Distribution;
    QComboBox * EDP_Fitting;
    QComboBox * colBasis;
    */

    QCheckBox * costAndTimeDep;
    QCheckBox * injSeverityDep;

    /*
    QCheckBox * needRecCost;
    QCheckBox * needRecTime;
    QCheckBox * needInjuries;
    QCheckBox * needRedTag;
    */

    QVBoxLayout *mainLayout; /*!< Main layout for general settings */
};

#endif // P58DEPENDENCIES_CONTAINER_H
