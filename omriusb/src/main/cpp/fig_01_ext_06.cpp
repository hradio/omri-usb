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

#include "fig_01_ext_06.h"

#include <iostream>

Fig_01_Ext_06::Fig_01_Ext_06(const std::vector<uint8_t>& figData) : Fig_01(figData) {

}

Fig_01_Ext_06::~Fig_01_Ext_06() {

}

void Fig_01_Ext_06::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.begin() +1;
    while(figIter < figData.end()) {
        m_isProgramme = ((*figIter & 0x80) >> 7) == 0;
        //uint8_t rfa1 = (*figIter & 0x70) >> 4;
        m_scIdS = (*figIter++ & 0x0F) & 0xFF;

        if(m_isProgramme) {
            m_serviceId = static_cast<uint32_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        } else {
            m_serviceId = static_cast<uint32_t>((*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        }

        //uint8_t rfa2 = (*figIter & 0xC0) >> 6;
        //bool rfu = (*figIter & 0x20) >> 5;
        m_xpadAppType = static_cast<uint8_t>((*figIter++ & 0x1F) & 0xFF);

        parseLabel(std::vector<uint8_t>(figIter, figIter+18), m_uappLabel, m_uappShortLabel);
        std::cout << m_logTag << " XPAD SCIDs: " << std::hex << +m_scIdS << std::dec << " - " << m_uappLabel << " : " << m_uappShortLabel << std::endl;

        figIter += 18;
    }
}

bool Fig_01_Ext_06::isProgrammeService() const {
    return m_isProgramme;
}

uint32_t Fig_01_Ext_06::getServiceId() const {
    return m_serviceId;
}

uint8_t Fig_01_Ext_06::getServiceComponentIdWithinService() const {
    return m_scIdS;
}

uint8_t Fig_01_Ext_06::getXPadApplicationType() const {
    return m_xpadAppType;
}

std::string Fig_01_Ext_06::getXPadUserAppLabel() const {
    return m_uappLabel;
}

std::string Fig_01_Ext_06::getXPadUserAppShortLabel() const {
    return m_uappShortLabel;
}
