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

#include "fig_00_ext_19.h"

#include <iostream>

Fig_00_Ext_19::Fig_00_Ext_19(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_19::~Fig_00_Ext_19() {

}

void Fig_00_Ext_19::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        AnnouncementSwitching aSwitch;

        aSwitch.clusterId = static_cast<uint8_t>(*figIter++ & 0xFF);
        uint16_t aswFlags = static_cast<uint16_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));

        for(int i = 0; i <= 15; i++) {
            bool supported = ((((aswFlags << i) >> 15) & 0x01)) != 0;
            if(supported) {
                aSwitch.announcementsSwitched.push_back(static_cast<registeredtables::ANNOUNCEMENT_TYPE>(15-i));
            }
        }

        aSwitch.isNewlyIntroduced = (((*figIter & 0x80) >> 7) & 0xFF) != 0;
        aSwitch.subchannelId = static_cast<uint8_t>((*figIter++ & 0x3F) & 0xFF);

        m_switchedAnnouncements.push_back(aSwitch);
    }
}
