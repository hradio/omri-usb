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

#include "paddecoder.h"

#include <iostream>

#include "global_definitions.h"

constexpr uint8_t PadDecoder::XPAD_SIZE[];

PadDecoder::PadDecoder() {

}

PadDecoder::~PadDecoder() {

}

void PadDecoder::padDataInput(const std::vector<uint8_t>& padData) {
    auto padRiter = padData.rbegin();
    while (padRiter < padData.rend()) {
        //F-PAD
        bool ciPresent = (*padRiter & 0x02) >> 1 != 0;
        bool z = (*padRiter & 0x01) != 0;
        if(z) {
            std::cout << m_logTag << " Z-Field: " << std::boolalpha << z << std::noboolalpha << std::endl;
        }
        padRiter++; //Byte L data field

        uint8_t fPadType = static_cast<uint8_t>((*padRiter & 0xC0) >> 6); //Byte L-1 data field

        X_PAD_INDICATOR xPadIndicator = static_cast<X_PAD_INDICATOR>((*padRiter++ & 0x30) >> 4);
        //std::cout << m_logTag << " FPadType: " << +fPadType << " XPadInd: " << +xPadIndicator << " CI: " << std::boolalpha << ciPresent << " Z: " << z << std::noboolalpha << std::endl;

        if(fPadType == 0) {
            std::vector<std::pair<uint8_t, PAD_APPLICATION_TYPE>> apps;

            if(xPadIndicator == X_PAD_INDICATOR::NO_XPAD) {
                //NO Xpad
            }
            //short xpad
            if(xPadIndicator == X_PAD_INDICATOR::SHORT_XPAD) {
                std::vector<uint8_t> xpadData;
                int xpadSize = 4;
                uint8_t xpadAppType = 0;
                if(ciPresent) {
                    //uint8_t rfu = (*padRiter & 0xE0) >> 5;
                    m_noCiLastShortXpAdAppType = xpadAppType = static_cast<uint8_t>(*padRiter++ & 0x1f);
                    xpadSize = 3;
                } else {
                    xpadAppType = static_cast<uint8_t>(m_noCiLastShortXpAdAppType+1);
                }

                apps.push_back(std::make_pair(xpadSize, static_cast<PAD_APPLICATION_TYPE>(xpadAppType)));
            }
            //variable xpad
            if(xPadIndicator == X_PAD_INDICATOR::VARIABLE_XPAD) {
                //std::vector<std::pair<uint8_t, PAD_APPLICATION_TYPE>> apps;
                if(ciPresent) {
                    //std::cout << m_logTag << " XPad CI present" << std::endl;
                    for(int i = 0; i < 4; i++) {
                        uint8_t xpadLengthIndex = static_cast<uint8_t>((*padRiter & 0xE0) >> 5);
                        uint8_t xpadAppType = static_cast<uint8_t>(*padRiter++ & 0x1f);

                        if( (xpadAppType >= 4 && xpadAppType <= 11) || (xpadAppType >= 16 && xpadAppType <= 31) ) {
                            //std::cout << m_logTag << "[" << +i << "] XPadLength: " << +xpadLengthIndex << " : " << +XPAD_SIZE[xpadLengthIndex] << " AppType: " << +xpadAppType << std::endl;
                            auto uAppIter = m_userApps.find(xpadAppType);
                            if(uAppIter != m_userApps.cend()) {
                                std::cout << m_logTag << " Found Userapp for XPADApptype: " << +xpadAppType << " : " << +uAppIter->second->getXpadAppType() << " UAppType: " << +uAppIter->second->getUserApplicationType() << " DGUsed: " << std::boolalpha << uAppIter->second->dataGroupsUsed() << std::noboolalpha << " DSCTy: " << uAppIter->second->getDataServiceComponentType() << std::endl;
                            }
                        }
                        if(xpadAppType == 0) {
                            break;
                        }

                        m_noCiLastLength = PadDecoder::XPAD_SIZE[xpadLengthIndex];
                        m_noCiLastXpAdAppType = xpadAppType;

                        apps.push_back(std::make_pair(XPAD_SIZE[xpadLengthIndex], static_cast<PAD_APPLICATION_TYPE>(xpadAppType)));
                    }
                } else {
                    if(m_noCiLastLength > 0 || m_noCiLastXpAdAppType != 0xFF) {
                        //if the last xpadapptype was not a continuation
                        if(!(m_noCiLastXpAdAppType%2)) {
                            ++m_noCiLastXpAdAppType;
                        }
                        apps.push_back(std::make_pair(padData.size()-2, static_cast<PAD_APPLICATION_TYPE>(m_noCiLastXpAdAppType)));
                    }
                }
            }

            for(const std::pair<uint8_t, PAD_APPLICATION_TYPE>& app : apps) {
                std::vector<uint8_t> xpadDataSubfield(padRiter, padRiter+app.first);
                padRiter += app.first;

                if(app.second == PAD_APPLICATION_TYPE::DATA_GROUP_LENGTH_INDICATOR) {
                    if(CRC_CCITT_CHECK(xpadDataSubfield.data(), static_cast<uint8_t>(xpadDataSubfield.size()))) {
                        if(!m_currentDataGroup.empty()) {
                            //last datagroup may be complete
                            //std::cout << m_logTag << " MOT Datagroup complete, size: " << +m_currentDataGroup.size() << " : " << +m_currentDataGroupLength << std::endl;
                            if(m_currentDataGroup.size() != m_currentDataGroupLength) {
                                std::cout << m_logTag << " MOT Datagroup size mismatch, should be: " << +m_currentDataGroupLength << " and is: " << m_currentDataGroup.size() << std::endl;
                            }

                            m_motDecoder.motDataInput(m_currentDataGroup);
                        }

                        m_currentDataGroup.clear();
                        m_currentDataGroupLength = static_cast<uint16_t>((xpadDataSubfield[0] & 0x3F) << 8 | (xpadDataSubfield[1] & 0xFF));
                    } else {
                        std::cout << m_logTag << " ############### MOT DG DATA_GROUP_LENGTH_INDICATOR CRC failed ###############" << std::endl;
                        m_currentDataGroup.clear();
                        m_currentDataGroupLength = 0;
                    }
                }

                if(app.second == PAD_APPLICATION_TYPE::MOT_DATAGROUP_START || app.second == PAD_APPLICATION_TYPE::MOT_DATAGROUP_CONTINUATION) {
                    if(app.second == PAD_APPLICATION_TYPE::MOT_DATAGROUP_CONTINUATION) {
                        //std::cout << m_logTag << " CurSize: " << +m_currentDataGroup.size() << ", GoalSize: " << +m_currentDataGroupLength << ", NextSize: " << +std::distance(xpadDataSubfield.begin(), xpadDataSubfield.end()) << std::endl;
                        long dataSize = std::distance(xpadDataSubfield.begin(), xpadDataSubfield.end());
                        long remainingData = m_currentDataGroupLength - m_currentDataGroup.size();
                        if(dataSize <= remainingData) {
                            m_currentDataGroup.insert(m_currentDataGroup.end(), xpadDataSubfield.begin(), xpadDataSubfield.end());
                        } else {
                            //std::cout << m_logTag << " Too much data: " << +dataSize << " : " << +remainingData << std::endl;
                            m_currentDataGroup.insert(m_currentDataGroup.end(), xpadDataSubfield.begin(), xpadDataSubfield.begin() + remainingData);
                        }
                    }
                    if(app.second == PAD_APPLICATION_TYPE::MOT_DATAGROUP_START) {
                        m_currentDataGroup.insert(m_currentDataGroup.end(), xpadDataSubfield.begin(), xpadDataSubfield.end());
                    }
                }

                if(app.second == PAD_APPLICATION_TYPE::DLS_DATAGROUP_START || app.second == PAD_APPLICATION_TYPE::DLS_DATAGROUP_CONTINUATION) {
                    auto uappIter = m_userAppDecoders.find(registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL);
                    if(uappIter != m_userAppDecoders.cend()) {
                        uappIter->second->applicationDataInput(xpadDataSubfield, app.second);
                    }
                }
            }
        }

        break;
    }
}

void PadDecoder::addUserApplication(std::shared_ptr<DabUserApplication> uApp) {
    std::cout << m_logTag << " +++++++++++ Adding UserApp: " << +uApp->getUserApplicationType() << std::endl;
    m_userApps.insert(std::make_pair(uApp->getXpadAppType(), uApp));

}

void PadDecoder::addUserApplicationDecoder(std::shared_ptr<DabUserapplicationDecoder> uAppDecoder) {
    if(uAppDecoder != nullptr) {
        std::cout << m_logTag << " +++++++++++ Adding UserAppDecoder: " << +uAppDecoder->getUserApplicationType() << std::endl;
        m_userAppDecoders.insert(std::make_pair(uAppDecoder->getUserApplicationType(), uAppDecoder));
        if(uAppDecoder->getUserApplicationType() == registeredtables::USERAPPLICATIONTYPE::MOT_SLIDESHOW || uAppDecoder->getUserApplicationType() == registeredtables::USERAPPLICATIONTYPE::MOT_BROADCAST_WEBSITE || uAppDecoder->getUserApplicationType() == registeredtables::USERAPPLICATIONTYPE::EPG) {
            std::cout << m_logTag << " +++++++++++ Adding MotUserAppDecoder: " << +uAppDecoder->getUserApplicationType() << std::endl;
            m_motDecoder.addUserapplicationDecoder(std::static_pointer_cast<DabMotUserapplicationDecoder>(uAppDecoder));
        }
    } else {
        std::cout << m_logTag << " +++++++++++ UserAppDecoder is null!" << std::endl;
    }
}

void PadDecoder::reset() {
    auto uAppIter = m_userAppDecoders.cbegin();
    while(uAppIter != m_userAppDecoders.cend()) {
        uAppIter->second->reset();
        ++uAppIter;
    }
}