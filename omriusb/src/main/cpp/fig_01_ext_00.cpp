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

#include "fig_01_ext_00.h"

Fig_01_Ext_00::Fig_01_Ext_00(const std::vector<uint8_t>& figData) : Fig_01(figData) {
    parseFigData(figData);
}

Fig_01_Ext_00::~Fig_01_Ext_00() {

}

void Fig_01_Ext_00::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        m_ensembleId = static_cast<uint16_t>((*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF));

        parseLabel(figIter, m_ensembleLabel, m_ensembleShortLabel);
    }
}

uint16_t Fig_01_Ext_00::getEnsembleId() const {
    return m_ensembleId;
}

std::string Fig_01_Ext_00::getEnsembleLabel() const {
    return m_ensembleLabel;
}

std::string Fig_01_Ext_00::getEnsembleShortLabel() const {
    return m_ensembleShortLabel;
}
