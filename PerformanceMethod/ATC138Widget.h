#ifndef ATC138Widget_H
#define ATC138Widget_H
/* *****************************************************************************
Copyright (c) 2026, The Regents of the University of California (Regents).
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

// Written: Adam Zsarnoczay

#include <SimCenterAppWidget.h>

class QLineEdit;
class QLabel;
class QSpinBox;
class QDoubleSpinBox;
class QCheckBox;
class QPushButton;
class GeneralInformationWidget;
class PelicunLossRepairContainer;

class ATC138Widget : public SimCenterAppWidget
{
    Q_OBJECT

public:
    ATC138Widget(QWidget *parent = 0);
    ~ATC138Widget();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &jsonObj);
    bool inputAppDataFromJSON(QJsonObject &jsonObject);
    bool outputCitation(QJsonObject &jsonObject);

    void clear(void);

private slots:
    void handleTenantUnitBrowsePressed();
    void handleOptionalInputsBrowsePressed();
    void onTenantUnitCheckChanged(int state);
    void onOptionalInputsCheckChanged(int state);

    // GI -> ATC138 sync
    void setNumStoriesAndHeight(int numFloors, double height);
    void setBuildingDimensions(double newWidth, double newDepth, double planArea);
    void onUnitLengthChanged(QString unitName);

    // ATC138 -> GI sync
    void updateGIDimensions();
    void updateGIStories();

    // LossRepair -> ATC138 sync
    void setReplacementCost(QString median);

    // ATC138 -> LossRepair sync
    void updateLossRepairReplacementCost();

    // Recalculate derived fields
    void recalcTypStoryHeight();
    void recalcPlanArea();

private:
    double convertToFeet(double value, const QString &fromUnit);
    double convertFromFeet(double value, const QString &toUnit);
    QString getGeneralInputsFilePath();

    GeneralInformationWidget *theGI;
    PelicunLossRepairContainer *theLossRepair;
    QString currentLengthUnit;

    // GI-linked fields
    QLineEdit *lengthSide1LE;
    QLineEdit *lengthSide2LE;
    QLineEdit *numStoriesLE;
    QLineEdit *heightLE;
    QLabel *typStoryHtValueLabel;
    QLabel *planAreaValueLabel;

    // ATC138-specific fields
    QLineEdit *replacementCostLE;
    QSpinBox *numEntryDoorsSB;
    QDoubleSpinBox *typStructBayLengthDSB;
    QDoubleSpinBox *peakOccRateDSB;
    QDoubleSpinBox *engineeringCostRatioDSB;
    QSpinBox *numElevatorsSB;

    // Optional file inputs
    QCheckBox *useTenantUnitCB;
    QLineEdit *tenantUnitPathLE;
    QPushButton *tenantUnitBrowseBtn;

    QCheckBox *useOptionalInputsCB;
    QLineEdit *optionalInputsPathLE;
    QPushButton *optionalInputsBrowseBtn;

    // Guard against signal loops during GI sync
    bool updatingGI;
    bool updatingLossRepair;
};

#endif // ATC138Widget_H
