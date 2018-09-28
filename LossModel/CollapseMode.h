#ifndef COLLAPSE_MODE_H
#define COLLAPSE_MODE_H

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

#include "SimCenterWidget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QHBoxLayout;
class RandomVariableDistribution;
class QRadioButton;

/*!
 * Widget for collapse modes in PBE tool
 */
class CollapseMode : public SimCenterWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of CollapseMode
     */
    explicit CollapseMode(QWidget *parent = 0);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~CollapseMode();

    /*!
     * Writes collapse mode contents to JSON
     * @param[in, out] outputObject Write contents of collapse mode to this object
     * @return Returns true if successful, false otherwise
     */
    virtual bool outputToJSON(QJsonObject &outputObject);

    /*!
     * Reads collapse mode contents from JSON
     * @param[in] inputObject Read collapse mode contents from this object
     * @return Returns true if successfull, false otherwise
     */
    virtual bool inputFromJSON(const QJsonObject & inputObject);

    /*!
     * Checks whether button has been checked to remove collapse mode
     * @return Current state of button
     */
    bool isSelectedForRemoval() const;

    /*!
     * Get the name of the collapse mode
     * @return Returns the name of the collapse mode as a QString
     */
    QString getCollapseModeName() const;

    
signals:

public slots:
       
private:

    QLineEdit * collapseModeName; /*!< Line edit widget to input collapse mode name */
    QLineEdit * collapseModeProbability; /*!< Line edit widget to input collapse mode ID */
    QLineEdit * collapseModeAffectedArea; /*!< Line edit widget to input collapse mode quantity */
    QLineEdit * collapseModeInjuries; /*!< Line edit widget to input collapse mode coefficient of variation */
    
    QRadioButton *button; /*!< Button for selecting collapse mode */
    QHBoxLayout *mainLayout; /*!< Main layout for collapse mode */
};

#endif // COLLAPSE_MODE_H
