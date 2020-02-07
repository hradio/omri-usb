/*
 * Copyright (C) 2018 IRT GmbH
 *
 * Author:
 *  Fabian Sattler
 *
 * This file is a part of IRT DAB library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef FIG_01
#define FIG_01

#include <vector>
#include <string>

#include "fig.h"

class Fig_01 : public Fig {

public:
    virtual ~Fig_01() {}

    /*
     * ETS 300 401 clause 5.2.2.2.1
     * The FIG type 1 is used to signal labels for display.
     */

    /*
     * Charset: this 4-bit field shall identify a character set to qualify the character information contained in the FIG type 1
     * field. The interpretation of this field shall be as defined in TS 101 756, table 1.
     */
    virtual uint8_t getCharset() const { return m_charSet; }

    /*
     * OE: this 1-bit flag shall indicate whether the information is related to this or another ensemble, as follows:
     * 0: this ensemble;
     * 1: other ensemble (or FM or AM services)
     */
    virtual bool isOtherEnsemble() const { return m_isOtherEnsemble; }

protected:
    Fig_01(const std::vector<uint8_t>& figData) : m_charSet((figData[0] & 0xF0) >> 4), m_isOtherEnsemble((figData[0] & 0x08) >> 3) {}

    inline void parseLabel(std::vector<uint8_t>::const_iterator& labelIter, std::string& label, std::string& shortLabel) {
        label.resize(16);
        label.insert(label.begin(), labelIter, labelIter+16);

        labelIter += 16;

        shortLabel.resize(8);
        int shortCnt = 0;
        for(int i = 0; i < 2; i++) {
            for(int j = 0; j <= 7; j++) {

                //sanity check
                if(shortCnt > 7) {
                    break;
                }

                if((((*labelIter << j) >> 7) & 0x01)) {
                    shortLabel[shortCnt] = label.data()[j + i*7];
                    shortCnt++;
                }
            }
            labelIter++;
        }
    }

    inline void parseLabel(const std::vector<uint8_t>& labelData, std::string& label, std::string& shortLabel) {
        auto labelIter = labelData.begin();
        while(labelIter < labelData.end()) {
            label.resize(16);
            label.insert(label.begin(), labelIter, labelIter+16);

            labelIter += 16;

            shortLabel.resize(8);
            int shortCnt = 0;
            for(int i = 0; i < 2; i++) {
                for(int j = 0; j <= 7; j++) {

                    //sanity check
                    if(shortCnt > 7) {
                        break;
                    }

                    if((((*labelIter << j) >> 7) & 0x01)) {
                        shortLabel[shortCnt] = label.data()[j + i*7];
                        shortCnt++;
                    }
                }
                labelIter++;
            }
        }
    }

private:
    const uint8_t m_charSet;
    const bool m_isOtherEnsemble;
};

#endif // FIG_01

