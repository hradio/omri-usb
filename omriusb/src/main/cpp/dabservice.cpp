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

#include "dabservice.h"

#include <iostream>

#include "dabservicecomponentmscpacketdata.h"

DabService::DabService() {
    //std::cout << "DabService constructed" << std::endl;
}

DabService::~DabService() {
    //std::cout << "DabService got deleted" << std::endl;

    m_components.clear();
}

uint32_t DabService::getServiceId() const {
    return m_serviceId;
}

bool DabService::isCaApplied() const {
    return m_caApplied;
}

uint8_t DabService::getCaId() const {
    return m_caId;
}

uint8_t DabService::getNumberServiceComponents() const {
    return m_numSrvComps;
}

bool DabService::isProgrammeService() const {
    return m_isProgrammeService;
}

uint8_t DabService::getLabelCharset() const {
    return m_labelCharset;
}

std::string DabService::getServiceLabel() const {
    return m_serviceLabel;
}

std::string DabService::getServiceShortLabel() const {
    return m_serviceShortLabel;
}

const std::vector<std::shared_ptr<DabServiceComponent>>& DabService::getServiceComponents() const {
    return m_components;
}

uint8_t DabService::getProgrammeTypeCode() const {
    return m_ptyCode;
}

bool DabService::isProgrammeTypeDynamic() const {
    return m_ptyIsDynamic;
}

std::string DabService::getProgrammeTypeFullName() const {
    return m_ptyNameFull;
}

std::string DabService::getProgrammeType16charName() const {
    return m_ptyName16;
}

std::string DabService::getProgrammeType8CharName() const {
    return m_ptyName8;
}

uint32_t DabService::getEnsembleFrequency() const {
    return m_ensembleFrequency;
}

void DabService::setServiceId(uint32_t serviceId) {
    m_serviceId = serviceId;
}

void DabService::setIsProgrammeService(bool isProgramme) {
    m_isProgrammeService = isProgramme;
}

void DabService::setCaId(uint8_t caId) {
    m_caId = m_caApplied = caId;
}

void DabService::setNumberOfServiceComponents(uint8_t numSc) {
    m_numSrvComps = numSc;
}

void DabService::setLabelCharset(uint8_t charset) {
    m_labelCharset = charset;
}

void DabService::setServiceLabel(const std::string& label) {
    if(m_serviceLabel.empty()) {
        m_serviceLabel = label;

        std::cout << m_logTag << " Setting ServiceLabel: " << m_serviceLabel << " to SId: " << std::hex << m_serviceId << std::dec << std::endl;
    }
}

void DabService::setServiceShortLabel(const std::string& shortLabel) {
    if(m_serviceShortLabel.empty()) {
        m_serviceShortLabel = shortLabel;

        std::cout << m_logTag << " Setting ServiceShortlabel: " << m_serviceShortLabel << " to SId: " << std::hex << m_serviceId << std::dec << std::endl;
    }
}

void DabService::addServiceComponent(std::shared_ptr<DabServiceComponent> component) {
    m_components.push_back(component);
    std::cout << m_logTag << " Adding ServicecomponentPtr with SubChanId: " << std::hex << +component->getSubChannelId() << " for SId: " << +m_serviceId << std::dec << " as Servicecomponent#: " << m_components.size() << std::endl;
}

void DabService::setProgrammeTypeCode(uint8_t intPtyCode) {
    if(m_ptyCode != intPtyCode && intPtyCode < 32) {
        m_ptyCode = intPtyCode;
        m_ptyNameFull = registeredtables::PROGRAMME_TYPE_NAME[m_ptyCode][0];
        m_ptyName16 = registeredtables::PROGRAMME_TYPE_NAME[m_ptyCode][1];
        m_ptyName8 = registeredtables::PROGRAMME_TYPE_NAME[m_ptyCode][2];

        std::cout << m_logTag << " Setting " << (m_ptyIsDynamic ? "dynamic" : "static") << " PTY for SId: " << std::hex << m_serviceId << std::dec << " to: " << +m_ptyCode << " : " << m_ptyNameFull << " : " << m_ptyName16 << " : " << m_ptyName8 << std::endl;
    }
}

void DabService::setProgrammeTypeIsDynamic(bool dynamic) {
    m_ptyIsDynamic = dynamic;
}

void DabService::setEnsembleFrequency(uint32_t ensembleFrequency) {
    m_ensembleFrequency = ensembleFrequency;
}

