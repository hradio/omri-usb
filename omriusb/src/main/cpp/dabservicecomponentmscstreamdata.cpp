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

#include "dabservicecomponentmscstreamdata.h"

#include <iostream>

DabServiceComponentMscStreamData::DabServiceComponentMscStreamData() {
    m_componentType = DabServiceComponent::SERVICECOMPONENTTYPE::MSC_STREAM_DATA;
}

DabServiceComponentMscStreamData::~DabServiceComponentMscStreamData() {

}

uint8_t DabServiceComponentMscStreamData::getDataServiceComponentType() const {
    return m_dscTy;
}

void DabServiceComponentMscStreamData::setDataServiceComponentType(uint8_t dscty) {
    m_dscTy = dscty;
}

void DabServiceComponentMscStreamData::componentMscDataInput(const std::vector<uint8_t>& mscData, bool synchronized) {
    std::cout << " MscStreamData Input ################" << std::endl;
}

void DabServiceComponentMscStreamData::flushBufferedData() {

}