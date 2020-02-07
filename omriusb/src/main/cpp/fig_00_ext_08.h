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

#ifndef FIG_00_EXT_08_H
#define FIG_00_EXT_08_H

#include <vector>

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.3.5 Service component global definition
 *
 * The Extension 8 of FIG type 0 (FIG 0/8) provides information to uniquely identify service components globally (in
 * combination with the SId and ECC) and to link the service component to the service component label, X-PAD user
 * application label and user application information. Figure 24 shows the structure. FIG 0/8 shall be signalled for all
 * secondary service components, and for all primary service components which carry data applications signalled with FIG 0/13 (including PAD).
 */
class Fig_00_Ext_08 : public Fig_00 {

public:
    struct ServiceComponentGlobalDefinition {
        inline bool operator==(const ServiceComponentGlobalDefinition& other) const {
            return other.isProgrammeService == isProgrammeService && \
                   other.serviceId == serviceId && \
                   other.scIdS == scIdS && \
                   other.isShortForm == isShortForm && \
                   other.subchannelId == subchannelId && \
                   other.serviceComponentId == serviceComponentId;
        }
        inline bool operator!=(const ServiceComponentGlobalDefinition& other) const { return !operator==(other); }

        bool isProgrammeService;
        uint32_t serviceId;
        uint8_t scIdS;
        bool isShortForm;
        uint8_t subchannelId{0xFF};           //if isShortForm
        uint16_t serviceComponentId{0xFFFF};  //if !isShortForm
    };

public:
    explicit Fig_00_Ext_08(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_08();

    virtual const std::vector<Fig_00_Ext_08::ServiceComponentGlobalDefinition>& getGLobalDefinitions() const;

public:
    inline bool operator==(const Fig_00_Ext_08& other) const {
        if(m_globalDefinitions.size() != other.m_globalDefinitions.size()) {
            return false;
        }
        for(int i = 0; i < m_globalDefinitions.size(); i++) {
            if(m_globalDefinitions[i] != other.m_globalDefinitions[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_08& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_08]"};

    std::vector<Fig_00_Ext_08::ServiceComponentGlobalDefinition> m_globalDefinitions;
};

#endif // FIG_00_EXT_08_H
