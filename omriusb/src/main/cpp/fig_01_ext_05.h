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

#ifndef FIG_01_EXT_05_H
#define FIG_01_EXT_05_H

#include "fig_01.h"

/*
 * ETS 300 401 clause 8.1.14.2 Data service label
 *
 * The data service label feature is encoded in Extension 5 of the FIG type 1 (FIG 1/5) and FIG type 2 (FIG 2/5).
 */
class Fig_01_Ext_05 : public Fig_01 {

public:
    explicit Fig_01_Ext_05(const std::vector<uint8_t>& figData);
    virtual ~Fig_01_Ext_05();

    virtual uint32_t getDataServiceId() const;
    virtual std::string getDataServiceLabel() const;
    virtual std::string getDataServiceShortLabel() const;

    inline bool operator==(const Fig_01_Ext_05& other) const {
        return other.getDataServiceId() == m_dataSId && \
                   other.getDataServiceLabel() == m_srvLabel && \
                   other.getDataServiceShortLabel() == m_srvShortLabel;
    }
    inline bool operator!=(const Fig_01_Ext_05& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_01_Ext_05]"};

    uint32_t m_dataSId;
    std::string m_srvLabel;
    std::string m_srvShortLabel;
};

#endif // FIG_01_EXT_05_H
