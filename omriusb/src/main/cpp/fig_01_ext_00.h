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

#ifndef FIG_01_EXT_00_H
#define FIG_01_EXT_00_H

#include "fig_01.h"

/*
 * ETS 300 401 clause 8.1.13 Ensemble label
 *
 * The ensemble label feature is used to identify the ensemble in a textual format. The feature is encoded in Extension 0 of
 * the FIG type 1 (FIG 1/0) and FIG type 2 (FIG 2/0).
 */
class Fig_01_Ext_00 : public Fig_01 {

public:
    explicit Fig_01_Ext_00(const std::vector<uint8_t>& figData);
    virtual ~Fig_01_Ext_00();

    virtual uint16_t getEnsembleId() const;
    virtual std::string getEnsembleLabel() const;
    virtual std::string getEnsembleShortLabel() const;

    inline bool operator==(const Fig_01_Ext_00& other) const {
        return other.getEnsembleLabel() == m_ensembleLabel && \
                   other.getEnsembleId() == m_ensembleId && \
                   other.getEnsembleShortLabel() == m_ensembleShortLabel;
    }
    inline bool operator!=(const Fig_01_Ext_00& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_01_Ext_00]"};

    uint16_t m_ensembleId;
    std::string m_ensembleLabel;
    std::string m_ensembleShortLabel;
};

#endif // FIG_01_EXT_00_H
