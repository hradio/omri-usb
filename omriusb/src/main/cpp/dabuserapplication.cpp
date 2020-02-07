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

#include "dabuserapplication.h"

#include <iostream>

#include "dynamiclabeldecoder.h"
#include "slideshowdecoder.h"

DabUserApplication::DabUserApplication() {

}

DabUserApplication::~DabUserApplication() {

}

registeredtables::USERAPPLICATIONTYPE DabUserApplication::getUserApplicationType() const {
    return m_userAppType;
}

bool DabUserApplication::isCaApplied() const {
    return m_caApplied;
}

bool DabUserApplication::isXpadApp() const {
    return m_isXpadApp;
}

uint8_t DabUserApplication::getXpadAppType() const {
    return m_xpadAppType;
}

bool DabUserApplication::dataGroupsUsed() const {
    return m_dataGroupsUsed;
}

registeredtables::DATA_SERVICE_COMPONENT_TYPE DabUserApplication::getDataServiceComponentType() const {
    return m_dataServiceComponentType;
}

uint16_t DabUserApplication::getCaOrganization() const {
    return m_caOrganization;
}

const std::vector<uint8_t>& DabUserApplication::getUserApplicationData() const {
    return m_userAppData;
}

std::shared_ptr<DabUserapplicationDecoder> DabUserApplication::getUserApplicationDecoder() const {
    return m_userAppDecoder;
}

void DabUserApplication::setUserApplicationType(registeredtables::USERAPPLICATIONTYPE uAppType) {
    m_userAppType = uAppType;

    switch (m_userAppType) {
        case registeredtables::USERAPPLICATIONTYPE::DYNAMIC_LABEL: {
            m_userAppDecoder = std::make_shared<DynamiclabelDecoder>();
            break;
        }
        case registeredtables::USERAPPLICATIONTYPE::MOT_SLIDESHOW: {
            m_userAppDecoder = std::make_shared<SlideshowDecoder>();
            break;
        }
        default:
            break;
    }
}

void DabUserApplication::setIsCaApplied(bool caApplied) {
    m_caApplied = caApplied;
}

void DabUserApplication::setIsXpadApp(bool isXpadApp) {
    m_isXpadApp = isXpadApp;
}

void DabUserApplication::setXpadAppType(uint8_t xpadAppType) {
    m_xpadAppType = xpadAppType;
}

void DabUserApplication::setIsDataGroupsUsed(bool dgUsed) {
    m_dataGroupsUsed = dgUsed;
}

void DabUserApplication::setDataServiceComonentType(registeredtables::DATA_SERVICE_COMPONENT_TYPE dscty) {
    m_dataServiceComponentType = dscty;
}

void DabUserApplication::setCaOrganization(uint16_t caOrg) {
    m_caOrganization = caOrg;
}

void DabUserApplication::setUserApplicationData(const std::vector<uint8_t>& uAppData) {
    m_userAppData = uAppData;
}

void DabUserApplication::userApplicationDataInput(const std::vector<uint8_t>& uAppData, uint8_t dataType) {
    if(m_userAppDecoder != nullptr) {
        m_userAppDecoder->applicationDataInput(uAppData, dataType);
    } else {
        std::cout << m_logTag << " No decoder for UserApp: " << +m_userAppType << " ######################################" << std::endl;
    }
}
