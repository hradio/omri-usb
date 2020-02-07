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

#include "fig_01_ext_05.h"

#include <iostream>

Fig_01_Ext_05::Fig_01_Ext_05(const std::vector<uint8_t> &figData) : Fig_01(figData) {
    parseFigData(figData);
}

Fig_01_Ext_05::~Fig_01_Ext_05() {

}

void Fig_01_Ext_05::parseFigData(const std::vector<uint8_t> &figData) {
    auto figIter = figData.begin() +1;
    while(figIter < figData.end()) {
        m_dataSId = (*figIter++ & 0xFF) << 24 | (*figIter++ & 0xFF) << 16 | (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);

        parseLabel(std::vector<uint8_t>(figIter, figIter+18), m_srvLabel, m_srvShortLabel);

        figIter += 18;
    }
}

uint32_t Fig_01_Ext_05::getDataServiceId() const {
    return m_dataSId;
}

std::string Fig_01_Ext_05::getDataServiceLabel() const {
    return m_srvLabel;
}

std::string Fig_01_Ext_05::getDataServiceShortLabel() const {
    return m_srvShortLabel;
}
