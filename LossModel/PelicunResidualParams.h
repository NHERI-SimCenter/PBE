#ifndef RESIDUALPARAMS_H
#define RESIDUALPARAMS_H

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

// Written: adamzs

#include <QCheckBox>

#include "SimCenterWidget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QHBoxLayout;
class QRadioButton;

/*!
 * Widget for collapse limits in PBE tool
 */
class ResidualParam : public SimCenterWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of Component
     */
    explicit ResidualParam(QWidget *parent = nullptr,
                             QMap<QString, QString> *RP_data_in = nullptr);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~ResidualParam();

    /*!
     * Writes collapse limit data to JSON
     * @param[in, out] outputObject Write collapse limit data to this object
     * @return Returns true if successful, false otherwise
     */
    virtual bool outputToJSON(QJsonObject &outputObject);

    /*!
     * Reads collapse limit data from JSON
     * @param[in] inputObject Read collapse limit data from this object
     * @return Returns true if successfull, false otherwise
     */
    virtual bool inputFromJSON(const QJsonObject & inputObject);

    void delete_RP_data();

signals:

public slots:

    void storeRPDirection(void);
    void storeRPYieldDrift(void);

private:

    QMap<QString, QString> *RP_data;

    QLineEdit *rpDirection;
    QLineEdit *rpYieldDrift;

    QHBoxLayout *mainLayout; /*!< Main layout for truncation limit */
};

#endif // RESIDUALPARAMS_H
