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

#include "fig_00_ext_00.h"

#include <iostream>

Fig_00_Ext_00::Fig_00_Ext_00(const std::vector<uint8_t> &figData) : Fig_00(figData) {

    parseFigData(figData);
}

Fig_00_Ext_00::~Fig_00_Ext_00() {

}

uint16_t Fig_00_Ext_00::getEnsembleId() const {
    return m_EnsembleId;
}

Fig_00_Ext_00::CHANGE_FLAG Fig_00_Ext_00::getChangeFlag() const {
    return m_ChangeFlag;
}

bool Fig_00_Ext_00::isAlarmSupported() const {
    return m_AlarmSupported;
}

uint8_t Fig_00_Ext_00::getCifCountHigh() const {
    return m_CifCntHi;
}

uint8_t Fig_00_Ext_00::getCifCountLow() const {
    return m_CifCntLo;
}

uint8_t Fig_00_Ext_00::getOccurenceChange() const {
    return m_OccChange;
}

void Fig_00_Ext_00::parseFigData(const std::vector<uint8_t>& figData) {

    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        m_EnsembleId = static_cast<uint16_t>(((*figIter++ & 0xFF) << 8) | (*figIter++ & 0xFF));
        m_ChangeFlag = static_cast<Fig_00_Ext_00::CHANGE_FLAG>((*figIter & 0xC0) >> 6);
        m_AlarmSupported = ((*figIter & 0x20) >> 5) != 0;
        m_CifCntHi = static_cast<uint8_t>((*figIter++ & 0x1F) & 0xFF);
        m_CifCntLo = static_cast<uint8_t>(*figIter++ & 0xFF);

        if(m_ChangeFlag != CHANGE_FLAG::NO_CHANGE) {
            m_OccChange = static_cast<uint8_t>(*figIter++ & 0xFF);
        }

        //std::cout << m_logTag << " EId: " << std::hex << (unsigned)m_EnsembleId << std::dec << " CifCnt: " << (unsigned)m_CifCntHi << ":" << (unsigned)m_CifCntLo << std::endl;
    }
}
