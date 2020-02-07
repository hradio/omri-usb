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

#ifndef FIG_00_EXT_06_H
#define FIG_00_EXT_06_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.15 Service linking information
 *
 * The Service linking information feature provides service linking information for use when services carry the same
 * primary service component (hard link) or when the primary service components are related (soft link). The feature is
 * encoded in extension 6 of FIG type 0 (FIG 0/6).
 */
class Fig_00_Ext_06 : public Fig_00 {

public:
    /*
     * IdLQ (Identifier List Qualifier): this 2-bit field shall indicate how the identifiers, contained in the Id list, except for
     * the key service, are qualified, as follows
     */
    enum SERVICE_LINKING_IDENTIFIER_LIST_QUALIFIER {
        DAB_SID = 0,
        RDS_PI = 1,
        RFU = 2,
        DRM_AMSS_SID = 3
    };

    struct ServiceLinkList {
        /*
         * IdLQ (Identifier List Qualifier): this 2-bit field shall indicate how the identifiers, contained in the Id list, except for the key service, are qualified, as follows:
         *   b6 - b5;
         *   0    0 : each Id represents a DAB SId (see clause 6.3.1);
         *   0    1 : each Id represents an RDS PI-code (see IEC 62106 [10]);
         *   1    0 : reserved for future use;
         *   1    1 : each Id represents all or part of a DRM Service Identifier (see ETSI ES 201 980 [8]) or all or part of an AMSS Service Identifier (see ETSI TS 102 386).
         */
        SERVICE_LINKING_IDENTIFIER_LIST_QUALIFIER idListQualifier;

        /*
         * The identifier for the key service, which shall always be a DAB SId of a service carried in the ensemble, is the first
         * identifier in the Id list at the start of the database; this is when the version number of the type 0 field is set to "0" (see clause 5.2.2.1).
         *
         * Depending on isIls, isDataService and idListQualifier the size of id's are:
         * if(isIls == false && isDataService == false)
         *   id's are 16 bit
         *
         * if(isIls == true && isDataService == false)
         *   id's are 24 bit (8 bit ECC and 16 bit ID)
         *
         * if(isDataService == true && (isIls == true || isIls == false) )
         *   id's are 32 bit
         */
        std::vector<uint32_t> idList;


        inline bool operator==(const ServiceLinkList& other) const {
            //return std::includes(frequenciesKHz.begin(), frequenciesKHz.end(), otherFreqList.begin(), otherFreqList.end());
            return idListQualifier == other.idListQualifier &&
                   std::includes(idList.begin(), idList.end(), other.idList.begin(), other.idList.end());
        }

        inline bool operator!=(const ServiceLinkList& other) const { return !operator==(other); }

        bool containsId(uint32_t otherId) {
            if(std::find(idList.begin(), idList.end(), otherId) != idList.end()) {
                return true;
            }

            return false;
        }
    };

    struct ServiceLinkingInformation {

        bool isDataService{false};

        bool isChangeEvent{false};

        bool isContinuation{false};
        /*
         * LA (Linkage Actuator): this 1-bit flag shall indicate whether the link is active or inactive (potential), as follows:
         *   0: potential future link or de-activated link;
         *   1: active link.
         */
        bool linkageActive{false};

        /*
         * S/H (Soft/Hard): this 1-bit flag shall indicate whether the link is soft or hard, as follows:
         *   0: Soft link (related services);
         *   1: Hard link (services carrying the same primary service component).
         */
        bool isSoftLink{false};

        /*
         * ILS (International linkage set indicator): this 1-bit flag shall indicate whether the link affects only one country
         * (national) or several countries (international), as follows:
         *   0: national link;
         *   1: international link (or DRM or AMSS).
         *
         * NOTE: If a linkage set includes services broadcast using DRM or AMSS, then the link will always be an
         * international link, even if all linked services are intended for national use.
         */
        bool isIls{false};

        /*
         * LSN (Linkage Set Number): this 12-bit field represents a number which shall be common to all Services linked
         * together as a set. The use of LSN = "0000 0000 0000" is reserved.
         */
        uint16_t linkageSetNumber{0x0000};

        /* The database key comprises the OE and P/D flags (see clause 5.2.2.1) and the S/H, ILS, and LSN fields. */
        uint16_t linkDbKey{0x00};

        /*
         * The identifier for the key service, which shall always be a DAB SId of a service carried in the ensemble,
         * is the first identifier in the Id list at the start of the database;
         * this is when the version number of the type 0 field is set to "0"
         */
        uint32_t keyServiceId{0x00};

        std::vector<Fig_00_Ext_06::ServiceLinkList> serviceLinks;

        inline bool containsServiceLinkList(const Fig_00_Ext_06::ServiceLinkList& linkList) const {
            return std::find(serviceLinks.begin(), serviceLinks.end(), linkList) != serviceLinks.end();
        }
    };

public:
    explicit Fig_00_Ext_06(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_06();

    const std::vector<Fig_00_Ext_06::ServiceLinkingInformation>& getServiceLinkingInformations() const { return m_sli; };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_06]"};

    std::vector<Fig_00_Ext_06::ServiceLinkingInformation> m_sli;
};

#endif // FIG_00_EXT_06_H
