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

#ifndef FIG_00_EXT_17_H
#define FIG_00_EXT_17_H

#include <vector>

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.5 Programme Type
 *
 * The Programme Type (PTy) feature allows programme contents to be categorized according to their intended audience.
 *
 * The Programme Type feature is encoded in Extension 17 of FIG type 0 (FIG 0/17). The Programme Type code applies
 * to all the audio components of the service. Figure 41 shows the structure of Programme Type field which is part of the Type 0 field
 *
 * The FIG 0/17 has a repetition rate of once per second.
 */
class Fig_00_Ext_17 : public Fig_00 {

public:
    struct ProgrammeTypeInformation {
        inline bool operator==(const ProgrammeTypeInformation& other) const {
            return other.serviceId == serviceId && \
                   other.isDynamic == isDynamic && \
                   other.intPtyCode == intPtyCode;
        }
        inline bool operator!=(const ProgrammeTypeInformation& other) const { return !operator==(other); }

        uint16_t serviceId;
        bool isDynamic;
        uint8_t intPtyCode;
    };

public:
    explicit Fig_00_Ext_17(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_17();

    virtual const std::vector<Fig_00_Ext_17::ProgrammeTypeInformation>& getProgrammeTypeInformations() const;

public:
    inline bool operator==(const Fig_00_Ext_17& other) const {
        if(m_ptyInformations.size() != other.m_ptyInformations.size()) {
            return false;
        }
        for(int i = 0; i < m_ptyInformations.size(); i++) {
            if(m_ptyInformations[i] != other.m_ptyInformations[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_17& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_17]"};

    std::vector<Fig_00_Ext_17::ProgrammeTypeInformation> m_ptyInformations;
};

#endif // FIG_00_EXT_17_H
