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

#ifndef FIG_00_EXT_25_H
#define FIG_00_EXT_25_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.6.3 OE Announcement support
 *
 * The OE announcement support feature is encoded in Extension 25 of FIG type 0 (FIG 0/25).
 * Figure 44 shows the structure of the OE announcement support field which is part of the Type 0 field.
 * FIG 0/25 signals the permissions for services carried in the tuned ensemble to be interrupted by announcements carried in other ensembles.
 *
 * The FIG 0/25 has a repetition rate of once per second.
 */
class Fig_00_Ext_25 : public Fig_00 {

public:
    explicit Fig_00_Ext_25(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_25();

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_25]"};
};

#endif // FIG_00_EXT_25_H
