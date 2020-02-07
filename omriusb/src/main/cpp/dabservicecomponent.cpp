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

#include "dabservicecomponent.h"

#include <iostream>
#include <algorithm>

DabServiceComponent::DabServiceComponent() {

}

DabServiceComponent::~DabServiceComponent() {
    std::cout << "Dabservicecomponent base deleted" << std::endl;
}

DabServiceComponent::SERVICECOMPONENTTYPE DabServiceComponent::getServiceComponentType() const {
    return m_componentType;
}

uint8_t DabServiceComponent::getSubChannelId() const {
    return m_subChanId;
}

uint8_t DabServiceComponent::getServiceComponentIdWithinService() const {
    return m_scIdS;
}

uint16_t DabServiceComponent::getMscStartAddress() const {
    return m_mscStartAddress;
}

uint16_t DabServiceComponent::getSubchannelSize() const {
    return m_subChanSize;
}

std::string DabServiceComponent::getConvolutionalCodingRate() const {
    return m_convoCodingRate;
}

std::string DabServiceComponent::getProtectionLevelString() const {
    return m_protectionLevelString;
}

uint8_t DabServiceComponent::getProtectionLevel() const {
    return m_protectionLevel;
}

uint8_t DabServiceComponent::getProtectionType() const {
    return m_protectionType;
}

uint8_t DabServiceComponent::getUepTableIndex() const {
    return m_uepTableIdx;
}

uint16_t DabServiceComponent::getSubchannelBitrate() const {
    return m_subChanBitrate;
}

bool DabServiceComponent::isFecSchemeApplied() const {
    return m_fecSchemeAplied;
}

uint8_t DabServiceComponent::getLabelCharset() const {
    return m_labelCharset;
}

std::string DabServiceComponent::getServiceComponentLabel() const {
    return m_serviceComponentLabel;
}

std::string DabServiceComponent::getServiceComponentShortLabel() const {
    return m_serviceComponentShortLabel;
}

const std::vector<DabUserApplication>& DabServiceComponent::getUserApplications() const {
    return m_userApplications;
}

void DabServiceComponent::setSubchannelId(uint8_t subChanId) {
    m_subChanId = subChanId;
}

bool DabServiceComponent::isPrimary() const {
    return m_isPrimary;
}

bool DabServiceComponent::isCaApplied() const {
    return m_isCaApplied;
}

void DabServiceComponent::setIsCaApplied(bool caApplied) {
    m_isCaApplied = caApplied;
}

void DabServiceComponent::setIsPrimary(bool primary) {
    m_isPrimary = primary;
}

void DabServiceComponent::setServiceComponentIdWithinService(uint8_t scIdS) {
    m_scIdS = scIdS;
    m_scIdS == 0 ? (m_isPrimary = true) : (m_isPrimary = false);
}

void DabServiceComponent::setMscStartAddress(uint16_t startAddress) {
    m_mscStartAddress = startAddress;
}

void DabServiceComponent::setSubchannelSize(uint16_t subChanSize) {
    m_subChanSize = subChanSize;
}

void DabServiceComponent::setConvolutionalCodingRate(std::string convo) {
    m_convoCodingRate = convo;
}

void DabServiceComponent::setProtectionLevelString(std::string protLevel) {
    m_protectionLevelString = protLevel;
}

void DabServiceComponent::setProtectionLevel(uint8_t protLevel) {
    m_protectionLevel = protLevel;
}

void DabServiceComponent::setProtectionType(uint8_t protType) {
    m_protectionType = protType;
}

void DabServiceComponent::setUepTableIndex(uint8_t tableIdx) {
    m_uepTableIdx = tableIdx;
}

void DabServiceComponent::setSubchannelBitrate(uint16_t bitrateKbits) {
    m_subChanBitrate = bitrateKbits;
}

void DabServiceComponent::setIsFecSchemeApplied(bool fecApplied) {
    m_fecSchemeAplied = fecApplied;
}

void DabServiceComponent::setLabelCharset(uint8_t charset) {
    m_labelCharset = charset;
}

void DabServiceComponent::setServiceComponentLabel(const std::string& label) {
    if(m_serviceComponentLabel.empty()) {
        m_serviceComponentLabel = label;

        std::cout << m_logTag << " Setting ServiceComponentLabel: " << m_serviceComponentLabel << " to ScIdS: " << std::hex << +m_scIdS << std::dec << std::endl;
    }
}

void DabServiceComponent::setServiceComponentShortLabel(const std::string& shortLabel) {
    if(m_serviceComponentShortLabel.empty()) {
        m_serviceComponentShortLabel = shortLabel;

        std::cout << m_logTag << " Setting ServiceComponentShortLabel: " << m_serviceComponentShortLabel << " to ScIdS: " << std::hex << +m_scIdS << std::dec << std::endl;
    }
}

void DabServiceComponent::addUserApplication(const DabUserApplication& uApp) {
    for(const DabUserApplication& app : m_userApplications) {
        if(app == uApp) {
            //std::cout << m_logTag << " UserAppType: " << +uApp.getUserApplicationType() << " already in list: " << std::hex << +m_subChanId << std::dec << std::endl;
            return;
        }
    }

    std::cout << m_logTag << " ############## Adding UserApplicationType: " << +uApp.getUserApplicationType() << " with DataServiceComponentType: " << +uApp.getDataServiceComponentType() << " for SubChanId: " << std::hex << +m_subChanId << std::dec << std::endl;
    m_userApplications.push_back(uApp);
}

void DabServiceComponent::flushBufferedData() {

}