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

#ifndef FIG_01_EXT_01_H
#define FIG_01_EXT_01_H

#include "fig_01.h"

/*
 * ETS 300 401 clause 8.1.14.1 Programme service label
 *
 * The programme service label feature is encoded in Extension 1 of the FIG type 1 (FIG 1/1) and FIG type 2 (FIG 2/1).
 * Figure 49 shows the structure of the Identifier field for the service label which is part of the Type 1 field.
 */
class Fig_01_Ext_01 : public Fig_01 {

public:
    explicit Fig_01_Ext_01(const std::vector<uint8_t>& figData);
    virtual ~Fig_01_Ext_01();

    virtual uint16_t getProgrammeServiceId() const;
    virtual std::string getProgrammeServiceLabel() const;
    virtual std::string getProgrammeServiceShortLabel() const;

    inline bool operator==(const Fig_01_Ext_01& other) const {
        return other.getProgrammeServiceId() == m_programmeSId && \
                   other.getProgrammeServiceLabel() == m_srvLabel && \
                   other.getProgrammeServiceShortLabel() == m_srvShortLabel;
    }
    inline bool operator!=(const Fig_01_Ext_01& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_01_Ext_01]"};

    uint16_t m_programmeSId;
    std::string m_srvLabel;
    std::string m_srvShortLabel;
};

#endif // FIG_01_EXT_01_H
