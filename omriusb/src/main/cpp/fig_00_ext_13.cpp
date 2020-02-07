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

#include "fig_00_ext_13.h"

Fig_00_Ext_13::Fig_00_Ext_13(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_13::~Fig_00_Ext_13() {

}

void Fig_00_Ext_13::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        UserApplicationInformation userAppInfo;

        userAppInfo.isProgrammeService = !isDataService();
        if(userAppInfo.isProgrammeService) {
            userAppInfo.serviceID = static_cast<uint32_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        } else {
            userAppInfo.serviceID = static_cast<uint32_t>((*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
        }

        userAppInfo.scIdS = static_cast<uint8_t>((*figIter & 0xF0) >> 4);
        userAppInfo.numUserApps = static_cast<uint8_t>((*figIter++ & 0x0F) & 0xFF);

        for(uint8_t i = 0; i < userAppInfo.numUserApps; i++) {
            UserApplication userApp;

            userApp.userAppType = static_cast<registeredtables::USERAPPLICATIONTYPE>((*figIter++ & 0xFF) << 3 | (*figIter & 0xE0) >> 5);
            uint8_t uAppDataLength = static_cast<uint8_t>((*figIter++ & 0x1F) & 0xFF);
            userApp.isXpadData = !isDataService();

            //X-PAD data: this field is only present for applications carried in the X-PAD of an audio service component
            if(userApp.isXpadData) {
                if(uAppDataLength >= 2) {
                    userApp.caApplied = ((*figIter & 0x80) >> 7) != 0;
                    bool caorgFlag = ((*figIter & 0x40) >> 6) != 0;
                    //bool rfu1 = (*figIter & 0x20) >> 5;
                    userApp.xpadAppType = static_cast<uint8_t>((*figIter++ & 0x1F) & 0xFF);
                    userApp.dataGroupsUsed = !((*figIter & 0x80) >> 7);
                    //bool rfu2 = (*figIter & 0x40) >> 6;
                    userApp.dataServiceComponentType = static_cast<registeredtables::DATA_SERVICE_COMPONENT_TYPE>((*figIter++ & 0x3F) & 0xFF);
                    if(caorgFlag) {
                        userApp.caOrganization = static_cast<uint16_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));
                        uAppDataLength -= 2;
                    }

                    uAppDataLength -= 2;
                }
            }

            /*
             * User Application data: these 8-bit fields may be used to signal application specific information.
             * The interpretation of these fields is determined by the user application identified by the User Application Type
             */
            for(uint8_t i = 0; i < uAppDataLength; i++) {
                userApp.userAppData.push_back(static_cast<uint8_t>(*figIter++ & 0xFF));
            }

            userAppInfo.userApplications.push_back(userApp);
        }

        m_userAppDescriptions.push_back(userAppInfo);
    }
}

const std::vector<Fig_00_Ext_13::UserApplicationInformation>& Fig_00_Ext_13::getUserApplicationInformations() const {
    return m_userAppDescriptions;
}
