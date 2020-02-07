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

#ifndef FIG_00_EXT_26_H
#define FIG_00_EXT_26_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.6.4 OE Announcement switching
 *
 * The OE announcement switching description is encoded in Extension 26 of FIG type 0 (FIG 0/26).
 * Figure 45 shows the structure of the OE announcement switching field which is part of the Type 0 field.
 * FIG 0/26 signals the properties of an announcement that is carried in another ensemble.
 *
 * At the start of an announcement, FIG 0/26 shall be signalled with a repetition rate of ten times per second for the first
 * five seconds and then at a rate of once per second for the duration of the announcement.
 *
 * At the end of the announcement, the FIG 0/26 shall be signalled with the ASw flags set to all zero with a repetition rate of
 * ten times per second for two seconds.
 */
class Fig_00_Ext_26 : public Fig_00 {

public:
    explicit Fig_00_Ext_26(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_26();

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_26]"};
};

#endif // FIG_00_EXT_26_H
