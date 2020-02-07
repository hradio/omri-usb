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

#ifndef FIG_00_EXT_18_H
#define FIG_00_EXT_18_H

#include <vector>

#include "fig_00.h"

#include "registered_tables.h"

/*
 * ETS 300 401 clause 8.1.6.1 Announcement support
 *
 * The announcement support feature is encoded in Extension 18 of FIG type 0 (FIG 0/18).
 * FIG 0/18 signals the permissions for services carried in the tuned ensemble to be
 * interrupted by announcements carried in the tuned ensemble.
 *
 * The FIG 0/18 has a repetition rate of once per second.
 */
class Fig_00_Ext_18 : public Fig_00 {

public:
    struct AnnouncementSupport {
        uint16_t serviceId{0x00};
        std::vector<registeredtables::ANNOUNCEMENT_TYPE> supportedAnnouncements;
        std::vector<uint8_t> clusterIds;
    };

public:
    explicit Fig_00_Ext_18(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_18();

    virtual std::vector<Fig_00_Ext_18::AnnouncementSupport> getAnnouncementSupports() const { return m_announcementSupports; };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_18]"};

    std::vector<Fig_00_Ext_18::AnnouncementSupport> m_announcementSupports;
};

#endif // FIG_00_EXT_18_H
