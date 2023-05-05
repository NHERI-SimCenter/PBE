/* *****************************************************************************
Copyright (c) 2016-2023, The Regents of the University of California (Regents).
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

#include "PelicunShared.h"

void
parseCSVLine(QString &line, QStringList &line_list, 
   SimCenterAppWidget *parent)
{
    // parse the line considering "" and , and \n
    bool in_commented_block = false;
    bool save_element = false;
    int c_0 = 0;

    for (int c=0; c<line.length(); c++) {
        if (line[c] == "\"") {
            if (in_commented_block) {
                save_element = true;
                in_commented_block = false;
            } else {
                in_commented_block = true;
                c_0 = c+1;
            }
        } else if (line[c] == ",") {
            if (c_0 == c){
                line_list.append("");
                c_0++;
            } else if (in_commented_block == false) {
                save_element = true;
            }
        } else if (c == line.length()-1) {
            save_element = true;
            c++;
        }

        if (save_element) {
            QString element = line.mid(c_0, c-c_0);

            c_0 = c+1;
            line_list.append(element);
            save_element = false;

            if (c_0 < line.length()-1) {
                if (line[c_0-1] == "\"") {
                    if (line[c_0] != ",") {
                        if (parent != nullptr){
                           parent->statusMessage("Error while parsing CSV file at the following line: " + line);   
                        }
                    } else {
                        c_0 ++;
                        c++;
                    }
                }
            }
        }
    }
}