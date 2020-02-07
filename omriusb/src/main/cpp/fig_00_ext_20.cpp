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

#include "fig_00_ext_20.h"

#include <iostream>

Fig_00_Ext_20::Fig_00_Ext_20(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_20::~Fig_00_Ext_20() {

}

void Fig_00_Ext_20::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        uint32_t sId;
        if(!isDataService()) {
            sId = static_cast<uint32_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        } else {
            sId = static_cast<uint32_t>((*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        }

        uint8_t scIdS = static_cast<uint8_t>(((*figIter & 0xF0) >> 4) & 0xFF);
        Fig_00_Ext_20::CHANGETYPE changeFlags = static_cast<Fig_00_Ext_20::CHANGETYPE>(((*figIter & 0x0C) >> 2) & 0xFF);
        Fig_00_Ext_20::PARTTIME ptFlag = static_cast<Fig_00_Ext_20::PARTTIME>(((*figIter & 0x02) >> 2) & 0xFF);

        std::cout << m_logTag << " SId: " << std::hex << sId << " ScIdS: " << scIdS << std::dec << std::endl;

        bool scFlag = (*figIter++ & 0x01) != 0;
        if(scFlag) {
            bool caFlag = (((*figIter & 0x80) >> 7) & 0xFF) != 0;
            bool adFlag = (((*figIter & 0x40) >> 6) & 0xFF) != 0;
            uint8_t scTy = static_cast<uint8_t>(*figIter++ & 0x3F);

            uint8_t date = static_cast<uint8_t>(((*figIter & 0xF8) >> 3) & 0xFF);
            uint8_t hour = static_cast<uint8_t>(((*figIter++ & 0x07) << 2) | ((*figIter & 0xC0) >> 6));
            uint8_t minute = static_cast<uint8_t>(*figIter++ & 0x3F);
            uint8_t second = static_cast<uint8_t>(((*figIter & 0xFC) >> 2) & 0xFF);

            bool sidFlag = (((*figIter & 0x02) >> 1) & 0xFF) != 0;
            bool eidFlag = (*figIter++ & 0x01) != 0;
            if(sidFlag) {
                uint32_t sId;
                if(!isDataService()) {
                    sId = static_cast<uint32_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
                } else {
                    sId = static_cast<uint32_t>((*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
                }

                if(eidFlag) {
                    uint16_t transEid = static_cast<uint16_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
                }
            }
        }
    }
}
