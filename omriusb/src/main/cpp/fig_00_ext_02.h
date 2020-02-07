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

#ifndef FIG_00_EXT_02_H
#define FIG_00_EXT_02_H

#include <vector>

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.3.1 Basic service and service component definition
 *
 * The Extension 2 of FIG type 0 (FIG 0/2) defines the basic service organization. All the service descriptions applying to
 * a service shall be contained within one field (service k) carried in a single FIG.
 */
class Fig_00_Ext_02 : public Fig_00 {

public:
    /*
     * TMId (Transport Mechanism Identifier): this 2-bit field shall indicate the transport mechanism used, as follows:
     *  b15 - b14
     *   0     0: MSC - Stream mode - audio;
     *   0     1: MSC - Stream mode - data;
     *   1     0: FIDC;
     *   1     1: MSC - Packet mode - data.
     */
    enum TMID {
        MSC_STREAM_AUDIO,
        MSC_STREAM_DATA,
        RESERVED, //obsolete FIDC
        MSC_PACKET_MODE_DATA
    };

    struct ServiceComponentDescription {
        inline bool operator==(const ServiceComponentDescription& other) const {
            return other.transportModeId == transportModeId && \
                   other.subChannelId == subChannelId && \
                   other.isPrimary == isPrimary && \
                   other.isCaApplied == isCaApplied && \
                   other.serviceComponentType == serviceComponentType && \
                   other.serviceComponentId == serviceComponentId;
        }
        inline bool operator!=(const ServiceComponentDescription& other) const { return !operator==(other); }

        TMID transportModeId;
        uint8_t subChannelId{0xFF};             //only valid for TMID::MSC_STREAM_AUDIO and TMID::MSC_STREAM_DATA
        bool isPrimary;
        bool isCaApplied;
        uint8_t serviceComponentType{0xFF};     //only valid for TMID::MSC_STREAM_AUDIO and TMID::MSC_STREAM_DATA
        uint16_t serviceComponentId{0xFFFF};    //only valid for TMID::MSC_PACKET_MODE_DATA
    };

    /*
     * SId (Service Identifier): this 16-bit or 32-bit field shall identify the service. The length of the SId shall be signalled by the P/D flag
     *
     * --------------
     * |   P/D = 0  |
     * -----------------------------------
     * |   4 bits   |        12 bits     |
     * -----------------------------------
     * | CountryID  | Service reference  |
     * -----------------------------------
     *
     * --------------
     * |   P/D = 1  |
     * ------------------------------------------------
     * |   8 bits   |   4 bits   |        20 bits     |
     * ------------------------------------------------
     * |    ECC     | CountryID  | Service reference  |
     * ------------------------------------------------
     *
     * Service Identifier description:
     * • Country Id (Identification): this 4-bit field shall be as defined in TS 101 756, tables 3 to 7.
     * • Service reference: this field shall indicate the number of the service.
     * • ECC (Extended Country Code): this 8-bit field shall be as defined in TS 101 756, tables 3 to 7
     */
    struct ServiceDescription {
        inline bool operator==(const ServiceDescription& other) const {
            if(serviceComponents.size() != other.serviceComponents.size()) {
                return false;
            }

            for(int i = 0; i < serviceComponents.size(); i++) {
                if(other.serviceComponents[i] != serviceComponents[i]) {
                    return false;
                }
            }

            return other.serviceId == serviceId && \
                   other.caId == caId && \
                   other.numServiceComponents == numServiceComponents && \
                   other.isProgrammeService == isProgrammeService;
        }
        inline bool operator!=(const ServiceDescription& other) const { return !operator==(other); }

        uint32_t serviceId;
        uint8_t caId;                           //If no ACS is used for the service, CAId is set to zero
        uint8_t numServiceComponents;
        bool isProgrammeService;
        std::vector<Fig_00_Ext_02::ServiceComponentDescription> serviceComponents;
    };

public:
    explicit Fig_00_Ext_02(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_02();

    virtual const std::vector<Fig_00_Ext_02::ServiceDescription>& getServiceDescriptions() const;

public:
    inline bool operator==(const Fig_00_Ext_02& other) const {
        if(m_serviceDescriptions.size() != other.m_serviceDescriptions.size()) {
            return false;
        }
        for(int i = 0; i < m_serviceDescriptions.size(); i++) {
            if(m_serviceDescriptions[i] != other.m_serviceDescriptions[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_02& other) const { return !operator==(other); }

public:
    //Yeah...inheritance with specialized structs would be great...but otherwise downcasting is a bitch
    /*
    struct ServiceComponentDescription {
        virtual ~ServiceComponentDescription();

        TMID transportModeId;
        bool isPrimary;
        bool isCaApplied;
    };

    struct ServiceComponentMscStream : public ServiceComponentDescription {
        uint8_t subchannelId;
    };

    struct ServiceComponentMscStreamAudio : public ServiceComponentMscStream {
        ServiceComponentMscStreamAudio() { transportModeId = MSC_STREAM_AUDIO; }
        uint8_t audioServiceComponentType; //ascty
    };

    struct ServiceComponentMscStreamData : public ServiceComponentMscStream {
        ServiceComponentMscStreamData() { transportModeId = MSC_STREAM_DATA; }
        uint8_t dataServiceComponentType; //dscty
    };

    struct ServiceComponentMscPacketData : public ServiceComponentDescription {
        ServiceComponentMscPacketData() { transportModeId = MSC_STREAM_DATA; }
        uint16_t serviceComponentId;
    };
    */

private:
    const std::string m_logTag = {"[Fig_00_Ext_02]"};

    std::vector<ServiceDescription> m_serviceDescriptions;

private:
    void parseFigData(const std::vector<uint8_t>& figData);
};

#endif // FIG_00_EXT_02_H
