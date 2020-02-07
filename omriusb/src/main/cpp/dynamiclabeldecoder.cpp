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

#include "dynamiclabeldecoder.h"

#include <iostream>
#include <iomanip>

#include "global_definitions.h"
#include "datatypes.h"

DynamiclabelDecoder::DynamiclabelDecoder() {

}

DynamiclabelDecoder::~DynamiclabelDecoder() {

}

registeredtables::USERAPPLICATIONTYPE DynamiclabelDecoder::getUserApplicationType() const {
    return registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL;
}

void DynamiclabelDecoder::reset() {
    std::cout << m_logTag << " Reseting..." << std::endl;
    m_dlsData.clear();
    m_dlsFullData.clear();
    m_dlsFullSegNum = 0;
    m_currentDlsCharset = 0xFF;
}

void DynamiclabelDecoder::applicationDataInput(const std::vector<uint8_t>& appData, uint8_t dataType) {
    if(dataType == DLS_DATAGROUP_START) {
        //Last DLS segment complete
        if(m_dlsData.size() >= 4) {
            auto dlIter = m_dlsData.begin();

            bool toggleBit = (*dlIter & 0x80) >> 7 != 0;
            uint8_t firstLast = static_cast<uint8_t>((*dlIter & 0x60) >> 5);
            auto segType = static_cast<SEGMENT_TYPE>(firstLast);
            bool commandFlag = static_cast<uint8_t>((*dlIter & 0x10) >> 4);

            if(!commandFlag) {
                uint8_t length = static_cast<uint8_t>((*dlIter++ & 0x0F) + 1);

                if(!CRC_CCITT_CHECK(m_dlsData.data(), static_cast<uint16_t>(length+4))) {
                    std::cout << m_logTag << " DLS CharData CRC failed" << std::endl;
                    m_dlsData.clear();
                    return;
                }

                if(segType == SEGMENT_TYPE::FIRST) {
                    m_currentDlsCharset = static_cast<uint8_t>((*dlIter & 0xF0) >> 4);
                    uint8_t rfa = static_cast<uint8_t>((*dlIter++ & 0x0F));

                    m_dlsFullData.clear();
                    m_dlsFullSegNum = 0;

                    m_dlsFullData.insert(m_dlsFullData.end(), dlIter, dlIter+length);

                    //std::cout << m_logTag  << " SEGMENT_TYPE::FIRS: " << std::string(m_dlsFullData.begin(), m_dlsFullData.end()) << std::endl;
                }

                if(segType == SEGMENT_TYPE::INTERMEDIATE) {
                    bool rfa = static_cast<bool>((*dlIter & 0x80) >> 7);
                    uint8_t segNum = static_cast<uint8_t >((*dlIter++ & 0x70) >> 4);

                    if(!m_dlsFullData.empty()) {
                        if(segNum != m_dlsFullSegNum+1) {
                            std::cout << m_logTag << "DLS intermediate SegNum discontinuation: " << +segNum << " : " << +m_dlsFullSegNum << std::endl;
                            m_dlsFullData.clear();
                            m_dlsFullSegNum = 0;
                            return;
                        }

                        m_dlsFullSegNum = segNum;

                        m_dlsFullData.insert(m_dlsFullData.end(), dlIter, dlIter+length);

                        //std::cout << m_logTag  << " SEGMENT_TYPE::INTER: " << std::string(m_dlsFullData.begin(), m_dlsFullData.end()) << std::endl;
                    }
                }

                if(segType == SEGMENT_TYPE::LAST) {
                    bool rfa = static_cast<bool>((*dlIter & 0x80) >> 7);
                    uint8_t segNum = static_cast<uint8_t>((*dlIter++ & 0x70) >> 4);

                    if(!m_dlsFullData.empty()) {
                        if(segNum != m_dlsFullSegNum+1) {
                            std::cout << m_logTag << "DLS last SegNum discontinuation: " << +segNum << " : " << +m_dlsFullSegNum << std::endl;
                            m_dlsFullData.clear();
                            m_dlsFullSegNum = 0;
                            return;
                        }

                        m_dlsFullData.insert(m_dlsFullData.end(), dlIter, dlIter+length);

                        //std::cout << m_logTag  << " SEGMENT_TYPE::LAST: " << std::string(m_dlsFullData.begin(), m_dlsFullData.end()) << std::endl;

                        if(!m_isDynamicPlus) {
                            DabDynamicLabel label;
                            label.dynamicLabel = std::string(m_dlsFullData.begin(), m_dlsFullData.end());
                            label.charset = m_currentDlsCharset;
                            m_userappDataDispatcher.invoke(std::make_shared<DabDynamicLabel>(label));
                        }

                        m_dlsData.clear();
                    }
                }

                if(segType == SEGMENT_TYPE::ONE_AND_ONLY) {
                    //std::cout << m_logTag  << " SEGMENT_TYPE::ONE_AND_ONLY" << std::endl;

                    DabDynamicLabel label;
                    m_currentDlsCharset = label.charset = static_cast<uint8_t>((*dlIter & 0xF0) >> 4);
                    uint8_t rfa = static_cast<uint8_t>((*dlIter++ & 0x0F));

                    m_dlsFullData.clear();
                    m_dlsFullData.insert(m_dlsFullData.end(), dlIter, dlIter+length);

                    label.dynamicLabel = std::string(dlIter, dlIter+length);

                    if(!m_isDynamicPlus) {
                        m_userappDataDispatcher.invoke(std::make_shared<DabDynamicLabel>(label));
                    }

                    m_dlsFullSegNum = 0;
                    m_dlsData.clear();
                }
            } else {
                //std::cout << m_logTag << " CommandFlag: " << +(*dlIter & 0x0F) << ", Size: " << +m_dlsData.size() << std::endl;

                REMOVE_PADDING(m_dlsData);
                if(!CRC_CCITT_CHECK(m_dlsData.data(), static_cast<uint16_t>(m_dlsData.size()))) {
                    //std::cout << m_logTag << " DLS CommandData CRC failed: " << m_dlsData.size() << std::endl;
                    m_dlsData.clear();
                    return;
                }

                DLS_COMMAND command = static_cast<DLS_COMMAND>(*dlIter++ & 0x0F);
                switch (command) {
                    case DLS_COMMAND::CLEAR_DISPLAY: {
                        //std::cout << m_logTag << " DLS_DATAGROUP COMMAND Clear Display " << std::endl;
                        break;
                    }
                    case DLS_COMMAND::DL_PLUS_COMMAND: {
                        m_isDynamicPlus = true;

                        //Field 2
                        //First flag = 1
                        bool linkBit;
                        if(segType == SEGMENT_TYPE::FIRST || segType == SEGMENT_TYPE::ONE_AND_ONLY) {
                            linkBit = (*dlIter & 0x80) >> 7 != 0;
                        }
                        //First flag = 0
                        if(segType == SEGMENT_TYPE::INTERMEDIATE || segType == SEGMENT_TYPE::LAST) {
                            linkBit = (*dlIter & 0x80) >> 7 != 0;
                            uint8_t segNum = static_cast<uint8_t>((*dlIter & 0x70) >> 4);
                            //std::cout << m_logTag << " DLPLUS SegNum: " << + segNum << std::endl;
                        }

                        //Field 3
                        uint8_t dlCommandLength = static_cast<uint8_t>((*dlIter++ & 0x0F));

                        uint8_t cId = static_cast<uint8_t>((*dlIter & 0xF0) >> 4);
                        //uint8_t cb = (*dlIter++ & 0x0F);

                        if(cId == 0x00) {
                            if(!m_dlsFullData.empty()) {
                                DabDynamicLabel label;
                                label.dynamicLabel = std::string(m_dlsFullData.begin(), m_dlsFullData.end());
                                label.charset = m_currentDlsCharset;

                                label.itemToggle = (*dlIter & 0x08) >> 3 != 0;
                                label.itemRunning = (*dlIter & 0x04) >> 2 != 0;

                                //Field 3: this 4-bit field, expressed as an unsigned binary number, shall specify the number of bytes in the DL Command field minus 1.
                                uint8_t numTags = static_cast<uint8_t>((*dlIter++ & 0x03) + 1);
                                //std::cout << m_logTag << " DLPLUS NumTags: " << +numTags << std::endl;
                                for(uint8_t i = 0; i < numTags; i++) {
                                    DabDynamicLabelPlusTag dlItem;

                                    //bool rfa = (*dlIter & 0x80) >> 7;
                                    dlItem.contentType = static_cast<DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE>(*dlIter++ & 0x7F);
                                    //bool rfa = (*dlIter & 0x80) >> 7;
                                    uint8_t startMarker = static_cast<uint8_t>(*dlIter++ & 0x7F);
                                    //bool rfa = (*dlIter & 0x80) >> 7;
                                    uint8_t lengthMarker = static_cast<uint8_t>(*dlIter++ & 0x7F);

                                    if(dlItem.contentType != DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE::DUMMY) {
                                        dlItem.dlPlusTagText = std::string(m_dlsFullData.begin()+startMarker, m_dlsFullData.begin()+startMarker+lengthMarker+1);

                                        //std::cout << m_logTag << " DLPLUS ContentType: " << +dlItem.contentType << " Start: " << +startMarker << " Length: " << +lengthMarker << std::endl;
                                        //std::cout << m_logTag << " DLPLUS: " << DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE_STRING[dlItem.contentType] << " : " << dlItem.dlPlusTagText << std::endl;
                                    }

                                    label.dlPlusTags.push_back(dlItem);
                                    //std::cout << m_logTag << " DLPLUS Items: " << +label.dlPlusTags.size() << std::endl;
                                }

                                m_userappDataDispatcher.invoke(std::make_shared<DabDynamicLabel>(label));
                            }
                        }

                        break;
                    }
                    default:
                        break;
                }
            }
        }

        //First received DLS data. Clear previous
        m_dlsData.clear();
        m_dlsData.insert(m_dlsData.end(), appData.begin(), appData.end());
    }

    if(dataType == DLS_DATAGROUP_CONTINUATION) {
        //std::cout << m_logTag << " DLS_DATAGROUP_CONTINUATION Empty: " << std::boolalpha << m_dlsData.empty() << std::noboolalpha << " " << std::string(xpadDataSubfield.begin(), xpadDataSubfield.end()) << std::endl;
        if(!m_dlsData.empty()) {
            m_dlsData.insert(m_dlsData.end(), appData.begin(), appData.end());
        }
    }
}

std::shared_ptr<DabUserapplicationDecoder::UserapplicationDataCallback> DynamiclabelDecoder::registerUserapplicationDataCallback(DabUserapplicationDecoder::UserapplicationDataCallback appSpecificDataCallback) {
    return m_userappDataDispatcher.add(appSpecificDataCallback);
}

const std::string DynamiclabelDecoder::DL_PLUS_CONTENT_TYPE_STRING[] {
    "DUMMY",
    "ITEM_TITLE",
    "ITEM_ALBUM",
    "ITEM_TRACKNUMBER",
    "ITEM_ARTIST",
    "ITEM_COMPOSITION",
    "ITEM_MOVEMENT",
    "ITEM_CONDUCTOR",
    "ITEM_COMPOSER",
    "ITEM_BAND",
    "ITEM_COMMENT",
    "ITEM_GENRE",
    "INFO_NEWS",
    "INFO_NEWS_LOCAL",
    "INFO_STOCKMARKET",
    "INFO_SPORT",
    "INFO_LOTTERY",
    "INFO_HOROSCOPE",
    "INFO_DAILY_DIVERSION",
    "INFO_HEALTH",
    "INFO_EVENT",
    "INFO_SCENE",
    "INFO_CINEMA",
    "INFO_TV",
    "INFO_DATE_TIME",
    "INFO_WEATHER",
    "INFO_TRAFFIC",
    "INFO_ALARM",
    "INFO_ADVERTISEMENT",
    "INFO_URL",
    "INFO_OTHER",
    "STATIONNAME_SHORT",
    "STATIONNAME_LONG",
    "PROGRAMME_NOW",
    "PROGRAMME_NEXT",
    "PROGRAMME_PART",
    "PROGRAMME_HOST",
    "PROGRAMME_EDITORIAL_STAFF",
    "PROGRAMME_FREQUENCY",
    "PROGRAMME_HOMEPAGE",
    "PROGRAMME_SUBCHANNEL",
    "PHONE_HOTLINE",
    "PHONE_STUDIO",
    "PHONE_OTHER",
    "SMS_STUDIO",
    "SMS_OTHER",
    "EMAIL_HOTLINE",
    "EMAIL_STUDIO",
    "EMAIL_OTHER",
    "MMS_OTHER",
    "CHAT",
    "CHAT_CENTER",
    "VOTE_QUESTION",
    "VOTE_CENTRE",

    "RFU_1",
    "RFU_2",

    "PRIVATE_CLASS_1",
    "PRIVATE_CLASS_2",
    "PRIVATE_CLASS_3",

    "DESCRIPTOR_PLACE",
    "DESCRIPTOR_APPOINTMENT",
    "DESCRIPTOR_IDENTIFIER",
    "DESCRIPTOR_PURCHASE",
    "DESCRIPTOR_GET_DATA"
};
