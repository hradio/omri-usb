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

#include "fig_00_ext_08.h"

#include <iostream>

Fig_00_Ext_08::Fig_00_Ext_08(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_08::~Fig_00_Ext_08() {

}

void Fig_00_Ext_08::parseFigData(const std::vector<uint8_t> &figData) {
    auto figIter = figData.cbegin() +1;
    //std::cout << m_logTag << " ################## Begin ##################### " << std::endl;
    while(figIter < figData.cend()) {
        ServiceComponentGlobalDefinition globalDef;

        globalDef.isProgrammeService = !isDataService();
        if(!isDataService()) {
            globalDef.serviceId = (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);
        } else {
            globalDef.serviceId = (*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);
        }

        bool extFLag = (*figIter & 0x80) >> 7;
        //bool rfa = (*figIter & 0x70) >> 4;
        globalDef.scIdS = (*figIter++ & 0x0F) & 0xFF;

        //std::cout << m_logTag << " SId: " << std::hex << +globalDef.serviceId << " ScIdS: " << +globalDef.scIdS << std::dec << std::endl;

        globalDef.isShortForm = ((*figIter & 0x80) >> 7) == 0;
        if(globalDef.isShortForm) {
            //bool rfu = (*figIter & 0x40) >> 6;
            globalDef.subchannelId = (*figIter++ & 0x3F) & 0xFF;
        } else {
            //uint8_t rfaLs = (*figIter & 0x70) >> 4;
            globalDef.serviceComponentId = (*figIter++ & 0x0F) << 8 | (*figIter++ & 0xFF);
        }

        if(extFLag) {
            //uint8_t rfaField = (*figIter++ & 0xFF);
            figIter++;
        }

        m_globalDefinitions.push_back(globalDef);
    }
    //std::cout << m_logTag << " ################## End ##################### " << std::endl;
}

const std::vector<Fig_00_Ext_08::ServiceComponentGlobalDefinition>& Fig_00_Ext_08::getGLobalDefinitions() const {
    return m_globalDefinitions;
}
