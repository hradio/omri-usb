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

#ifndef FIG_00_EXT_20_H
#define FIG_00_EXT_20_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.4 Service Component Information
 *
 * The Service Component Information (SCI) feature is used to signal information about services and service components
 * to assist in the construction of a service list. The feature is encoded in Extension 20 of FIG type 0 (FIG 0/20).
 *
 * The use of the Service Component Information feature is described in detail in ETSI TS 103 176
 */
class Fig_00_Ext_20 : public Fig_00 {

public:
    explicit Fig_00_Ext_20(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_20();

public:
    /*
     * Change flags: this 2-bit field shall indicate future changes to a service element, as follows:
     *  0 0: the service will remain in the ensemble with a new SId or will be moved to or from another ensemble;
     *  0 1: the service element will be added to the ensemble;
     *  1 0: the service element will be removed from the ensemble;
     *  1 1: the service element will be removed from all ensembles.
     */
    enum CHANGETYPE {
        REMAINS_WITH_NEW_SID,
        ADDED_TO_ENSEMBLE,
        REMOVED_FROM_ENSEMBLE,
        REMOVED_FROM_ALL_ENSEMBLES
    };

    /*
     * P-T (Part-time) flag: this 1-bit flag shall indicate whether the service element is on-air or off-air continuously or cycles
     * through on-air and off-air periods, as follows:
     *  0: the service element is on-air or off-air continuously (i.e. 24 hours/day);
     *  1: the service element cycles on-air and off-air (i.e. part-time).
     */
    enum PARTTIME {
        ON_OFF_AIR_CONTINOUSLY,
        ON_OFF_AIR_CYCLE
    };

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_20]"};
};

#endif // FIG_00_EXT_20_H
