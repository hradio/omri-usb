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

#include "fig_00_ext_10.h"

#include <iostream>

Fig_00_Ext_10::Fig_00_Ext_10(const std::vector<uint8_t>& figData) : Fig_00(figData) {
    parseFigData(figData);
}

Fig_00_Ext_10::~Fig_00_Ext_10() {

}

void Fig_00_Ext_10::parseFigData(const std::vector<uint8_t>& figData) {
    auto figIter = figData.cbegin() +1;
    while(figIter < figData.cend()) {
        //bool rfu = ((*figIter & 0x80) >> 7) & 0xFF;
        uint32_t mjd = (*figIter++ & 0x7F) << 10 | (*figIter++ & 0xFF) << 2 | (((*figIter & 0xC0) >> 6) & 0xFF);
        m_leapSecondPending = ((*figIter & 0x20) >> 5) & 0xFF;
        //bool rfa = ((*figIter & 0x10) >> 4) & 0xFF;
        bool utcFlag = ((*figIter & 0x08) >> 3) & 0xFF;
        m_dabTime.hour = (*figIter++ & 0x07) << 2 | ((*figIter & 0xC0) >> 6);
        m_dabTime.minute = (*figIter++ & 0x3F) & 0xFF;

        mjd2ymd(mjd, &m_dabTime.year, &m_dabTime.month, &m_dabTime.day);

        if(utcFlag) {
            m_dabTime.second = ((*figIter & 0xFC) >> 2) & 0xFF;
            m_dabTime.milliseconds = (*figIter++ & 0x03) << 8 | (*figIter++ & 0xFF);
            //std::cout << ":" << +seconds << "." << +milliseconds;
        }

        std::tm tm = {
                m_dabTime.second,
                m_dabTime.minute,
                m_dabTime.hour,
                m_dabTime.day,
                m_dabTime.month-1,
                m_dabTime.year-1900
        };
        tm.tm_isdst = 0;

        m_dabTime.unixTimestampSeconds = _mkgmtime(&tm);
        std::cout << m_logTag << " Year: " << +m_dabTime.year << ", Month: " << +m_dabTime.month << ", Day: " << +m_dabTime.day << ", Hour: " << +m_dabTime.hour << ", Minutes: " << +m_dabTime.minute << ", Seconds: " << +m_dabTime.second << ", Millis: " << +m_dabTime.milliseconds << ", UnixTime: " << +m_dabTime.unixTimestampSeconds << std::endl;
    }
}


bool Fig_00_Ext_10::isLeapSecondPending() const {
    return m_leapSecondPending;
}

Fig_00_Ext_10::DabTime Fig_00_Ext_10::getDabTime() const {
    return m_dabTime;
}
