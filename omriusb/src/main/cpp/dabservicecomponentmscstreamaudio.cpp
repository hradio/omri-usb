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

#include "dabservicecomponentmscstreamaudio.h"
#include "dabmpegservicecomponentdecoder.h"

#include <iostream>

DabServiceComponentMscStreamAudio::DabServiceComponentMscStreamAudio() {
    m_componentType = DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_AUDIO;

    //Adding a Dynamic Label userapplication
    DabUserApplication userApp;
    userApp.setIsCaApplied(m_isCaApplied);
    userApp.setIsDataGroupsUsed(true);
    userApp.setUserApplicationType(registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL);
    userApp.setIsXpadApp(true);

    this->addUserApplication(userApp);
}

DabServiceComponentMscStreamAudio::~DabServiceComponentMscStreamAudio() {
     //std::cout << "############## DabServiceComponent got deleted #################" << std::endl;
}

uint8_t DabServiceComponentMscStreamAudio::getAudioServiceComponentType() const {
    return m_ascTy;
}

void DabServiceComponentMscStreamAudio::setAudioServiceComponentType(uint8_t ascty) {
    m_ascTy = ascty;
    if(m_ascTy == 0x3F) {
        std::shared_ptr<DabPlusServiceComponentDecoder> componentDecoder = std::make_shared<DabPlusServiceComponentDecoder>();
        componentDecoder->setSubchannelBitrate(m_subChanBitrate);
        m_padCallback = componentDecoder->registerPadDataCallback(std::bind(&PadDecoder::padDataInput, &m_padDecoder, std::placeholders::_1));

        m_componentDecoder = componentDecoder;
    }
    if(m_ascTy == 0x00) {
        std::cout << m_logTag << " ############## Creating MPEG component decoder: " << +m_subChanBitrate << std::endl;
        std::shared_ptr<DabMpegServiceComponentDecoder> componentDecoder = std::make_shared<DabMpegServiceComponentDecoder>();
        componentDecoder->setSubchannelBitrate(static_cast<uint16_t>(m_subChanBitrate));
        m_padCallback = componentDecoder->registerPadDataCallback(std::bind(&PadDecoder::padDataInput, &m_padDecoder, std::placeholders::_1));

        m_componentDecoder = componentDecoder;
    }
}

void DabServiceComponentMscStreamAudio::addUserApplication(const DabUserApplication &uApp) {
    for(const DabUserApplication& app : m_userApplications) {
        if(app == uApp) {
            //std::cout << m_logTag << " UserApp already in list..." << std::endl;
            return;
        }
    }

    std::cout << m_logTag << " ############## Adding UserApplicationType: " << +uApp.getUserApplicationType() << " with DataServiceComponentType: " << +uApp.getDataServiceComponentType() << " for SubChanId: " << std::hex << +m_subChanId << std::dec << std::endl;

    m_padDecoder.addUserApplication(std::make_shared<DabUserApplication>(uApp));
    m_padDecoder.addUserApplicationDecoder(uApp.getUserApplicationDecoder());
    m_userApplications.push_back(uApp);
}

void DabServiceComponentMscStreamAudio::setSubchannelBitrate(uint16_t bitrateKbits) {
    DabServiceComponentMscStream::setSubchannelBitrate(bitrateKbits);
    if(m_componentDecoder != nullptr) {
        if(m_ascTy == 0x3F) {
            m_componentDecoder->setSubchannelBitrate(m_subChanBitrate);
        }
        if(m_ascTy == 0x00) {
            m_componentDecoder->setSubchannelBitrate(static_cast<uint16_t>(m_subChanBitrate));
        }
    }
}

void DabServiceComponentMscStreamAudio::setSubchannelId(uint8_t subChanId) {
    DabServiceComponentMscStream::setSubchannelId(subChanId);

    if(m_componentDecoder != nullptr) {
        if(m_ascTy == 0x3F) {
            m_componentDecoder->setSubchannelBitrate(m_subChanBitrate);
        }
        if(m_ascTy == 0x00) {
            m_componentDecoder->setSubchannelBitrate(static_cast<uint16_t>(m_subChanBitrate));
        }
    }
}

void DabServiceComponentMscStreamAudio::componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) {
    //std::cout << m_logTag << " MSC Data input: " << std::hex << +mscData.data()[0] << std::dec << std::endl;

    if(m_subChanId != 0xFF) {
        if(m_componentDecoder != nullptr) {
            m_componentDecoder->componentDataInput(mscData, synchronized);
        }
    }
}

void DabServiceComponentMscStreamAudio::flushBufferedData() {
    if(m_componentDecoder != nullptr) {
        m_componentDecoder->flushBufferedData();
    }

    m_padDecoder.reset();
}

std::shared_ptr<DabServiceComponentMscStreamAudio::AUDIO_DATA_CALLBACK> DabServiceComponentMscStreamAudio::registerAudioDataCallback(DabServiceComponentMscStreamAudio::AUDIO_DATA_CALLBACK cb) {
    if(m_ascTy == 0x3F) {
        return (std::static_pointer_cast<DabPlusServiceComponentDecoder>(m_componentDecoder))->registerAudioDataCallback(cb);
    } else {
        return (std::static_pointer_cast<DabMpegServiceComponentDecoder>(m_componentDecoder))->registerAudioDataCallback(cb);
    }
}
