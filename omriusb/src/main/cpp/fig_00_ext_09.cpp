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

#include "fig_00_ext_09.h"

#include <iostream>

Fig_00_Ext_09::Fig_00_Ext_09(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_09::~Fig_00_Ext_09() {

}

void Fig_00_Ext_09::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        bool extendedFieldPresent = ((*figIter & 0x80) >> 7) & 0xFF;
        //bool rfa1 = ((*figIter & 0x40) >> 6) & 0xFF;
        bool isNegativeOffset = ((*figIter & 0x20) >> 5) & 0xFF;
        m_lto = (*figIter++ & 0x1F) & 0xFF;
        if(isNegativeOffset) {
            m_lto *= -1;
        }

        m_ensembleEcc = (*figIter++ & 0xFF);
        m_interTableId = (*figIter++ & 0xFF);

        //std::cout << m_logTag << " LTO: " << +m_lto << " Extension: " << std::boolalpha << extendedFieldPresent << std::noboolalpha << std::endl;
        if(extendedFieldPresent) {
            while(figIter < figData.end()) {
                uint8_t numSrvs = ((*figIter & 0xC0) >> 6) & 0xFF;
                uint8_t rfa2 = (*figIter++ & 0x3F);
                uint8_t ecc = (*figIter++ & 0xFF);
                for(uint8_t i = 0; i < numSrvs; i++) {
                    uint16_t sid = (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);
                }
            }
        }
    }
}

int Fig_00_Ext_09::getLocalTimeOffset() const {
    return m_lto;
}

uint8_t Fig_00_Ext_09::getEnsembleEcc() const {
    return m_ensembleEcc;
}

uint8_t Fig_00_Ext_09::getInternationalTableId() const {
    return m_interTableId;
}
