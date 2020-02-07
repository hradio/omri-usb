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

#ifndef FIG_00_EXT_24_H
#define FIG_00_EXT_24_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.10 OE Services
 *
 * The OE Services feature is used to identify the services currently carried in other DAB ensembles.
 * The feature is encoded in Extension 24 of FIG type 0 (FIG 0/24).
 *
 * If the service is carried in the tuned ensemble the OE flag shall be set to "0" If the service is not carried in the tuned
 * ensemble the OE flag shall be set to "1".
 *
 * This feature shall use the SIV signalling (see clause 5.2.2.1). The database shall be divided by use of a database key.
 * Changes to the database shall be signalled using the CEI.
 *
 * OE Services forms part of the signalling used for service following, which is described in detail in ETSI TS 103 176
 *
 * The database key comprises the OE and P/D flags (see clause 5.2.2.1) and the SId field.
 *
 * The Change Event Indication (CEI) is signalled by the Number of EIds field = 0.
 *
 * The repetition rates for FIG 0/24 are provided in ETSI TS 103 176.
 */
class Fig_00_Ext_24 : public Fig_00 {

public:
    struct OtherEnsembleServiceInformation {
        uint32_t serviceId{0x00};
        uint8_t caId{0x00};
        std::vector<uint16_t> ensembleIds;
        bool isOtherEnsemble{false};
        bool isCei{false};
    };

public:
    explicit Fig_00_Ext_24(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_24();

    const std::vector<OtherEnsembleServiceInformation>& getOtherEnsembleServiceInformations() const { return m_oeSrvInfos; };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_24]"};

    std::vector<OtherEnsembleServiceInformation> m_oeSrvInfos;
};

#endif // FIG_00_EXT_24_H
