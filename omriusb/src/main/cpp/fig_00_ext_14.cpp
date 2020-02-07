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

#include "fig_00_ext_14.h"

#include <iostream>

Fig_00_Ext_14::Fig_00_Ext_14(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_14::~Fig_00_Ext_14() {

}

void Fig_00_Ext_14::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;

    while(figIter < figData.cend()) {
        FecSchemeDescription desc;
        desc.subChannelId = ((*figIter & 0xFC) >> 2) & 0xFF;
        desc.fecScheme = static_cast<Fig_00_Ext_14::FEC_SCHEME>((*figIter++ & 0x03) & 0xFF);

        m_descriptions.push_back(desc);
    }
}

const std::vector<Fig_00_Ext_14::FecSchemeDescription>& Fig_00_Ext_14::getFecSchemeDescriptions() const {
    return m_descriptions;
}

