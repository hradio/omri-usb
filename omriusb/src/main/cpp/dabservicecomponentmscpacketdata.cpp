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

#include "dabservicecomponentmscpacketdata.h"

#include <iostream>

#include "global_definitions.h"

#include "mscdatagroupdecoder.h"

constexpr uint8_t DabServiceComponentMscPacketData::PACKETLENGTH[4][2];

DabServiceComponentMscPacketData::DabServiceComponentMscPacketData() {
    m_componentType = DabServiceComponent::SERVICECOMPONENTTYPE::MSC_PACKET_MODE_DATA;
}

DabServiceComponentMscPacketData::~DabServiceComponentMscPacketData() {

}

uint16_t DabServiceComponentMscPacketData::getDataServiceComponentId() const {
    return m_serviceComponentId;
}

uint8_t DabServiceComponentMscPacketData::getDataServiceComponentType() const {
    return m_dscty;
}

uint16_t DabServiceComponentMscPacketData::getPacketAddress() const {
    return m_packetAddress;
}

uint16_t DabServiceComponentMscPacketData::getCaOrganization() const {
    return m_caOrg;
}

bool DabServiceComponentMscPacketData::isDataGroupTransportUsed() const {
    return m_dataGroupsUsed;
}

void DabServiceComponentMscPacketData::setDataServiceComponentId(uint16_t scid) {
    m_serviceComponentId = scid;
}

void DabServiceComponentMscPacketData::setPacketAddress(uint16_t packAddr) {
    m_packetAddress = packAddr;
}

void DabServiceComponentMscPacketData::setCaOrganization(uint16_t caOrg) {
    m_caOrg = caOrg;
}

void DabServiceComponentMscPacketData::setIsDataGroupTransportUsed(bool dataGroupsUsed) {
    m_dataGroupsUsed = dataGroupsUsed;
}

void DabServiceComponentMscPacketData::setDataServiceComponentType(uint8_t dscty) {
    m_dscty = dscty;
}

void DabServiceComponentMscPacketData::flushBufferedData() {
}

void DabServiceComponentMscPacketData::componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) {
    if(true) {
        return;
    }
    std::cout << m_logTag << " #### PacketData DatagroupsUsed: " << std::boolalpha << m_dataGroupsUsed << " Synchronized: " << synchronized << std::noboolalpha << " DSCTy: " << +m_dscty << " DataSize: " << +mscData.size() << std::endl;

    //if(m_subChanId != 0x0e || m_scIdS != 0) {
    //    return;
    //}

    //TODO synchronize data, cache overlap etc....partly done....thread this for processing
    if(!synchronized) {
        std::vector<uint8_t> data;
        if(!m_unsyncDataBuffer.empty()) {
            data.insert(data.begin(), m_unsyncDataBuffer.begin(), m_unsyncDataBuffer.end());
            m_unsyncDataBuffer.clear();
        }

        data.insert(data.end(), mscData.begin(), mscData.end());

        auto packIter = data.begin();
        while(packIter < data.end()) {
            uint8_t packetLength = (*packIter & 0xC0) >> 6;
            if((std::distance(data.begin(), packIter) + PACKETLENGTH[packetLength][0]) >= data.size()) {
                //std::cout << " Breaking out for running out of data at pos: " << +std::distance(data.begin(), packIter) << " : " << +PACKETLENGTH[packetLength][0] << " : " << +data.size() << std::endl;
                break;
            }
            uint8_t continuityIndex = (*packIter & 0x30) >> 4;
            std::vector<uint8_t> checkDat(packIter, packIter + PACKETLENGTH[packetLength][0]);
            if(CRC_CCITT_CHECK(checkDat.data(), checkDat.size())) {
                std::cout << " Found sync at: " << +std::distance(data.begin(), packIter) << " ContIdx: " << +continuityIndex << " PacketLength: " << +PACKETLENGTH[packetLength][0] << std::endl;
                packIter += PACKETLENGTH[packetLength][0];
                continue;
            }
            packIter++;
        }

        //std::cout << " Caching " << +(data.size() - std::distance(data.begin(), packIter)) << " bytes from pos: " << +std::distance(data.begin(), packIter) << std::endl;
        m_unsyncDataBuffer.insert(m_unsyncDataBuffer.begin(), packIter, data.end());
    }

    return;

    if(CRC_CCITT_CHECK(mscData.data(), mscData.size())) {

        auto packetIter = mscData.begin();
        uint8_t packetLength = (*packetIter & 0xC0) >> 6;
        uint8_t continuityIndex = (*packetIter & 0x30) >> 4;
        DabServiceComponentMscPacketData::FIRST_LAST_PACKET firstLast = static_cast<DabServiceComponentMscPacketData::FIRST_LAST_PACKET>((*packetIter & 0x0C) >> 2);
        uint16_t packetAddress = (*packetIter++ & 0x03) << 8 | (*packetIter++ & 0xFF);
        bool commandFlag = (*packetIter & 0x80) >> 7;
        uint8_t usefulDataLength = (*packetIter++ & 0x7F);

        if(PACKETLENGTH[packetLength][0] != mscData.size()) {
            return;
        }

        if(m_packetAddress != packetAddress) {
            //May be possible that more than one packetaddresses are transmitted in the same subchannel
            return;
        }

        if(m_dataGroupsUsed) {
            switch (firstLast) {
                case FIRST_LAST_PACKET::FIRST: {
                    //std::cout << m_logTag << " #### Packet First " << std::endl;
                    m_mscPacket.packetData.clear();
                    m_mscPacket.packetData.insert(m_mscPacket.packetData.end(), packetIter, packetIter+usefulDataLength);
                    m_mscPacket.packetAddress = packetAddress;
                    m_mscPacket.continuityIndex = continuityIndex;

                    break;
                }
                case FIRST_LAST_PACKET::INTERMEDIATE: {
                    if(!m_mscPacket.packetData.empty()) {
                        if( ((m_mscPacket.continuityIndex+1) % 4) != continuityIndex) {
                            //std::cout << m_logTag << " Continuity at intermediate packet interrupted: " << +m_mscPacket.continuityIndex << " : " << +continuityIndex << std::endl;

                            //m_mscPacket.packetData.clear();
                            return;
                        }

                        //std::cout << m_logTag << " #### Packet Continuation " << std::endl;
                        m_mscPacket.continuityIndex = continuityIndex;
                        m_mscPacket.packetData.insert(m_mscPacket.packetData.end(), packetIter, packetIter+usefulDataLength);
                    }
                    break;
                }
                case FIRST_LAST_PACKET::LAST: {
                    if(!m_mscPacket.packetData.empty()) {
                        if( ((m_mscPacket.continuityIndex+1) % 4) != continuityIndex) {
                            //std::cout << m_logTag << " Continuity at last packet interrupted: " << +m_mscPacket.continuityIndex << " : " << +continuityIndex << std::endl;

                            //m_mscPacket.packetData.clear();
                            return;
                        }

                        m_mscPacket.continuityIndex = continuityIndex;
                        m_mscPacket.packetData.insert(m_mscPacket.packetData.end(), packetIter, packetIter+usefulDataLength);

                        //std::cout << m_logTag << " #### Packet complete: " << m_mscPacket.packetData.size() << std::endl;

                        if(CRC_CCITT_CHECK(m_mscPacket.packetData.data(), m_mscPacket.packetData.size())) {
                            //std::cout << m_logTag << " #### Packet CRC okay!" << std::endl;

                            enum MOT_DATAGROUP_TYPE {
                                GENERAL_DATA,
                                CA_MESSAGE,
                                MOT_HEADER,
                                MOT_BODY_UNSCRAMBLED,
                                MOT_BODY_SCRAMBLED,
                                MOT_DIRECTORY_UNCOMPRESSED,
                                MOT_DIRECTORY_COMPRESSED,
                            };

                            //MSC Datagroup decoding
                            auto dgIter = m_mscPacket.packetData.begin();
                            //MSC Datagroup Header
                            bool extensionFlag = (*dgIter & 0x80) >> 7;
                            bool crcFlag = (*dgIter & 0x40) >> 6;
                            bool segmentFlag = (*dgIter & 0x20) >> 5;
                            bool userAccessFlag = (*dgIter & 0x10) >> 4;
                            uint8_t datagroupType = (*dgIter++ & 0x0F);

                            uint8_t continuityIdx = (*dgIter & 0xF0) >> 4;
                            uint8_t repetitionIdx = (*dgIter++ & 0x0F);
                            if(extensionFlag) {
                                dgIter += 2;
                            }
                            //std::cout << m_logTag << " MOT DG ExtFlg: " << std::boolalpha << extensionFlag << " CRCFlg: " << crcFlag << " SegFlg: " << segmentFlag << std::noboolalpha << std::endl;
                            //std::cout << m_logTag << " MOT DG Type: " << +datagroupType << " Cont: " << +continuityIdx << " Rep: " << +repetitionIdx << std::endl;

                            //MSC Datagroup Session header
                            if(segmentFlag) {
                                bool isLast = (*dgIter & 0x80) >> 7;
                                uint16_t segNum = (*dgIter++ & 0x7F) << 8 | (*dgIter++ & 0xFF);

                                //std::cout << m_logTag << " MOT DG isLast: " << std::boolalpha << isLast << std::noboolalpha << " SegNum: " << +segNum << std::endl;
                                //if(isLast) {
                                //    std::cout << m_logTag << " MOT DG isLast ##############################" << std::endl;
                                //}
                            }

                            if(userAccessFlag) {
                                uint8_t rfa = (*dgIter & 0xE0) >> 6;
                                bool transportIdFlag = (*dgIter & 0x10) >> 4;
                                uint8_t lengthIndicator = (*dgIter++ & 0x0F);
                                //std::cout << m_logTag << " MOT DG TransportIdFlg: " << std::boolalpha << transportIdFlag << std::noboolalpha << " Length: " << +lengthIndicator << std::endl;
                                if(transportIdFlag) {
                                    uint16_t transportId = (*dgIter++ & 0xFF) << 8 | (*dgIter++ & 0xFF);
                                    //std::cout << m_logTag << " MOT DG TransportId: " << std::hex << transportId << std::dec << std::endl;
                                }
                                std::vector<uint8_t> endUserAddress;
                                for(uint8_t i = 0; i < (lengthIndicator-2); i++) {
                                    //std::cout << m_logTag << " ############## MOT DG EnduserAddress: " << std::hex << *dgIter << std::dec << std::endl;
                                    endUserAddress.push_back(*dgIter++);
                                }
                            }
                        } else {
                            //std::cout << m_logTag << " #### Packet CRC failed!" << std::endl;
                        }
                    }
                    break;
                }
                case FIRST_LAST_PACKET::ONE_AND_ONLY: {

                    break;
                }
            default:
                break;
            }
        }
        //std::cout << m_logTag << " ############################ MSC Packetdata DGUsed: " << std::boolalpha << m_dataGroupsUsed << std::noboolalpha << " Length: " << +mscData.size() << " : " << +DabServiceComponentMscPacketData::PACKETLENGTH[packetLength][0] << " - " << +usefulDataLength << " FirstLast: " << +firstLast << " for SubchanId: " << std::hex << +m_subChanId << std::dec <<  " Cont: " << +continuityIndex << " Address: " << +m_packetAddress << " : " << +packetAddress << " UApps: " << +m_userApplications.size() << std::endl;
    }
}
