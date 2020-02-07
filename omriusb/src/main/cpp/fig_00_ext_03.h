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

#ifndef FIG_00_EXT_03_H
#define FIG_00_EXT_03_H

#include <vector>

#include "fig_00.h"

/* ETS 300 401 clause 6.3.2 Service component in packet mode with or without Conditional Access
 *
 * The Extension 3 of FIG type 0 (FIG 0/3) gives additional information about the service component description in packet mode.
 */
class Fig_00_Ext_03 : public Fig_00 {

public:
    /*
     * SCId (Service Component Identifier): this 12-bit field shall uniquely identify the service component within the ensemble.
     *
     * CAOrg flag: this 1-bit flag shall indicate whether the Conditional Access Organization (CAOrg) field is present, or not, as follows:
     * 0: CAOrg field absent;
     * 1: CAOrg field present.
     *
     * CAOrg (Conditional Access Organization): this 16-bit field shall contain information about the applied Conditional
     * Access Systems and mode (see TS 102 367).
     *
     * DG flag: this 1-bit flag shall indicate whether data groups are used to transport the service component as follows:
     *  0: data groups are used to transport the service component;
     *  1: data groups are not used to transport the service component.
     *
     * DSCTy (Data Service Component Type): this 6-bit field shall indicate the type of the data service component.
     * The interpretation of this field shall be as defined in TS 101 756, table 2.
     *
     * SubChId (Sub-channel Identifier): this 6-bit field shall identify the sub-channel in which the service component is carried.
     *
     * Packet address: this 10-bit field shall define the address of the packet in which the service component is carried.
     */
    struct PacketModeServiceComponentDescription {
        inline bool operator==(const PacketModeServiceComponentDescription& other) const {
            return other.isProgrammeService == isProgrammeService && \
                   other.serviceComponentId == serviceComponentId && \
                   other.caOrganizationPresent == caOrganizationPresent && \
                   other.caOrganization == caOrganization && \
                   other.dataGroupTransportUsed == dataGroupTransportUsed && \
                   other.dataServiceComponentType == dataServiceComponentType && \
                   other.subchannelId == subchannelId && \
                   other.packetAddress == packetAddress;
        }
        inline bool operator!=(const PacketModeServiceComponentDescription& other) const { return !operator==(other); }

        bool isProgrammeService;
        uint16_t serviceComponentId;
        bool caOrganizationPresent;
        uint16_t caOrganization{0};
        bool dataGroupTransportUsed;
        uint8_t dataServiceComponentType;
        uint8_t subchannelId;
        uint16_t packetAddress;
    };

public:
    explicit Fig_00_Ext_03(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_03();

    virtual const std::vector<Fig_00_Ext_03::PacketModeServiceComponentDescription>& getPacketModeServiceDescriptions() const;

public:
    inline bool operator==(const Fig_00_Ext_03& other) const {
        if(m_srvDescs.size() != other.m_srvDescs.size()) {
            return false;
        }
        for(int i = 0; i < m_srvDescs.size(); i++) {
            if(m_srvDescs[i] != other.m_srvDescs[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_03& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_03]"};

    std::vector<Fig_00_Ext_03::PacketModeServiceComponentDescription> m_srvDescs;
};

#endif // FIG_00_EXT_03_H
