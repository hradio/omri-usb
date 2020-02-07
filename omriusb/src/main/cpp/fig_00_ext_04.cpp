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

#include "fig_00_ext_04.h"

Fig_00_Ext_04::Fig_00_Ext_04(const std::vector<uint8_t> &figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_04::~Fig_00_Ext_04() {

}

void Fig_00_Ext_04::parseFigData(const std::vector<uint8_t> &figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        bool rfa = ((*figIter & 0x80) >> 7) & 0xFF;
        bool rfu = ((*figIter & 0x40) >> 6) & 0xFF;
        uint8_t subChId = (*figIter++ & 0x3F) & 0xFF;
        uint16_t caOrg = (*figIter++ & 0xFF) << 8 | (*figIter++ & 0xFF);
    }
}
