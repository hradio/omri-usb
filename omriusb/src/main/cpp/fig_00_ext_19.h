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

#ifndef FIG_00_EXT_19_H
#define FIG_00_EXT_19_H

#include <vector>

#include "fig_00.h"

#include "registered_tables.h"

/*
 * ETS 300 401 clause 8.1.6.2 Announcement switching
 *
 * The announcement switching description is encoded in Extension 19 of FIG type 0 (FIG 0/19). Figure 43 shows the
 * structure of the announcement switching field which is part of the Type 0 field.
 * FIG 0/19 signals the properties of an announcement that is carried in the tuned ensemble
 *
 * At the start of an announcement, FIG 0/19 shall be signalled with a repetition rate of ten times per second for the first
 * five seconds and then at a rate of once per second for the duration of the announcement.
 *
 * At the end of the announcement, the FIG 0/19 shall be signalled with the Asw flags set to all zero with a
 * repetition rate of ten times per second for two seconds
 */
class Fig_00_Ext_19 : public Fig_00 {

public:
    struct AnnouncementSwitching {
        uint8_t clusterId{0x00};
        std::vector<registeredtables::ANNOUNCEMENT_TYPE> announcementsSwitched;
        bool isNewlyIntroduced{false};
        uint8_t subchannelId{0x00};
    };

public:
    explicit Fig_00_Ext_19(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_19();

    virtual std::vector<Fig_00_Ext_19::AnnouncementSwitching> getSwitchedAnnouncements() const { return m_switchedAnnouncements; };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_19]"};

    std::vector<Fig_00_Ext_19::AnnouncementSwitching> m_switchedAnnouncements;
};

#endif // FIG_00_EXT_19_H
