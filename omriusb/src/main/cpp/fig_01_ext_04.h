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

#ifndef FIG_01_EXT_04_H
#define FIG_01_EXT_04_H

#include "fig_01.h"

/*
 * ETS 300 401 clause 8.1.14.3 Service component label
 *
 * The service component label feature is encoded in Extension 4 of FIG type 1 (FIG 1/4) and FIG type 2 (FIG 2/4).
 */
class Fig_01_Ext_04 : public Fig_01 {

public:
    explicit Fig_01_Ext_04(const std::vector<uint8_t>& figData);
    virtual ~Fig_01_Ext_04();

    virtual bool isProgrammeService() const;
    virtual uint32_t getServiceId() const;
    virtual uint8_t getServiceComponentIdWithinService() const;
    virtual std::string getServiceComponentLabel() const;
    virtual std::string getServiceComponentShortLabel() const;

    inline bool operator==(const Fig_01_Ext_04& other) const {
        return other.isProgrammeService() == isProgrammeService() && \
                   other.getServiceId() == m_serviceId && \
                   other.getServiceComponentIdWithinService() == m_scIdS && \
                   other.getServiceComponentLabel() == m_srvCompLabel && \
                   other.getServiceComponentShortLabel() == m_srvCompShortLabel;
    }
    inline bool operator!=(const Fig_01_Ext_04& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_01_Ext_04]"};

    uint8_t m_scIdS;
    uint32_t m_serviceId;
    bool m_isProgramme;
    std::string m_srvCompLabel;
    std::string m_srvCompShortLabel;
};

#endif // FIG_01_EXT_04_H
