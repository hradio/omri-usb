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

#ifndef FIG_00_EXT_13_H
#define FIG_00_EXT_13_H

#include <vector>

#include "fig_00.h"

#include "registered_tables.h"

/*
 * ETS 300 401 clause 6.3.6 User application information
 *
 * The Extension 13 of FIG type 0 (FIG 0/13) provides information to allow data applications to be associated with the
 * correct user application decoder by the receiver. Figure below shows the structure. It allocates User Application
 * information to a service component, and also allows a limited amount of application specific information to be signalled.
 */
class Fig_00_Ext_13 : public Fig_00 {

public:
    struct UserApplication {
        inline bool operator==(const UserApplication& other) const {
            return other.userAppType == userAppType && \
                   other.caApplied == caApplied && \
                   other.isXpadData == isXpadData && \
                   other.xpadAppType == xpadAppType && \
                   other.dataGroupsUsed == dataGroupsUsed && \
                   other.dataServiceComponentType == dataServiceComponentType && \
                   other.caOrganization == caOrganization && \
                   other.userAppData == userAppData;
        }
        inline bool operator!=(const UserApplication& other) const { return !operator==(other); }

        registeredtables::USERAPPLICATIONTYPE userAppType;
        bool caApplied;
        bool isXpadData;
        uint8_t xpadAppType;
        bool dataGroupsUsed;
        registeredtables::DATA_SERVICE_COMPONENT_TYPE dataServiceComponentType{registeredtables::DATA_SERVICE_COMPONENT_TYPE::UNSPECIFIED_DATA};
        uint16_t caOrganization{0x00};
        std::vector<uint8_t> userAppData;
    };

    /*
     * SId (Service Identifier): this 16-bit or 32-bit field shall identify the service (see clause 6.3.1) and the length of the SId
     * shall be signalled by the P/D flag
     *
     * SCIdS (Service Component Identifier within the Service): this 4-bit field shall identify the service component within
     * the service. The combination of the SId and the SCIdS provides a globally valid identifier for a service component.
     */
    struct UserApplicationInformation {
        inline bool operator==(const UserApplicationInformation& other) const {
            if(other.serviceID != serviceID || other.isProgrammeService != isProgrammeService || other.scIdS != scIdS || other.numUserApps != numUserApps || other.userApplications.size() != userApplications.size()) {
                return false;
            }

            for(int i = 0; i < userApplications.size(); i++) {
                if(other.userApplications[i] != userApplications[i]) {
                    return false;
                }
            }

            return true;
        }
        inline bool operator!=(const UserApplicationInformation& other) const { return !operator==(other); }

        uint32_t serviceID;
        bool isProgrammeService;
        uint8_t scIdS;
        uint8_t numUserApps;
        std::vector<Fig_00_Ext_13::UserApplication> userApplications;
    };

public:
    explicit Fig_00_Ext_13(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_13();

    const std::vector<Fig_00_Ext_13::UserApplicationInformation>& getUserApplicationInformations() const;

public:
    inline bool operator==(const Fig_00_Ext_13& other) const {
        if(m_userAppDescriptions.size() != other.m_userAppDescriptions.size()) {
            return false;
        }
        for(int i = 0; i < m_userAppDescriptions.size(); i++) {
            if(m_userAppDescriptions[i] != other.m_userAppDescriptions[i]) {
                return false;
            }
        }

        return true;
    }
    inline bool operator!=(const Fig_00_Ext_13& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_13]"};

    std::vector<UserApplicationInformation> m_userAppDescriptions;
};

#endif // FIG_00_EXT_13_H
