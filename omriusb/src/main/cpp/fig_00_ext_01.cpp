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

#include "fig_00_ext_01.h"

#include <iostream>

constexpr uint16_t Fig_00_Ext_01::DAB_SUBCHANNEL_SHORTFORM_PROTECTION_TABLE[64][3];

Fig_00_Ext_01::Fig_00_Ext_01(const std::vector<uint8_t> &figData) : Fig_00(figData) {

    parseFigData(figData);
}

Fig_00_Ext_01::~Fig_00_Ext_01() {

}

void Fig_00_Ext_01::parseFigData(const std::vector<uint8_t> &figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        BasicSubchannelOrganization subChanOrga;

        subChanOrga.isProgrammeService = !isDataService();
        subChanOrga.subChannelId = static_cast<uint8_t>((*figIter & 0xFC) >> 2);
        subChanOrga.startAddress = static_cast<uint16_t>(((*figIter++ & 0x03) << 8) | (*figIter++ & 0xFF));
        bool longForm = ((*figIter >> 7) & 0x01) != 0;

        if(!longForm) {
            //std::cout << m_logTag << " Shortform: " << +subChanOrga.subChannelId << std::endl;
            bool tableSwitch = ((*figIter & 0x40) >> 6) != 0;
            uint8_t tableIndex = static_cast<uint8_t>((*figIter++ & 0x3F) & 0xFF);
            subChanOrga.protectionType = PROTECTION_TYPE_UEP;
            subChanOrga.uepTableIndex = tableIndex;
            subChanOrga.subChannelSize = DAB_SUBCHANNEL_SHORTFORM_PROTECTION_TABLE[tableIndex][0];
            subChanOrga.protectionLevel = static_cast<uint8_t >(DAB_SUBCHANNEL_SHORTFORM_PROTECTION_TABLE[tableIndex][1]);
            subChanOrga.subChannelBitrate = DAB_SUBCHANNEL_SHORTFORM_PROTECTION_TABLE[tableIndex][2];
            subChanOrga.protectionLevelType = static_cast<Fig_00_Ext_01::PROTECTIONLEVEL>(subChanOrga.protectionLevel);
            subChanOrga.convolutionalCodingRate = Fig_00_Ext_01::CONVOLUTIONAL_CODING_RATE[subChanOrga.protectionLevel];
        } else {
            uint8_t option = static_cast<uint8_t>((*figIter & 0x70) >> 4);
            uint8_t protectionLevel = static_cast<uint8_t>((*figIter & 0x0C) >> 2);
            subChanOrga.subChannelSize = static_cast<uint16_t>(((*figIter++ & 0x03) << 8) | (*figIter++ & 0xFF));
            subChanOrga.protectionLevel = protectionLevel;
            subChanOrga.uepTableIndex = 0xFF;
            switch (option) {
                case 0: {
                    subChanOrga.protectionLevelType = static_cast<Fig_00_Ext_01::PROTECTIONLEVEL>(protectionLevel);
                    subChanOrga.convolutionalCodingRate = Fig_00_Ext_01::CONVOLUTIONAL_CODING_RATE[protectionLevel];
                    subChanOrga.subChannelBitrate = static_cast<uint16_t>((subChanOrga.subChannelSize * 8) / PROTECTIONLEVEL_SUBCHAN_SIZE[protectionLevel]);
                    subChanOrga.protectionType = PROTECTION_TYPE_EEPA_8N;
                    break;
                }
                case 1: {
                    subChanOrga.protectionLevelType = static_cast<Fig_00_Ext_01::PROTECTIONLEVEL>(protectionLevel+4); //+4 for the right index in the enum
                    subChanOrga.convolutionalCodingRate = Fig_00_Ext_01::CONVOLUTIONAL_CODING_RATE[protectionLevel+4];
                    subChanOrga.subChannelBitrate = static_cast<uint8_t>((subChanOrga.subChannelSize * 32) / PROTECTIONLEVEL_SUBCHAN_SIZE[protectionLevel+4]);
                    subChanOrga.protectionType = PROTECTION_TYPE_EEPB_32N;
                    break;
                }
                default:
                    //RFU
                    break;
            }
        }

        //std::cout << m_logTag << " SubchanID: " << +subChanOrga.subChannelId << " SubchanSize: " << +subChanOrga.subChannelSize << std::endl;
        m_subChanOrgas.push_back(subChanOrga);
    }
}

const std::vector<Fig_00_Ext_01::BasicSubchannelOrganization>& Fig_00_Ext_01::getSubchannelOrganizations() const {
    return m_subChanOrgas;
}
