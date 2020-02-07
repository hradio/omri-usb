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

#include "slideshowdecoder.h"

#include <iostream>

#include "datatypes.h"

SlideshowDecoder::SlideshowDecoder() {

}

SlideshowDecoder::~SlideshowDecoder() {

}

registeredtables::USERAPPLICATIONTYPE SlideshowDecoder::getUserApplicationType() const {
    return registeredtables::USERAPPLICATIONTYPE::MOT_SLIDESHOW;
}

void SlideshowDecoder::applicationDataInput(const std::vector<uint8_t> &appData, uint8_t dataType) {

}

void SlideshowDecoder::motApplicationDataInput(const MOT_Data& motData) {
    //std::cout << m_logTag << " ### Slideshow data input: " << +motData.motBodySize << " : " << +motData.motBodyData.size() << std::endl;

    auto slidePtr = std::make_shared<DabSlideshow>();

    for(const auto& param : motData.motHeaderParams) {
        std::cout << m_logTag << " Slideshow Header ParamID: " << std::hex << +param.first << std::dec << ", length: " << +param.second.size() << std::endl;
        switch(param.first) {
            case 0x04: {
                //ExpireTime
                //TODO expiretime parameters parsing and set slide param
                bool validityFlag = (param.second[0] & 0x80) >> 7 != 0;
                std::cout << m_logTag << " Slideshow Header ExpireTime: " << +validityFlag << std::endl;
                break;
            }
            case 0x05: {
                //Triggertime
                //TODO triggertime parameters parsing and set slide param
                bool validityFlag = (param.second[0] & 0x80) >> 7 != 0;
                std::cout << m_logTag << " Slideshow Header TriggerTime: " << +validityFlag << std::endl;
                break;
            }
            case 0x0C: {
                //Contentname
                uint8_t characterSetIndicator = static_cast<uint8_t>((param.second[0] & 0xF0) >> 4);
                slidePtr->contentName.insert(slidePtr->contentName.begin(), param.second.begin() + 1, param.second.end());
                std::cout << m_logTag << " Slideshow Header ContentName: " << slidePtr->contentName << " CharacterSet: " << +characterSetIndicator << std::endl;
                break;
            }
            case 0x25: {
                //CategoryId/SlideId
                slidePtr->categoryId = static_cast<uint8_t>(param.second[0] & 0xFF);
                slidePtr->slideId = static_cast<uint8_t>(param.second[1] & 0xFF);
                slidePtr->isCategorized = true;
                std::cout << m_logTag << " Slideshow Header CategoryID: " << +slidePtr->categoryId << ", SLideID: " << +slidePtr->slideId << std::endl;
                break;
            }
            case 0x26: {
                //CategoryTitle
                slidePtr->categoryTitle.insert(slidePtr->categoryTitle.begin(), param.second.begin(), param.second.end());
                std::cout << m_logTag << " Slideshow Header CategoryTitle: " << slidePtr->categoryTitle << std::endl;
                break;
            }
            case 0x27: {
                //ClickThroughURL
                slidePtr->clickThroughUrl.insert(slidePtr->clickThroughUrl.begin(), param.second.begin(), param.second.end());
                std::cout << m_logTag << " Slideshow Header ClickThroughURL: " << slidePtr->clickThroughUrl << std::endl;
                break;
            }
            case 0x28: {
                //AlternativeLocationURL
                slidePtr->alternativeLocationUrl.insert(slidePtr->alternativeLocationUrl.begin(), param.second.begin(), param.second.end());
                std::cout << m_logTag << " Slideshow Header AlternativeLocationURL: " << slidePtr->alternativeLocationUrl << std::endl;
                break;
            }
            case 0x29: {
                //Alert
                break;
            }
            default: {
                break;
            }
        }
    }

    slidePtr->slideshowData = motData.motBodyData;
    slidePtr->contentSubType = motData.motContentSubtype;
    slidePtr->mimeType = IMAGE_MIMETYPE[motData.motContentSubtype];
    std::cout << m_logTag << " Slideshow Header Subtype: " << +slidePtr->contentSubType << " MIMEType: " << slidePtr->mimeType << std::endl;

    m_userappDataDispatcher.invoke(slidePtr);
}

std::shared_ptr<DabUserapplicationDecoder::UserapplicationDataCallback> SlideshowDecoder::registerUserapplicationDataCallback(DabUserapplicationDecoder::UserapplicationDataCallback appSpecificDataCallback) {
    return m_userappDataDispatcher.add(appSpecificDataCallback);
}

void SlideshowDecoder::reset() {

}

