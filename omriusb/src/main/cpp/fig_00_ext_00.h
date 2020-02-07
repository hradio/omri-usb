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

#ifndef FIG_00_EXT_00_H
#define FIG_00_EXT_00_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 6.4.1 Ensemble information
 *
 * The ensemble information contains control mechanisms which are common to all services contained in the ensemble. It
 * is used to signal the identity of the ensemble and to provide an alarm flag. Two change flags and the CIF counter (24 ms increments)
 * permit the management of multiplex reconfigurations.
 *
 * In any 96 ms period, the FIG 0/0 shall be transmitted in a fixed time position. In transmission mode I, this shall be the
 * first FIB (of the three) associated with the first CIF (of the four) in the transmission frame.
 * The FIG 0/0 shall be the first FIG in the FIB.
 */
class Fig_00_Ext_00 : public Fig_00 {

public:
    /*
     * Change flags: this 2-bit field shall be used to indicate whether there is to be a change to the multiplex configuration, as follows:
     * b15 - b14;
     *  0  -  0  : no change, no occurrence change field present;
     *  0  -  1  : next sub-channel organization only signalled (legacy support only);
     *  1  -  0  : next service organization only signalled (legacy support only);
     *  1  -  1  : complete next MCI (sub-channel organization and service organization) signalled.
     *
     * Transmission systems shall only indicate either Change flags = 0 0 or 1 1.
     */
    enum CHANGE_FLAG {
        NO_CHANGE,
        NEXT_SUBCHANNEL_ORGANIZATION,
        NEXT_SERVICE_ORAGNIZATION,
        COMPLETE_NEXT_MCI,
        CHANGE_FLAG_UNKNOWN
    };

public:
    explicit Fig_00_Ext_00(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_00();

    /*
     * EId (Ensemble Identifier): a unique 16-bit code, shall be allocated to the ensemble and allows unambiguous
     * identification of the ensemble when associated with the Ensemble ECC (see clause 8.1.3).
     *  - Country Id (Country Identification)
     *  - Ensemble reference: this 12-bit field shall indicate the number of the Ensemble allocated for use within a national area.
     */
    virtual uint16_t getEnsembleId() const;

    virtual Fig_00_Ext_00::CHANGE_FLAG getChangeFlag() const;

    /*
     * Al flag (Alarm flag): this 1-bit flag shall be used to signal that the ensemble supports the provision of alarm announcements, as follows:
     *  0: alarm announcements shall not interrupt any service;
     *  1: alarm announcements shall interrupt all services.
     *
     * Alarm announcements require careful management, since when support is enabled using the Al flag, the alarm
     * announcements signalled with FIG 0/19 or FIG 0/26 shall interrupt all services carried in the ensemble (see clause 8.1.6).
     */
    virtual bool isAlarmSupported() const;

    /*
     * CIF count: this modulo-5 000 binary counter shall be arranged in two parts and is incremented by one at each
     * successive CIF. The higher part is a modulo-20 counter (0 to 19) and the lower part is a modulo-250 counter (0 to 249).
     */
    virtual uint8_t getCifCountHigh() const;
    virtual uint8_t getCifCountLow() const;

    /*
     * Occurrence change: this 8-bit field shall indicate the value of the lower part of the CIF counter from which the new configuration applies.
     */
    virtual uint8_t getOccurenceChange() const;

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_00]"};

    uint16_t m_EnsembleId = 0x00;
    Fig_00_Ext_00::CHANGE_FLAG m_ChangeFlag = CHANGE_FLAG::CHANGE_FLAG_UNKNOWN;
    bool m_AlarmSupported = false;
    uint8_t m_CifCntHi = 0x00;
    uint8_t m_CifCntLo = 0x00;
    uint8_t m_OccChange = 0x00;
};

#endif // FIG_00_EXT_00_H
