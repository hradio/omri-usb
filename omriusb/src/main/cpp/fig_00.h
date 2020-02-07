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

#ifndef FIG_00
#define FIG_00

#include <vector>
#include <string>

#include "fig.h"

class Fig_00 : public Fig {

public:
    virtual ~Fig_00() {}

    /*
     * ETS 300 401 clause 5.2.2.1
     * MCI (Multiplex Configuration Information)
     *   Current/Next (C/N): this 1-bit flag shall indicate one of two situations, as follows:
     *   the type 0 field applies to the current or the next version of the multiplex configuration, as follows:
     *   0: current configuration;
     *   1: next configuration.
     *
     * SIV (Service Information Version)
     *   The type 0 field carries information for a database. The database may be large and require more than one FIG to transport.
     *   The C/N flag indicates the Service Information Version (SIV).
     *   Certain Extensions divide the database into smaller portions each addressed by use of a database key.
     *   When the database is defined, the C/N flag is used to indicate the first FIG carrying data for the database or subsequent FIGs.
     *   Any change to the contents of the database is signalled by sending a Change Event Indication (CEI) which is a FIG with certain
     *   parameters set to particular values.
     *
     *   When the database is being defined, the C/N flag is used as follows:
     *     0: start of database;
     *     1: continuation of database.
     *
     *   When a change to the database needs to be signalled, using the CEI, or a control function needs to be signalled, the C/N flag is used as follows:
     *     0: change event;
     *     1: control function.
     *
     *   The database key and CEI are defined individually for each Extension.
     */
    virtual bool isNextConfiguration() const { return m_IsNextConfiguration; }

    /*
     * Other Ensemble (OE): this 1-bit flag shall indicate whether the information is related to this or another ensemble, as follows
     * 0: this ensemble;
     * 1: other ensemble (or FM or AM or DRM service).
     */
    virtual bool isOtherEnsemble() const { return m_IsOtherSensemble; }

    /*
     * P/D: this 1-bit flag shall indicate whether the Service Identifiers (SIds) are in the 16-bit or 32-bit format, as follows:
     * 0: 16-bit SId, used for programme services;
     * 1: 32-bit SId, used for data services.
     * When the P/D flag is not used, the Service Identifier (SId) takes the 16-bit format.
     */
    virtual bool isDataService() const { return m_IsDataService; }

protected:
    Fig_00(const std::vector<uint8_t>& figData) : m_IsNextConfiguration((figData[0] & 0x80) >> 7), m_IsOtherSensemble((figData[0] & 0x40) >> 6), m_IsDataService((figData[0] & 0x20) >> 5){}

private:
    const bool m_IsNextConfiguration;
    const bool m_IsOtherSensemble;
    const bool m_IsDataService;
};

#endif // FIG_00

