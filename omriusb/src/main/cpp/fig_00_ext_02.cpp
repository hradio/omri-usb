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

#include "fig_00_ext_02.h"

#include <iostream>

Fig_00_Ext_02::Fig_00_Ext_02(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_02::~Fig_00_Ext_02() {

}

void Fig_00_Ext_02::parseFigData(const std::vector<uint8_t> &figData) {
    //std::cout << m_logTag << " ------------- Begin ---------------- " << std::endl;
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        ServiceDescription srvDesc;

        srvDesc.isProgrammeService = !isDataService();

        if(!isDataService()) {
            srvDesc.serviceId = static_cast<uint32_t>(((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));
        } else {
            srvDesc.serviceId = static_cast<uint32_t>(((*figIter++ & 0xFF) << 24) | ((*figIter++ & 0xFF) << 16) | ((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));
        }

        //std::cout << m_logTag << " ServiceId: " << std::hex << +srvDesc.serviceId << std::dec << " DataService: " << std::boolalpha << isDataService() << std::noboolalpha << std::endl;

        //bool Rfa = (*figIter >> 7) & 0x01;
        srvDesc.caId = static_cast<uint8_t>((*figIter & 0x70) >> 4);
        srvDesc.numServiceComponents = static_cast<uint8_t>((*figIter++ & 0x0F) & 0xFF);

        for(uint8_t i = 0; i < srvDesc.numServiceComponents; i++) {
            ServiceComponentDescription srvCompDesc;

            srvCompDesc.transportModeId = static_cast<Fig_00_Ext_02::TMID>((*figIter & 0xC0) >> 6);

            switch (srvCompDesc.transportModeId) {
                case Fig_00_Ext_02::TMID::MSC_STREAM_AUDIO:
                case Fig_00_Ext_02::TMID::MSC_STREAM_DATA: {
                    srvCompDesc.serviceComponentType = static_cast<uint8_t>((*figIter++ & 0x3F) & 0xFF);
                    srvCompDesc.subChannelId = static_cast<uint8_t>((*figIter & 0xFC) >> 2);
                    srvCompDesc.isPrimary = ((*figIter & 0x02) >> 1) != 0;
                    srvCompDesc.isCaApplied = ((*figIter++ & 0x01) & 0xFF) != 0;
                    break;
                }
                case Fig_00_Ext_02::TMID::MSC_PACKET_MODE_DATA: {
                    srvCompDesc.serviceComponentId = static_cast<uint16_t>((((*figIter++ & 0x3F) & 0xFF) << 6) | (((*figIter & 0xFC) & 0xFF) >> 2));
                    srvCompDesc.isPrimary = ((*figIter & 0x02) >> 1) != 0;
                    srvCompDesc.isCaApplied = ((*figIter++ & 0x01) & 0xFF) != 0;
                    break;
                }
                case Fig_00_Ext_02::TMID::RESERVED: {
                    break;
                }
            default:
                break;
            }

            srvDesc.serviceComponents.push_back(srvCompDesc);
        }

        m_serviceDescriptions.push_back(srvDesc);
    }
    //std::cout << m_logTag << " ------------- End ---------------- " << std::endl;
}

const std::vector<Fig_00_Ext_02::ServiceDescription>& Fig_00_Ext_02::getServiceDescriptions() const {
    return m_serviceDescriptions;
}
