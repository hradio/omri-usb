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

#include "fig_00_ext_03.h"

#include <iostream>

Fig_00_Ext_03::Fig_00_Ext_03(const std::vector<uint8_t> &figData) : Fig_00(figData) {

    parseFigData(figData);
}

Fig_00_Ext_03::~Fig_00_Ext_03() {

}

void Fig_00_Ext_03::parseFigData(const std::vector<uint8_t>& figData) {
    //std::cout << m_logTag << "------------ Begin ------------ " << std::endl;
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        PacketModeServiceComponentDescription packetDesc;

        packetDesc.isProgrammeService = !isDataService();
        packetDesc.serviceComponentId = (*figIter++ & 0xFF) << 4 | (*figIter & 0xF0) >> 4;
        //std::cout << m_logTag << " SCID: " << std::hex << +m_srvComponentId << std::dec << std::endl;
        //uint8_t rfa = (*figIter & 0x0E) >> 1;
        packetDesc.caOrganizationPresent = (*figIter++ & 0x01);
        packetDesc.dataGroupTransportUsed = (((*figIter & 0x80) >> 7) == 0);
        //bool rfu = (*figIter & 0x40) >> 6;
        packetDesc.dataServiceComponentType = (*figIter++ & 0x3F) & 0xFF;
        packetDesc.subchannelId = ((*figIter & 0xFC) >> 2) & 0xFF;
        //std::cout << m_logTag << " SubChId: " << std::hex << +packetDesc.subchannelId << std::dec << std::endl;
        packetDesc.packetAddress = (*figIter++ & 0x03) << 8 | (*figIter++ & 0xFF);

        if(packetDesc.caOrganizationPresent) {
            packetDesc.caOrganization = (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);
        }

        m_srvDescs.push_back(packetDesc);
    }

    //std::cout << m_logTag << "------------ End ------------ " << std::endl;
}

const std::vector<Fig_00_Ext_03::PacketModeServiceComponentDescription>& Fig_00_Ext_03::getPacketModeServiceDescriptions() const {
    return m_srvDescs;
}
