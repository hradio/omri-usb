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

#ifndef FIG_01_EXT_06_H
#define FIG_01_EXT_06_H

#include "fig_01.h"
/*
 * ETS 300 401 clause 8.1.14.4 X-PAD user application label
 *
 * The X-PAD user application label feature is encoded in Extension 6 of FIG type 1 (FIG 1/6) and FIG type 2 (FIG 2/6).
 */
class Fig_01_Ext_06 : public Fig_01 {

public:
    explicit Fig_01_Ext_06(const std::vector<uint8_t>& figData);
    virtual ~Fig_01_Ext_06();

    virtual bool isProgrammeService() const;
    virtual uint32_t getServiceId() const;
    virtual uint8_t getServiceComponentIdWithinService() const;
    virtual uint8_t getXPadApplicationType() const;
    virtual std::string getXPadUserAppLabel() const;
    virtual std::string getXPadUserAppShortLabel() const;

    inline bool operator==(const Fig_01_Ext_06& other) const {
        return other.isProgrammeService() == m_isProgramme && \
                   other.getServiceId() == m_serviceId && \
                   other.getServiceComponentIdWithinService() == m_scIdS && \
                   other.getXPadApplicationType() == m_xpadAppType && \
                   other.getXPadUserAppLabel() == m_uappLabel && \
                   other.getXPadUserAppShortLabel() == m_uappShortLabel;
    }
    inline bool operator!=(const Fig_01_Ext_06& other) const { return !operator==(other); }

private:
    void parseFigData(const std::vector<uint8_t>& figData);

private:
    const std::string m_logTag = {"[Fig_01_Ext_06]"};

    bool m_isProgramme;
    uint32_t m_serviceId;
    uint8_t m_scIdS;
    uint8_t m_xpadAppType;
    std::string m_uappLabel;
    std::string m_uappShortLabel;
};

#endif // FIG_01_EXT_06_H
