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

#ifndef FIG_00_EXT_09_H
#define FIG_00_EXT_09_H

#include "fig_00.h"

/*
 * ETS 300 401 clause 8.1.3.2 Country, LTO and International table
 *
 * The Country, LTO and International table feature defines the local time offset, the International Table and the Extended
 * Country Code (ECC) for programme services (the ECC is already part of the data service SId field). The feature is
 * encoded in Extension 9 of FIG type 0 (FIG 0/9). Figure 39 shows the structure of the country, LTO and international
 * table field which is part of the Type 0 field.
 *
 * The FIG 0/9 has a repetition rate of once per second.
 */
class Fig_00_Ext_09 : public Fig_00 {

public:
    explicit Fig_00_Ext_09(const std::vector<uint8_t>& figData);
    virtual ~Fig_00_Ext_09();

    /*
     * Ensemble LTO (Local Time Offset): this 6-bit field shall give the Local Time Offset (LTO) for the ensemble. It is
     * expressed in multiples of half hours in the range -15,5 hours to +15,5 hours.
     *
     * Bit b5 shall give the sense of the LTO, as follows:
     * 0: positive offset;
     * 1: negative offset.
     */
    virtual int getLocalTimeOffset() const;

    /*
     * Ensemble ECC (Extended Country Code): this 8-bit field shall make the Ensemble Id unique worldwide. The ECC
     * shall be as defined in ETSI TS 101 756, tables 3 to 7
     */
    virtual uint8_t getEnsembleEcc() const;

    /*
     * Inter. (International) Table Id: this 8-bit field shall be used to select an international table.
     * The interpretation of this field shall be as defined in ETSI TS 101 756, table 11.
     */
    virtual uint8_t getInternationalTableId() const;

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_00_Ext_09]"};

    int m_lto;
    uint8_t m_ensembleEcc;
    uint8_t m_interTableId;
};

#endif // FIG_00_EXT_09_H
