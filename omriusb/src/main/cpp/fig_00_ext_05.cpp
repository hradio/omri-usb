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

#include "fig_00_ext_05.h"

#include <iostream>

Fig_00_Ext_05::Fig_00_Ext_05(const std::vector<uint8_t>& figData) : Fig_00(figData){
    parseFigData(figData);
}

Fig_00_Ext_05::~Fig_00_Ext_05() {

}

void Fig_00_Ext_05::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        m_isShortForm = (((*figIter & 0x80) >> 7) & 0xFF) == 0;
        if(m_isShortForm) {
            //bool rfu = ((*figIter & 0x40) >> 6) & 0xFF;
            m_subChanId = (*figIter++ & 0x3F) & 0xFF;
        } else {
            //uint8_t rfa = ((*figIter & 0x70) >> 4) & 0xFF;
            m_scid = (*figIter++ & 0x0F) << 8 | (*figIter++ & 0xFF);
        }

        m_langCode = *figIter++ & 0xFF;
        //std::cout << m_logTag << " Language: " << std::hex << +m_langCode << std::dec << " : " << registeredtables::LANGUAGE[m_langCode] << std::endl;
    }
}

uint8_t Fig_00_Ext_05::getLanguageCode() const {
    return m_langCode;
}

std::string Fig_00_Ext_05::getLanguage() const {
    if(m_langCode < registeredtables::LANGUAGE_MAX) {
        return registeredtables::LANGUAGE[m_langCode];
    } else {
        return registeredtables::LANGUAGE[0];
    }
}

bool Fig_00_Ext_05::isShortForm() const {
    return m_isShortForm;
}

uint8_t Fig_00_Ext_05::getSubchannelId() const {
    return m_subChanId;
}

uint16_t Fig_00_Ext_05::getServiceComponentId() const {
    return m_scid;
}
