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

#ifndef FIG_00_EXT_14_H
#define FIG_00_EXT_14_H

#include <vector>

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.2.2 FEC sub-channel organization
 *
 * The Extension 14 of FIG type 0 (FIG 0/14) defines the additional Forward Error Correcting scheme applied to
 * sub-channels carrying packet mode service components
 */
class Fig_00_Ext_14 : public Fig_00 {

public:
    /*
     * FEC Scheme: this 2-bit field shall indicate the Forward Error Correction scheme in use, as follows:
     * b1 - b0;
     * 0    0: no FEC scheme applied (legacy support only);
     * 0    1: FEC scheme applied according to clause 5.3.5;
     * 1    0: reserved for future definition;
     * 1    1: reserved for future definition.
     */
    enum FEC_SCHEME {
        NOT_APPLIED,
        APPLIED,
        RESERVED0,
        RESERVED1
    };

    struct FecSchemeDescription {
        inline bool operator==(const FecSchemeDescription& other) const {
            return other.subChannelId == subChannelId && \
                   other.fecScheme == fecScheme;
        }
        inline bool operator!=(const FecSchemeDescription& other) const { return !operator==(other); }

        uint8_t subChannelId;
        Fig_00_Ext_14::FEC_SCHEME fecScheme;
    };

public:
    explicit Fig_00_Ext_14(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_14();

    virtual const std::vector<Fig_00_Ext_14::FecSchemeDescription>& getFecSchemeDescriptions() const;

public:
    inline bool operator==(const Fig_00_Ext_14& other) const {
        if(m_descriptions.size() != other.m_descriptions.size()) {
            return false;
        }
        for(int i = 0; i < m_descriptions.size(); i++) {
            if(m_descriptions[i] != other.m_descriptions[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_14& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_14]"};

    std::vector<FecSchemeDescription> m_descriptions;
};

#endif // FIG_00_EXT_14_H
