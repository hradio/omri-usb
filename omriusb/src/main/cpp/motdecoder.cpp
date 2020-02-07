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

#include "motdecoder.h"

#include <iostream>

MotDecoder::MotDecoder() {

}

MotDecoder::~MotDecoder() {

}

void MotDecoder::motDataInput(const std::vector<uint8_t>& mscDatagroup) {

    //MSC Datagroup decoding
    auto dgIter = mscDatagroup.begin();
    //MSC Datagroup Header
    bool extensionFlag = ((*dgIter & 0x80) >> 7) != 0;
    bool crcFlag = ((*dgIter & 0x40) >> 6) != 0;

    if(crcFlag) {
        if(!CRC_CCITT_CHECK(mscDatagroup.data(), mscDatagroup.size())) {
            std::cout << m_logTag << "MSC Datagroup CRC failed!" << std::endl;
            return;
        }
    }

    bool segmentFlag = ((*dgIter & 0x20) >> 5) != 0;
    bool userAccessFlag = ((*dgIter & 0x10) >> 4) != 0;
    MOT_DATAGROUP_TYPE datagroupType = static_cast<MOT_DATAGROUP_TYPE >((*dgIter++ & 0x0F));

    uint8_t continuityIdx = static_cast<uint8_t>((*dgIter & 0xF0) >> 4);
    uint8_t repetitionIdx = static_cast<uint8_t>((*dgIter++ & 0x0F));

    if(extensionFlag) {
        dgIter += 2;
    }

    //MSC Datagroup Session header
    bool isLast = false;
    uint16_t segNum{0xFFFF};
    if(segmentFlag) {
        isLast = ((*dgIter & 0x80) >> 7) != 0;
        segNum = static_cast<uint16_t>((*dgIter++ & 0x7F) << 8 | (*dgIter++ & 0xFF));

        //std::cout << m_logTag << "MOT DG SegNum: " << +segNum << std::endl;
    }

    uint16_t transportId{0xFFFF};
    std::vector<uint8_t> endUserAddress;
    if(userAccessFlag) {
        //uint8_t rfa = (*dgIter & 0xE0) >> 6;
        bool transportIdFlag = ((*dgIter & 0x10) >> 4) != 0;
        uint8_t lengthIndicator = static_cast<uint8_t>(*dgIter++ & 0x0F);
        //std::cout << m_logTag << " MOT DG TransportIdFlg: " << std::boolalpha << transportIdFlag << std::noboolalpha << " Length: " << +lengthIndicator << std::endl;
        if(transportIdFlag) {
            transportId = static_cast<uint16_t>((*dgIter++ & 0xFF) << 8 | (*dgIter++ & 0xFF));
            //std::cout << m_logTag << "MOT DG TransportId: " << std::hex << transportId << std::dec << std::endl;
        }

        for(uint8_t i = 0; i < (lengthIndicator-2); i++) {
            //std::cout << m_logTag << "############## MOT DG EnduserAddress: " << std::hex << *dgIter << std::dec << std::endl;
            endUserAddress.push_back(*dgIter++);
        }
    }
    //Segmentation Header
    uint8_t segmentRepititionCnt = static_cast<uint8_t>((*dgIter & 0xE0) >> 5);
    uint16_t segmentSize = static_cast<uint16_t>((*dgIter++ & 0x1F) << 8 | (*dgIter++ & 0xFF));
    //std::cout << m_logTag << "MOT DG Segment RepCnt: " << +segmentRepititionCnt << ", SegmentSize: " << +segmentSize << std::endl;

    switch(datagroupType) {
        case MOT_DATAGROUP_TYPE::MOT_HEADER: {
            MOT_Data motData;
            //Header Core
            motData.motBodySize = static_cast<uint32_t>((*dgIter++ & 0xFF) << 20 | (*dgIter++ & 0xFF) << 12 | (*dgIter++ & 0xFF) << 4 | (*dgIter & 0xF0) >> 4);
            uint16_t motHeaderSize = static_cast<uint16_t>((*dgIter++ & 0x0F) << 9 | (*dgIter++ & 0xFF) << 1 | (*dgIter & 0x0F) >> 7);
            motData.motContentType = static_cast<MOT_CONTENT_TYPE >((*dgIter & 0x7E) >> 1);
            motData.motContentSubtype = static_cast<uint16_t>((*dgIter++ & 0x01) << 8 | (*dgIter++ & 0xFF));

            //std::cout << m_logTag << "MOT DG Header BodySize: " << +motData.motBodySize << ", HeaderSize: " << +motHeaderSize << ", ContentType: " << +motData.motContentType << ", ContentSubType: " << +motData.motContentSubtype << std::endl;

            //Header Extension
            while(dgIter < mscDatagroup.end() - (crcFlag ? 2 : 0)) {
                uint8_t extHdrParamLengthIndicator = static_cast<uint8_t>((*dgIter & 0xC0) >> 6);
                uint8_t extHdrParamId = static_cast<uint8_t>((*dgIter++ & 0x3F) & 0xFF);
                int extHdrParamLength{0};
                switch(extHdrParamLengthIndicator) {
                    case 0: {
                        break;
                    }
                    case 1: {
                        extHdrParamLength = 1;
                        break;
                    }
                    case 2: {
                        extHdrParamLength = 4;
                        break;
                    }
                    case 3: {
                        bool dataFieldLengthFlag = ((*dgIter & 0x80) >> 7) != 0;
                        if(dataFieldLengthFlag) {
                            extHdrParamLength = (*dgIter++ & 0x7F) << 8 | (*dgIter++ & 0xFF);
                        } else {
                            extHdrParamLength = (*dgIter++ & 0x7F) & 0xFF;
                        }
                        break;
                    }
                    default: {
                        std::cout << m_logTag << "MOT DG Header Extension unknown LengthIndex: " << +extHdrParamLengthIndicator << std::endl;
                        break;
                    }
                }

                motData.motHeaderParams.insert(std::make_pair(extHdrParamId, std::vector<uint8_t>(dgIter, dgIter + extHdrParamLength)));
                dgIter += extHdrParamLength;
            }

            m_motHdrDataMap.insert(std::make_pair(transportId, motData));
            break;
        }
        case MOT_DATAGROUP_TYPE::MOT_BODY_UNSCRAMBLED: {
            auto motDataIter = m_motHdrDataMap.find(transportId);
            if(motDataIter != m_motHdrDataMap.end()) {
                if(motDataIter->second.previousSegmentNumber + 1 == segNum) {
                    motDataIter->second.previousSegmentNumber = segNum;

                    motDataIter->second.motBodyData.insert(motDataIter->second.motBodyData.end(), dgIter, dgIter + segmentSize);

                    if(motDataIter->second.motBodyData.size() == motDataIter->second.motBodySize) {
                        switch(motDataIter->second.motContentType) {
                            case MOT_CONTENT_TYPE::IMAGE: {
                                for(const auto& appDecoder : m_uAppDecoders) {
                                    if(appDecoder->getUserApplicationType() == registeredtables::USERAPPLICATIONTYPE::MOT_SLIDESHOW) {
                                        appDecoder->motApplicationDataInput(motDataIter->second);
                                    }
                                }

                                break;
                            }
                            default: {
                                std::cout << m_logTag << "Unhandled MOT_CONTENT_TYPE: " << +motDataIter->second.motContentType << " -------------------" << std::endl;
                                break;
                            }
                        }
                    }

                    if (isLast) {
                        //std::cout << m_logTag << "#### Last MOT DG Body segment, Size is: " << +motDataIter->second.motBodyData.size() << " from: " << +motDataIter->second.motBodySize << std::endl;
                        m_motHdrDataMap.erase(transportId);
                    }
                } else {
                    std::cout << m_logTag << "+++++ MOT DG SegmentNumber interruption: " << +motDataIter->second.previousSegmentNumber << " : " << segNum << std::endl;
                    m_motHdrDataMap.erase(transportId);
                }
            }
            break;
        }
        default: {
            std::cout << m_logTag << "Unsupported MOT Datagroup type: " << +datagroupType << std::endl;
        }
    }
}

void MotDecoder::addUserapplicationDecoder(std::shared_ptr<DabMotUserapplicationDecoder> uAppDecoder) {
    m_uAppDecoders.push_back(uAppDecoder);
}

