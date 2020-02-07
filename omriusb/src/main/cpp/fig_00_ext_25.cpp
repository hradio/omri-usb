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

#include "fig_00_ext_25.h"

#include <iostream>

Fig_00_Ext_25::Fig_00_Ext_25(const std::vector<uint8_t> &figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_25::~Fig_00_Ext_25() {

}

void Fig_00_Ext_25::parseFigData(const std::vector<uint8_t> &figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        uint16_t sid = static_cast<uint16_t>(((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));
        uint16_t asuFlags = static_cast<uint16_t>(((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));
        //uint8_t rfa = ((*figIter & 0xFC) >> 2) & 0xFF;
        uint8_t numTargets = static_cast<uint8_t>((*figIter++ & 0x03) & 0xFF);
        for(uint8_t i = 0; i < numTargets; i++) {
            uint8_t oeClusterId = static_cast<uint8_t>(*figIter++ & 0xFF);
            uint16_t eId = static_cast<uint16_t>(((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));

            std::cout << m_logTag << " SId: " << std::hex << +sid << " OEClusterId: " << +oeClusterId << " EId: " << +eId << std::dec << std::endl;
        }
    }
}
