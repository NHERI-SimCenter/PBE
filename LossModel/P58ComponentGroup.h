#ifndef COMPONENTGROUP_H
#define COMPONENTGROUP_H

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

// Written: fmckenna, mhgardner, adamzs

#include <QCheckBox>

#include "SimCenterWidget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QLabel;
class QHBoxLayout;
class QRadioButton;

/*!
 * Widget for components in PBE tool
 */
class ComponentGroup : public SimCenterWidget
{
    Q_OBJECT
public:

    /*!
     * @contructor Constructor taking pointer to parent widget
     * @param[in] parent Parent widget of Component
     */
    explicit ComponentGroup(QWidget *parent = nullptr,
                             QMap<QString, QString> *CG_data_in = nullptr);

    /*!
     * @destructor Virtual destructor
     */
    virtual ~ComponentGroup();

    /*!
     * Writes component contents to JSON
     * @param[in, out] outputObject Write contents of component to this object
     * @return Returns true if successful, false otherwise
     */
    virtual bool outputToJSON(QJsonObject &outputObject);

    /*!
     * Reads component contents from JSON
     * @param[in] inputObject Read component contents from this object
     * @return Returns true if successfull, false otherwise
     */
    virtual bool inputFromJSON(const QJsonObject & inputObject);

    /*!
     * Checks whether button has been checked to remove component
     * @return Current state of button
     */
    bool isSelectedForRemoval() const;

    /*!
     * Get the name of the component
     * @return Returns the name of the component as a QString
     */
    QString getComponentName() const;

    void delete_CG_data();
    
signals:

public slots:

    void storeCGLocation(void);
    void storeCGDirection(void);
    void storeCGMedian(void);
    void storeCGUnit(void);
    void storeCGDistribution(void);
    void storeCGCOV(void);
       
private:

    QMap<QString, QString> *CG_data;

    QLineEdit *cgLocation;
    QLineEdit *cgDirection;
    QLineEdit *cgMedianQNT;
    QLineEdit *cgUnit;
    QComboBox *cgDistribution;
    QLineEdit *cgCOV;

    QRadioButton *button; /*!< Button for selecting component */
    QHBoxLayout *mainLayout; /*!< Main layout for component */
};

#endif // COMPONENT_H
